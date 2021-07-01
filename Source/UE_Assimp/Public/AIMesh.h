// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "assimp/mesh.h"
#include "UObject/NoExportTypes.h"
#include "ProceduralMeshComponent.h"
#include "AIMesh.generated.h"

/**
 * 
 */

//wrapper for assimp mesh 
UCLASS(BlueprintType)
class UE_ASSIMP_API UAIMesh : public UObject
{
	GENERATED_BODY()


	public:
UFUNCTION(BlueprintCallable)
void GetMeshVertices(TArray<FVector>& Vertices );	
UFUNCTION(BlueprintCallable)
void GetMeshNormals(TArray<FVector>& Normals );	
UFUNCTION(BlueprintCallable)
void GetMeshDataForProceduralMesh(TArray<FVector>&Vertices,TArray<int32>& Triangles,TArray<FVector>& Normals, TArray<FVector2D>& UV0, TArray<FProcMeshTangent>& Tangents);
UFUNCTION(BlueprintCallable,BlueprintPure)
int GetNumVertices();

//Create New Assimp mesh to hold mesh data 	have to be checked before calling to not make duplicates 
static UAIMesh* InternalConstructNewAIMesh(aiMesh* InMesh,UObject* Parent);
	private:
	aiMesh* Mesh;
};
