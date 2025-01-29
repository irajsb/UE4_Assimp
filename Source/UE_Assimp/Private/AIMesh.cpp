// Fill out your copyright notice in the Description page of Project Settings.


#include "AIMesh.h"

#include "AIScene.h"
#include "KismetProceduralMeshLibrary.h"
#include "MeshDescriptionBuilder.h"
#include "StaticMeshDescription.h"
#include "UE_Assimp.h"
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION > 3
#include "UDynamicMesh.h"
#else
#include "GeometryFramework/Public/UDynamicMesh.h"
#endif
#include "GeometryScript/MeshBasicEditFunctions.h"
#include "GeometryScript/MeshMaterialFunctions.h"
#include "GeometryScript/MeshUVFunctions.h"

void UAIMesh::GetMeshVertices(TArray<FVector>& Vertices)
{
	if (!this)
	{
		UE_LOG(LogAssimp, Fatal, TEXT("No Mesh"));
		return;
	}
	if (!Mesh)
	{
		UE_LOG(LogAssimp, Fatal, TEXT("No Mesh Data"));
		return;
	}
	Vertices.Empty();
	Vertices.AddUninitialized(Mesh->mNumVertices);
	for (unsigned int Index = 0; Index < Mesh->mNumVertices; Index++)
	{
		Vertices[Index] = ToVector(Mesh->mVertices[Index]);
	}
}

void UAIMesh::GetMeshNormals(TArray<FVector>& Normals)
{
	if (!this)
	{
		UE_LOG(LogAssimp, Fatal, TEXT("No Mesh"));
		return;
	}
	if (!Mesh)
	{
		UE_LOG(LogAssimp, Fatal, TEXT("No Mesh Data"));
		return;
	}
	Normals.Empty();
	Normals.AddUninitialized(Mesh->mNumVertices);
	for (unsigned int Index = 0; Index < Mesh->mNumVertices; Index++)
	{
		Normals[Index] = ToVector(Mesh->mNormals[Index]);
	}
}

void UAIMesh::GetMeshDataForProceduralMesh(TArray<FVector>& Vertices, TArray<int32>& Triangles,
                                           TArray<FVector>& Normals, TArray<FVector2D>& UV0,
                                           TArray<FProcMeshTangent>& Tangents)
{
	if (!this)
	{
		UE_LOG(LogAssimp, Fatal, TEXT("No Mesh"));
		return;
	}
	if (!Mesh)
	{
		UE_LOG(LogAssimp, Fatal, TEXT("No Mesh Data"));
		return;
	}

	Tangents.Reset();
	Vertices.Reset();
	Triangles.Reset();
	Normals.Reset();
	UV0.Reset();

	Vertices.AddUninitialized(Mesh->mNumVertices);
	Normals.AddUninitialized(Mesh->mNumVertices);
	if (Mesh->HasTangentsAndBitangents())
	{
		Tangents.AddUninitialized(Mesh->mNumVertices);
	}
	else
	{
		//
		UE_LOG(LogAssimp, Warning, TEXT("Mesh is missing Tangents"));
	}
	UV0.AddUninitialized(Mesh->mNumVertices);


	for (unsigned int Index = 0; Index < Mesh->mNumVertices; Index++)
	{
		Normals[Index] = ToVector(Mesh->mNormals[Index]);
		Vertices[Index] = ToVector(Mesh->mVertices[Index]);

		if (Mesh->mTangents)
		{
			Tangents[Index].TangentX = ToVector(Mesh->mTangents[Index]);
		}

		if (Mesh->HasTextureCoords(0))
		{
			UV0[Index].X = Mesh->mTextureCoords[0][Index].x;
			UV0[Index].Y = Mesh->mTextureCoords[0][Index].y;
		}
	}


	for (unsigned int i = 0; i < Mesh->mNumFaces; i++)
	{
		const auto Face = Mesh->mFaces[i];
		for (unsigned index = 0; index < Face.mNumIndices; index++)
		{
			Triangles.Push(Face.mIndices[index]);
		}
	}
}

UStaticMesh* UAIMesh::GetStaticMesh()
{
	if (StaticMesh) {
		return StaticMesh;
	}
	MeshDescription = UStaticMesh::CreateStaticMeshDescription(this);

	FMeshDescriptionBuilder MeshDescBuilder;

	MeshDescBuilder.SetMeshDescription(&MeshDescription->GetMeshDescription());
	MeshDescBuilder.EnablePolyGroups();
	MeshDescBuilder.SetNumUVLayers(1);

	TArray<FVertexInstanceID> VertexInstances;
	VertexInstances.AddUninitialized(Mesh->mNumVertices);
	for (unsigned int Index = 0; Index < Mesh->mNumVertices; Index++)
	{
		auto VertexID = MeshDescBuilder.AppendVertex(ToVector(Mesh->mVertices[Index]));

		auto Instance = MeshDescBuilder.AppendInstance(VertexID);
		VertexInstances[Index] = Instance;
		
		if(Mesh->HasNormals())
		{
		MeshDescBuilder.SetInstanceNormal(Instance,ToVector(Mesh->mNormals[Index]));
		}else
		{
			UE_LOG(LogAssimp,Warning,TEXT("Normals not found consider generating them with assimp"))
		}
		
		if (Mesh->HasTextureCoords(0))
		{
			MeshDescBuilder.SetInstanceUV(
				Instance, FVector2D(Mesh->mTextureCoords[0][Index].x, Mesh->mTextureCoords[0][Index].y), 0);
		}
	}

	const FPolygonGroupID PolygonGroup = MeshDescBuilder.AppendPolygonGroup();


	for (unsigned int i = 0; i < Mesh->mNumFaces; i++)
	{
		const auto Face = Mesh->mFaces[i];
		if(Face.mNumIndices>2)
		{
			MeshDescBuilder.AppendTriangle(VertexInstances[Face.mIndices[0]], VertexInstances[Face.mIndices[1]],
										   VertexInstances[Face.mIndices[2]], PolygonGroup);
		}
	}
	// At least one material must be added
	 StaticMesh = NewObject<UStaticMesh>(this);
	StaticMesh->GetStaticMaterials().Add(FStaticMaterial());

	UStaticMesh::FBuildMeshDescriptionsParams MeshDescriptionsParams;
	MeshDescriptionsParams.bBuildSimpleCollision = true;


	// Build static mesh
	TArray<const FMeshDescription*> MeshDescriptions;
	MeshDescriptions.Emplace(&MeshDescription->GetMeshDescription());
	StaticMesh->BuildFromMeshDescriptions(MeshDescriptions, MeshDescriptionsParams);


	return StaticMesh;
}

UDynamicMesh* UAIMesh::GetDynamicMesh()
{
	if (DynamicMesh)
	{
		return DynamicMesh;
	}

	DynamicMesh = NewObject<UDynamicMesh>();
	// Add vertices to mesh
	for (unsigned int Index = 0; Index < Mesh->mNumVertices; Index++)
	{
		const FVector Vertex = FVector(Mesh->mVertices[Index].x, Mesh->mVertices[Index].y, Mesh->mVertices[Index].z);
		int NewVertexIndex;
		UGeometryScriptLibrary_MeshBasicEditFunctions::AddVertexToMesh(DynamicMesh, Vertex, NewVertexIndex);
	}
	// Add triangles to mesh
	for (unsigned int i = 0; i < Mesh->mNumFaces; i++)
	{
		const auto Face = Mesh->mFaces[i];
		if (Face.mNumIndices > 2)
		{
			const FIntVector NewTriangle = FIntVector(Face.mIndices[0], Face.mIndices[1], Face.mIndices[2]);
			int NewTriangleIndex;
			UGeometryScriptLibrary_MeshBasicEditFunctions::AddTriangleToMesh(
				DynamicMesh, NewTriangle, NewTriangleIndex);
			// Set UVs
			if (Mesh->HasTextureCoords(0))
			{
				FGeometryScriptUVTriangle UVTriangle;
				UVTriangle.UV0 = FVector2D(Mesh->mTextureCoords[0][NewTriangle.X].x,
				                           Mesh->mTextureCoords[0][NewTriangle.X].y);
				UVTriangle.UV1 = FVector2D(Mesh->mTextureCoords[0][NewTriangle.Y].x,
				                           Mesh->mTextureCoords[0][NewTriangle.Y].y);
				UVTriangle.UV2 = FVector2D(Mesh->mTextureCoords[0][NewTriangle.Z].x,
				                           Mesh->mTextureCoords[0][NewTriangle.Z].y);
				bool bIsValidTriangle;
				UGeometryScriptLibrary_MeshUVFunctions::SetMeshTriangleUVs(DynamicMesh, 0, NewTriangleIndex,
				                                                           UVTriangle, bIsValidTriangle);
			}
		}
	}
	// Enable Material IDs
	UGeometryScriptLibrary_MeshMaterialFunctions::EnableMaterialIDs(DynamicMesh);
	UGeometryScriptLibrary_MeshMaterialFunctions::RemapMaterialIDs(DynamicMesh, 0, GetMaterialIndex());

	// @TODO: Normals and Collision

	return DynamicMesh;
}


int UAIMesh::GetNumVertices()
{
	return Mesh->mNumVertices;
}

void UAIMesh::GetAllBones(TArray<FAIBone>& Bones)
{
	for (unsigned int i = 0; i < Mesh->mNumBones; i++)
	{
		Bones.Add(FAIBone(Mesh->mBones[i]));
	}
}

FString UAIMesh::GetMeshName() const
{
	return UTF8_TO_TCHAR(Mesh->mName.C_Str());
}

int UAIMesh::GetMaterialIndex()
{
	return Mesh->mMaterialIndex;
}

