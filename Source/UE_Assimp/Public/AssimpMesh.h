// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "ProceduralMeshComponent.h"
#include "AssimpMesh.generated.h"

/**
 *
 */
class UAIMesh;
UCLASS()
class UE_ASSIMP_API UAssimpMesh : public UProceduralMeshComponent
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Assimp")
	void SetupMesh(UAIMesh *InMeshData);

private:
	UPROPERTY()
	UAIMesh *MeshData;
};
