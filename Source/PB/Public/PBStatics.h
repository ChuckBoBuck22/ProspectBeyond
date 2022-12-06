// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PBStatics.generated.h"


/**
 * 
 */
UCLASS()
class PB_API UPBStatics : public UObject
{
	GENERATED_BODY()
	
public:
	static bool GetPixelFromT2D(UTexture2D* T2D, int32 X, int32 Y, FLinearColor& PixelColor);

};
