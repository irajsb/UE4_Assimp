// Fill out your copyright notice in the Description page of Project Settings.


#include "AILight.h"

#include "UE_Assimp.h"

UAILight* UAILight::InternalCreateNewObject(UObject* Parent, aiLight* InLight)
{

	//todo check if object is already created and skip creation and return object 
	UAILight* Object=	NewObject<UAILight>(Parent,UAILight::StaticClass(),NAME_None,RF_Transient);
	Object->Light= InLight;
	return Object;
}

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
	return ToVectorCM(Light->mPosition);
}

FVector UAILight::GetLightDirection()
{
	return  ToVector(Light->mDirection);
}

FVector UAILight::GetUpDirection()
{
	return  ToVector(Light->mUp);
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
	return FVector2D( Light->mSize.x*100.f,Light->mSize.y*100.f);
}
