// Fill out your copyright notice in the Description page of Project Settings.


#include "AIScene.h"

#include "AICamera.h"
#include "AILight.h"
#include "AIMesh.h"
#include "AINode.h"
#include "AIMaterial.h"
#include "AssimpMesh.h"
#include "ProceduralMeshComponent.h"
#include "assimp/cimport.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Serialization/BufferArchive.h"


UAIScene* UAIScene::InternalConstructNewScene(UObject* Parent, const aiScene* Scene, const bool DisableAutoSpaceChange)
{
	//todo check if object is already created and skip creation and return object 
	UAIScene* SceneObject = NewObject<UAIScene>(Parent, UAIScene::StaticClass(), NAME_None, RF_Transient);
	SceneObject->scene = const_cast<aiScene*>(Scene);
	//Setup Meshes
	SceneObject->OwnedMeshes.Reset();
	SceneObject->OwnedMeshes.AddUninitialized(Scene->mNumMeshes);
	SceneObject->OwnedLights.AddUninitialized(Scene->mNumLights);
	SceneObject->OwnedCameras.AddUninitialized(Scene->mNumCameras);
	SceneObject->OwnedMaterials.AddUninitialized(Scene->mNumMaterials);


        //Add Meshes
	if (Scene->HasMeshes())
	{
		for (unsigned Index = 0; Index < Scene->mNumMeshes; Index++)
		{
			UAIMesh* Mesh = NewObject<UAIMesh>(SceneObject, UAIMesh::StaticClass(), NAME_None, RF_Transient);
			Mesh->Mesh = Scene->mMeshes[Index];
			SceneObject->OwnedMeshes[Index] = Mesh;
		}
	}

	//Add Cams
	if (Scene->HasCameras())
	{
		for (unsigned Index = 0; Index < Scene->mNumCameras; Index++)
		{
			UAICamera* Camera = NewObject<UAICamera>(SceneObject, UAICamera::StaticClass(), NAME_None, RF_Transient);
			Camera->camera = Scene->mCameras[Index];
			SceneObject->OwnedCameras[Index] = Camera;
		}
	}

	//Add Lights
	if (Scene->HasLights())
	{
		for (unsigned Index = 0; Index < Scene->mNumLights; Index++)
		{
			UAILight* Light = NewObject<UAILight>(SceneObject, UAILight::StaticClass(), NAME_None, RF_Transient);
			Light->Light = Scene->mLights[Index];

			SceneObject->OwnedLights[Index] = Light;
		}
	}

	//Add Materials
	if (Scene->HasMaterials())
	{
		for (unsigned Index = 0; Index < Scene->mNumMaterials; Index++)
		{
			UAIMaterial* Material = NewObject<UAIMaterial>(SceneObject, UAIMaterial::StaticClass(), NAME_None,
			                                               RF_Transient);
			Material->Material = Scene->mMaterials[Index];
			SceneObject->OwnedMaterials[Index] = Material;
		}
	}

        //Build Node Tree
	UAINode* RootNode = NewObject<UAINode>(SceneObject, UAINode::StaticClass(), NAME_None, RF_Transient);
	SceneObject->OwnedRootNode = RootNode;


        // If assimp scene does not have UnitScaleFactor in metadata, presume 1.0f
	bool success = SceneObject->scene->mMetaData->Get("UnitScaleFactor", SceneObject->SceneScale);
        if (!success) {
            SceneObject->SceneScale = 1.0f;
            UE_LOG(LogAssimp, Warning, TEXT("No UnitScaleFactor in metadata."));
        }
        if (SceneObject->SceneScale == 0.0f) {
            SceneObject->SceneScale = 1.0f;
            UE_LOG(LogAssimp, Warning, TEXT("Zero UnitScaleFactor replaced with 1.0."));
        }
        UE_LOG(LogAssimp, Warning, TEXT("UnitScaleFactor: %g"), SceneObject->SceneScale);

        // The "parent" transform of the root node is an identity matrix.
        // However, we optionally apply the UnitScaleFactor and an x-rotation to move from y-up to z-up.
        aiMatrix4x4t<float> AdjustmentXfm;
        if (!DisableAutoSpaceChange) {
            aiMatrix4x4t<float> tmpRot;
            aiMatrix4x4t<float> tmpScale;
            aiMatrix4x4t<float>::RotationX( PI/2., tmpRot);
            aiMatrix4x4t<float>::Scaling( aiVector3t<float>(SceneObject->SceneScale), tmpScale);
            AdjustmentXfm = tmpScale * tmpRot;
        }

	RootNode->Setup(Scene->mRootNode, SceneObject, AdjustmentXfm);

        // Changes root transformation to incorporate unitscalefactor and x-rotation.
        if (!DisableAutoSpaceChange) {
             Scene->mRootNode->mTransformation = AdjustmentXfm * Scene->mRootNode->mTransformation;
        }

	return SceneObject;
}


float UAIScene::GetUnitScaleFactor()
{
	return SceneScale;
}

TArray<UMeshComponent*> UAIScene::SpawnAllMeshes(FTransform Transform, TSubclassOf<AActor> ClassToSpawn)
{
	//TODO 
	TArray<UMeshComponent*> Meshes;
	if (scene)
	{
		for (const auto Mesh : OwnedMeshes)
		{
			//mesh data will be owned by the scene object

			AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(ClassToSpawn, Transform);
			UAssimpMesh* AssimpMesh = NewObject<UAssimpMesh>(SpawnedActor, UAssimpMesh::StaticClass());
			AssimpMesh->RegisterComponent();
			AssimpMesh->SetWorldLocation(Transform.GetLocation());
			AssimpMesh->SetWorldRotation(Transform.GetRotation());
			AssimpMesh->AttachToComponent(SpawnedActor->GetRootComponent(),
			                              FAttachmentTransformRules::KeepWorldTransform);
			AssimpMesh->SetupMesh(Mesh);
		}
	}
	else
	{
		UE_LOG(LogAssimp, Error, TEXT("Assimp scene is not valid "));
	}


	return Meshes;
}

const TArray<UAIMesh*>& UAIScene::GetAllMeshes() const
{
	return OwnedMeshes;
}


const TArray<UAIMaterial*>& UAIScene::GetAllMaterials() const
{
	return OwnedMaterials;
}

void UAIScene::BeginDestroy()
{
	aiReleaseImport(scene);
	scene = nullptr;

	Super::BeginDestroy();
}


UAINode* UAIScene::GetRootNode()
{
	return OwnedRootNode;
}

UAIMesh* UAIScene::GetMeshAtIndex(int Index)
{
	return OwnedMeshes[Index];
}

const TArray<UAICamera*>& UAIScene::GetAllCameras() const
{
	return OwnedCameras;
}

UTexture2D* UAIScene::GetEmbeddedTexture(FString FilePath, bool bIsNormalMap)
{
	const auto EmbedTexture = scene->GetEmbeddedTexture(TCHAR_TO_UTF8(*FilePath));

	UTexture2D* Result;


	if (!EmbedTexture)
	{
		UE_LOG(LogAssimp, Log, TEXT("Embedded texture not found . Texture might be  external "));
		return nullptr;
	}

	const int TextureWidth = EmbedTexture->mWidth;
	const float TextureHeight = EmbedTexture->mHeight;
	const EPixelFormat PixelFormat = PF_B8G8R8A8;

	UE_LOG(LogAssimp, Log, TEXT("Importing embedded texture X: %d  Y: %f"), TextureWidth, TextureHeight);
	if (EmbedTexture->mHeight != 0)
	{
		Result = UTexture2D::CreateTransient(EmbedTexture->mWidth, EmbedTexture->mHeight, PixelFormat);


		if (Result)
		{
#if ENGINE_MAJOR_VERSION>4

			FTexturePlatformData* PlatformData = Result->GetPlatformData();
#else

			FTexturePlatformData* PlatformData = Result->PlatformData;
#endif
			Result->bNotOfflineProcessed = true;
			uint8* MipData = static_cast<uint8*>(PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE));


			//an array for pixels
			uint8* Pixels = new uint8[TextureWidth * TextureHeight * 4];
			for (int32 y = 0; y < TextureHeight; y++)
			{
				for (int32 x = 0; x < TextureWidth; x++)
				{
					//Get the current pixel
					const int32 CurrentPixelIndex = ((y * TextureWidth) + x);
					//Get a random vector that will represent the RGB values for the current pixel
					const aiTexel CurrentPixel = EmbedTexture->pcData[CurrentPixelIndex];

					Pixels[4 * CurrentPixelIndex] = CurrentPixel.b; //b
					Pixels[4 * CurrentPixelIndex + 1] = CurrentPixel.g; //g
					Pixels[4 * CurrentPixelIndex + 2] = CurrentPixel.b; //r
					Pixels[4 * CurrentPixelIndex + 3] = CurrentPixel.a; //set A channel always to maximum
				}
			}
			FMemory::Memcpy(MipData, Pixels, PlatformData->Mips[0].BulkData.GetBulkDataSize());


		        PlatformData->Mips[0].BulkData.Unlock();

		        Result->UpdateResource();
		}
	}
	else
	{
		//Texture is compressed read it from memory
		const size_t size = EmbedTexture->mWidth;
		const auto BinaryData = reinterpret_cast<const unsigned char*>(EmbedTexture->pcData);
		const TArray<uint8> Buffer(BinaryData, size);


		Result = UKismetRenderingLibrary::ImportBufferAsTexture2D(GetWorld(), Buffer);
	}

	if (Result && bIsNormalMap)
	{
		Result->CompressionSettings = TC_Normalmap;
		Result->SRGB = false;
		Result->UpdateResource();
	}
	return Result;
}


EPixelFormat UAIScene::GetPixelFormat(const aiTexture* Texture)
{
	if (Texture->CheckFormat("rgba8888"))
	{
		return EPixelFormat::PF_R8G8B8A8;
	}
	else
	{
		UE_LOG(LogAssimp, Fatal, TEXT("Pixel format not implemented"));
	}
	return EPixelFormat::PF_Unknown;
}
