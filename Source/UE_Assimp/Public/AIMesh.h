// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "AIBone.h"
#include "AIScene.h"
#include "assimp/mesh.h"
#include "MeshDescription.h"
#include "UObject/NoExportTypes.h"
#include "ProceduralMeshComponent.h"
#include "AIMesh.generated.h"

class UDynamicMesh;

/**
 * 
 */

//wrapper for assimp mesh 
UCLASS(BlueprintType)
class UE_ASSIMP_API UAIMesh : public UObject
{
	GENERATED_BODY()
friend UAIScene;

	public:
	/** The number of vertices in this mesh.
  * This is also the size of all of the per-vertex data arrays.
  * The maximum value for this member is #AI_MAX_VERTICES.
  */
	UFUNCTION(BlueprintCallable)
	void GetMeshVertices(TArray<FVector>& Vertices );
	/** Vertex normals.
	* The array contains normalized vectors, nullptr if not present.*/
	UFUNCTION(BlueprintCallable)
	void GetMeshNormals(TArray<FVector>& Normals );
	//Get All data needed to create a mesh section in Unreal engine
	UFUNCTION(BlueprintCallable)
	void GetMeshDataForProceduralMesh(TArray<FVector>&Vertices,TArray<int32>& Triangles,TArray<FVector>& Normals, TArray<FVector2D>& UV0, TArray<FProcMeshTangent>& Tangents);
	UFUNCTION(BlueprintCallable)
	UStaticMesh* GetStaticMesh();
	/**
	 * Get Dynamic Mesh from this mesh
	 * @return 
	 */
	UFUNCTION(BlueprintCallable, Category="Assimp|AIMesh")
	UDynamicMesh* GetDynamicMesh();
	
	//Num of vertices array
	UFUNCTION(BlueprintCallable,BlueprintPure)
	int GetNumVertices();
	/** The bones of this mesh.
  * A bone consists of a name by which it can be found in the
  * frame hierarchy and a set of vertex weights.
  */
	UFUNCTION(BlueprintCallable,BlueprintPure)
	void  GetAllBones(TArray<FAIBone>& Bones);

	/** Name of the mesh. Meshes can be named, but this is not a
   *  requirement and leaving this field empty is totally fine.
   *  There are mainly three uses for mesh names:
   *   - some formats name nodes and meshes independently.
   *   - importers tend to split meshes up to meet the
   *      one-material-per-mesh requirement. Assigning
   *      the same (dummy) name to each of the result meshes
   *      aids the caller at recovering the original mesh
   *      partitioning.
   *   - Vertex animations refer to meshes by their names.
   **/
	UFUNCTION(BlueprintCallable,BlueprintPure)
	FString GetMeshName() const;

	/*Get material for this mesh . use get materials in scene object with this index to get its material.
	 *Materials are property of scene because many meshes can share a single material .
	 **/
	
	UFUNCTION(BlueprintCallable,BlueprintPure)
	int GetMaterialIndex();

	
	UPROPERTY()
	UStaticMeshDescription* MeshDescription;
	UPROPERTY()
	UStaticMesh* StaticMesh = nullptr;
	aiMesh* Mesh;

	UPROPERTY()
	UDynamicMesh* DynamicMesh = nullptr;
};
