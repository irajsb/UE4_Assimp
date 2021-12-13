// Fill out your copyright notice in the Description page of Project Settings.


#include "AIMaterial.h"


FString TextureTypeToString(EAiTextureType in)
{ switch (in) {
case aiTextureType_NONE:
    return "n/a";
case aiTextureType_DIFFUSE:
    return "Diffuse";
case aiTextureType_SPECULAR:
    return "Specular";
case aiTextureType_AMBIENT:
    return "Ambient";
case aiTextureType_EMISSIVE:
    return "Emissive";
case aiTextureType_OPACITY:
    return "Opacity";
case aiTextureType_NORMALS:
    return "Normals";
case aiTextureType_HEIGHT:
    return "Height";
case aiTextureType_SHININESS:
    return "Shininess";
case aiTextureType_DISPLACEMENT:
    return "Displacement";
case aiTextureType_LIGHTMAP:
    return "Lightmap";
case aiTextureType_REFLECTION:
    return "Reflection";
case aiTextureType_BASE_COLOR:
    return "BaseColor";
case aiTextureType_NORMAL_CAMERA:
    return "NormalCamera";
case aiTextureType_EMISSION_COLOR:
    return "EmissionColor";
case aiTextureType_METALNESS:
    return "Metalness";
case aiTextureType_DIFFUSE_ROUGHNESS:
    return "DiffuseRoughness";
case aiTextureType_AMBIENT_OCCLUSION:
    return "AmbientOcclusion";
case aiTextureType_SHEEN:
    return "Sheen";
case aiTextureType_CLEARCOAT:
    return "Clearcoat";
case aiTextureType_TRANSMISSION:
    return "Transmission";
case aiTextureType_UNKNOWN:
    return "Unknown";
default:
    break;
}
	check(false);
	return "BUG";
}

UAIMaterial* UAIMaterial::InternalCreateNewObject(UObject* Parent, aiMaterial* InMaterial)
{
	//todo check if object is already created and skip creation and return object 
	UAIMaterial* Object=	NewObject<UAIMaterial>(Parent,UAIMaterial::StaticClass(),NAME_None,RF_Transient);
	Object->Material= InMaterial;
	return Object;
}

void UAIMaterial::GetMaterialBaseColor(FLinearColor& BaseColor) const
{
	aiColor3D color;
	if (AI_SUCCESS == Material->Get(AI_MATKEY_COLOR_DIFFUSE, color)) {
		BaseColor = FColor((color.r ), (color.g ), (color.b ), 1.0f);
				
	}
}

FString UAIMaterial::GetMaterialName() const
{
	
	return  UTF8_TO_TCHAR(Material->GetName().C_Str());
}

EAssimpReturn UAIMaterial::GetMaterialTexture( EAiTextureType Type, uint8 Index, FString &Path,
    EAiTextureMapping Mapping)
{

    aiString TempPath;
    aiTextureMapping* TempMapping=nullptr;
    const aiTextureType TempType=   static_cast<aiTextureType>( Type);
   
   
    auto AIResult= Material->GetTexture(TempType,Index,&TempPath);
    Path=UTF8_TO_TCHAR(TempPath.C_Str());


    const EAssimpReturn Result=static_cast<EAssimpReturn>(AIResult);
    
    return  Result;
}

