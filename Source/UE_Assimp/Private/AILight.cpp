// Fill out your copyright notice in the Description page of Project Settings.


#include "AILight.h"

#include "UE_Assimp.h"



FString UAILight::GetLightName()
{
	return  UTF8_TO_TCHAR(	Light->mName.C_Str());
}

TEnumAsByte<EAssimpLightType> UAILight::GetLightType()
{
	return static_cast<TEnumAsByte<EAssimpLightType>>( static_cast<uint8>( Light->mType));
	
}

FVector UAILight::GetLightPosition()
{
	return aiVector3DToVector(Light->mPosition);
}

FVector UAILight::GetLightDirection()
{
	return  aiVector3DToVector(Light->mDirection);
}

FVector UAILight::GetUpDirection()
{
	return  aiVector3DToVector(Light->mUp);
}

float UAILight::GetAttenuationConstant()
{
	return Light->mAttenuationConstant;
}

float UAILight::GetAttenuationLinear()
{
	return  Light->mAttenuationLinear;
}

float UAILight::GetAttenuationQuadratic()
{
	return Light->mAttenuationQuadratic;
}

FLinearColor UAILight::GetColorDiffuse()
{
	return  FLinearColor(Light->mColorDiffuse.r,Light->mColorDiffuse.g,Light->mColorDiffuse.b,1);
}

FLinearColor UAILight::GetColorSpecular()
{
	return  FLinearColor(Light->mColorSpecular.r,Light->mColorSpecular.g,Light->mColorSpecular.b,1);
}

FLinearColor UAILight::GetColorAmbient()
{
	return  FLinearColor(Light->mColorAmbient.r,Light->mColorAmbient.g,Light->mColorAmbient.b,1); 
}

float UAILight::GetConeInnerAngle()
{
	return Light->mAngleInnerCone;
}

float UAILight::GetConeOuterAngle()
{
	return Light->mAngleOuterCone;
}

FVector2D UAILight::GetAreaLightSize()
{
	return FVector2D( Light->mSize.x,Light->mSize.y);
}
