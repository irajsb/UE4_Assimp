// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "AIScene.h"
#include "assimp/material.h"
#include "UObject/NoExportTypes.h"
#include "AIMaterial.generated.h"

/**
 * 
 */
UCLASS()
class UE_ASSIMP_API UAIMaterial : public UObject
{
	GENERATED_BODY()

	friend  UAIScene;
	public:
	static UAIMaterial* InternalCreateNewObject(UObject* Parent,aiMaterial* InMaterial);

	UFUNCTION(BlueprintCallable,BlueprintPure,Category="Assimp|Material")
	void GetMaterialBaseColor(FLinearColor& BaseColor);
	private:
	aiMaterial* Material;
};
