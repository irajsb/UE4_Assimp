// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIScene.h"


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
UCLASS(BlueprintType)
class UE_ASSIMP_API UAINode : public UObject
{
	GENERATED_BODY()
public:

	void Setup(aiNode* InNode, UAIScene* Scene, const aiMatrix4x4& ParentTransform);
	/** The name of the node.
	*
	* The name might be empty (length of zero) but all nodes which
	* need to be referenced by either bones or animations are named.
	* Multiple nodes may have the same name, except for nodes which are referenced
	* by bones (see #aiBone and #aiMesh::mBones). Their names *must* be unique.
	*
	* Cameras and lights reference a specific node by name - if there
	* are multiple nodes with this name, they are assigned to each of them.
	* <br>
	* There are no limitations with regard to the characters contained in
	* the name string as it is usually taken directly from the source file.
	*
	* Implementations should be able to handle tokens such as whitespace, tabs,
	* line feeds, quotation marks, ampersands etc.
	*
	* Sometimes assimp introduces new nodes not present in the source file
	* into the hierarchy (usually out of necessity because sometimes the
	* source hierarchy format is simply not compatible). Their names are
	* surrounded by @verbatim <> @endverbatim e.g.
	*  @verbatim<DummyRootNode> @endverbatim.
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Assimp|Node")
	FString GetNodeName() const;
	/** Parent node. nullptr if this node is the root node. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Assimp|Node")
	UAINode* GetParentNode(bool& Success) const;
	/** The child nodes of this node. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Assimp|Node")
	const TArray<UAINode*>& GetChildNodes() const;
	/** The transformation relative to the node's parent. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Assimp|Node")
	void GetNodeTransform(FTransform& Transform) const;

	/** The transformation relative to Root */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Assimp|Node")
	FTransform GetRootTransform();
	//false if empty
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Assimp|Node")
	bool GetAllMeshes(TArray<UAIMesh*>& Meshes);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Assimp|Node")
	UAIScene* GetScene();

private:
	void RegisterNewNode(aiNode* InNode, UAIScene* Scene, const aiMatrix4x4& ParentTransform);

	TArray<UAINode*> OwnedNodes;
	FTransform WorldTransform;
	aiNode* Node;
};
