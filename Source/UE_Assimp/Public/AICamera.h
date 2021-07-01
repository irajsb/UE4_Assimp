// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "assimp/camera.h"
#include "UObject/NoExportTypes.h"
#include "AICamera.generated.h"

/**
 * 
 */
UCLASS()
class UE_ASSIMP_API UAICamera : public UObject
{
	GENERATED_BODY()

public:
	static UAICamera* InternalCreateNewObject(UObject* Parent,aiCamera* InCamera);

	/** The name of the camera.
	*
	*  There must be a node in the scenegraph with the same name.
	*  This node specifies the position of the camera in the scene
	*  hierarchy and can be animated.
	*/
	UFUNCTION(BlueprintCallable,BlueprintPure,Category="Assimp|Camera")
	FString GetCameraName();
	
	/** Half horizontal field of view angle, in radians.
	*
	*  The field of view angle is the angle between the center
	*  line of the screen and the left or right border.
	*  The default value is 1/4PI.
	*/
	UFUNCTION(BlueprintCallable,BlueprintPure,Category="Assimp|Camera")
	float GetCameraFOV();
	/** Distance of the far clipping plane from the camera.
	*
	* The far clipping plane must, of course, be further away than the
	* near clipping plane. The default value is 1000.f. The ratio
	* between the near and the far plane should not be too
	* large (between 1000-10000 should be ok) to avoid floating-point
	* inaccuracies which could lead to z-fighting.
	*/
	UFUNCTION(BlueprintCallable,BlueprintPure,Category="Assimp|Camera")
	float GetClipPlaneNear();
	/** Distance of the far clipping plane from the camera.
	*
	* The far clipping plane must, of course, be further away than the
	* near clipping plane. The default value is 1000.f. The ratio
	* between the near and the far plane should not be too
	* large (between 1000-10000 should be ok) to avoid floating-point
	* inaccuracies which could lead to z-fighting.
	* 
	 */
	UFUNCTION(BlueprintCallable,BlueprintPure,Category="Assimp|Camera")
	float GetClipPlaneFar();

	/** Screen aspect ratio.
	*
	* This is the ration between the width and the height of the
	* screen. Typical values are 4/3, 1/2 or 1/1. This value is
	* 0 if the aspect ratio is not defined in the source file.
	* 0 is also the default value.
	*/
	UFUNCTION(BlueprintCallable,BlueprintPure,Category="Assimp|Camera")
	float GetAspectRatio();

	/** Position of the camera relative to the coordinate space
	*  defined by the corresponding node.
	*
	*  The default value is 0|0|0.
	*/
	UFUNCTION(BlueprintCallable,BlueprintPure,Category="Assimp|Camera")
	FVector GetPosition();
	/** 'Up' - vector of the camera coordinate system relative to
	*  the coordinate space defined by the corresponding node.
	*
	*  The 'right' vector of the camera coordinate system is
	*  the cross product of  the up and lookAt vectors.
	*  The default value is 0|1|0. The vector
	*  may be normalized, but it needn't.
	*/
	UFUNCTION(BlueprintCallable,BlueprintPure,Category="Assimp|Camera")
	FVector GetUpVector();

	/** 'LookAt' - vector of the camera coordinate system relative to
	*  the coordinate space defined by the corresponding node.
	*
	*  This is the viewing direction of the user.
	*  The default value is 0|0|1. The vector
	*  may be normalized, but it needn't.
	*/
	UFUNCTION(BlueprintCallable,BlueprintPure,Category="Assimp|Camera")
	FVector GetLookAtVector();
	private:
	aiCamera* camera;
};
