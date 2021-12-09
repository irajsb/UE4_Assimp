// Fill out your copyright notice in the Description page of Project Settings.


#include "AICamera.h"

#include "UE_Assimp.h"


FString UAICamera::GetCameraName()
{
	return  UTF8_TO_TCHAR(	camera->mName.C_Str());
}

float UAICamera::GetCameraFOV()
{
	return  camera->mHorizontalFOV;
	
}

float UAICamera::GetClipPlaneNear()
{
	return  camera->mClipPlaneNear;
}

float UAICamera::GetClipPlaneFar()
{return  camera->mClipPlaneFar;
}

float UAICamera::GetAspectRatio()
{
	return  camera->mAspect;
	
}

FVector UAICamera::GetPosition()
{
	return ToVectorCM( camera->mPosition)
}

FVector UAICamera::GetUpVector()
{
	return ToVector( camera->mUp)
}

FVector UAICamera::GetLookAtVector()
{
	return ToVector( camera->mLookAt)
}
