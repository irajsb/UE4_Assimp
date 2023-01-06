// Fill out your copyright notice in the Description page of Project Settings.


#include "AIMaterial.h"


UAIMaterial* UAIMaterial::InternalCreateNewObject(UObject* Parent, aiMaterial* InMaterial)
{
	//todo check if object is already created and skip creation and return object 
	UAIMaterial* Object = NewObject<UAIMaterial>(Parent, UAIMaterial::StaticClass(), NAME_None, RF_Transient);
	Object->Material = InMaterial;
	return Object;
}

void UAIMaterial::GetMaterialBaseColor(FLinearColor& BaseColor) const
{
	aiColor3D color;
	if (AI_SUCCESS == Material->Get(AI_MATKEY_COLOR_DIFFUSE, color))
	{
		BaseColor = FLinearColor(color.r, color.g, color.b, 1.0f);
	}
}

void UAIMaterial::GetMaterialOpacity(float& Opacity) const
{
	if (AI_SUCCESS != Material->Get(AI_MATKEY_OPACITY, Opacity))
	{
		Opacity=1.f;
	}
}

FString UAIMaterial::GetMaterialName() const
{
	return UTF8_TO_TCHAR(Material->GetName().C_Str());
}

EAssimpReturn UAIMaterial::GetMaterialTexture(EAiTextureType Type,FVector2D &UVScale, uint8 Index, FString& Path,
                                              EAiTextureMapping Mapping)
{
	aiString TempPath;
	aiTextureMapping* TempMapping = nullptr;
	const aiTextureType TempType = static_cast<aiTextureType>(Type);


	auto AIResult = Material->GetTexture(TempType, Index, &TempPath);
	Path = UTF8_TO_TCHAR(TempPath.C_Str());

	//Get UV
	aiUVTransform UVTransform;
	Material->Get(AI_MATKEY_UVTRANSFORM(TempType,0),UVTransform);
	UVScale.X=UVTransform.mScaling.x;
	UVScale.Y=UVTransform.mScaling.y;
	
	const EAssimpReturn Result = static_cast<EAssimpReturn>(AIResult);

	return Result;
}
