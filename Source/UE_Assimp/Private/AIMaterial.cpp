// Fill out your copyright notice in the Description page of Project Settings.


#include "AIMaterial.h"

UAIMaterial* UAIMaterial::InternalCreateNewObject(UObject* Parent, aiMaterial* InMaterial)
{
	//todo check if object is already created and skip creation and return object 
	UAIMaterial* Object=	NewObject<UAIMaterial>(Parent,UAIMaterial::StaticClass(),NAME_None,RF_Transient);
	Object->Material= InMaterial;
	return Object;
}

void UAIMaterial::GetMaterialBaseColor(FLinearColor& BaseColor)
{
	aiColor3D color;
	if (AI_SUCCESS == Material->Get(AI_MATKEY_COLOR_DIFFUSE, color)) {
		BaseColor = FColor((color.r ), (color.g ), (color.b ), 1.0f);
				
	}
}
