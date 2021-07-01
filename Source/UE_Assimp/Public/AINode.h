// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


#include "UE_Assimp.h"
#include "assimp/scene.h"
#include "UObject/NoExportTypes.h"
#include "AINode.generated.h"

/**
 * 
 */

//todo add metadata support
class UAIMesh;
class UAIScene;
UCLASS()
class UE_ASSIMP_API UAINode : public UObject
{
	GENERATED_BODY()
public:
static UAINode* InternalCreateNewObject(UObject* Parent,aiNode* InNode);

	public:
	UFUNCTION(BlueprintCallable,BlueprintPure,Category="Assimp|Node")
	void GetChildNodes(bool &Empty,TArray<UAINode*>& Nodes);
	UFUNCTION(BlueprintCallable,BlueprintPure,Category="Assimp|Node")
	FString GetNodeName();
	
	UFUNCTION(BlueprintCallable,BlueprintPure,Category="Assimp|Node")
	UAINode* GetParentNode(bool& Success);

	UFUNCTION(BlueprintCallable,BlueprintPure,Category="Assimp|Node")
	void  GetNodeTransform(FTransform& Transform);
	//false if empty
	UFUNCTION(BlueprintCallable,BlueprintPure,Category="Assimp|Node")
	bool GetAllMeshes(TArray<UAIMesh*>& Meshes);
	UFUNCTION(BlueprintCallable,BlueprintPure,Category="Assimp|Node")
	UAIScene* GetScene();
	private:
	aiNode* Node;
};
