// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "AIBone.generated.h"
#include "UObject/NoExportTypes.h"


/**
 *
 */

struct aiBone;
struct aiVertexWeight;

USTRUCT(BlueprintType)
struct FAIBone
{
    friend class UAssimpFunctionLibrary;

    GENERATED_BODY()

    FAIBone();

    FAIBone(aiBone *Bone);

  private:
    aiBone *Bone = nullptr;
};

USTRUCT(BlueprintType)
struct FAIVertexWeight
{
    friend class UAssimpFunctionLibrary;

  public:
    GENERATED_BODY()

    FAIVertexWeight();

    FAIVertexWeight(aiVertexWeight InVertexWeight);

    UPROPERTY(BlueprintReadOnly, Category = "Assimp|Bone")
    int VertexID;
    UPROPERTY(BlueprintReadOnly, Category = "Assimp|Bone")
    float Weight;

  private:
};
