// Fill out your copyright notice in the Description page of Project Settings.


#include "PBStatics.h"



bool UPBStatics::GetPixelFromT2D(UTexture2D* T2D, int32 X, int32 Y, FLinearColor& PixelColor)
{
	if (!T2D)
	{
		return false;
	}

	if (X <= -1 || Y <= -1)
	{
		return false;
	}


	//~~~
	if (T2D->CompressionSettings != TC_VectorDisplacementmap)
	{
#if WITH_EDITOR
		FMessageLog("PIE").Error(FText::FromString("UVictoryBPFunctionLibrary::Victory_GetPixelFromT2D >> Texture Compression must be VectorDisplacementmap <3 Rama: {0}'" + T2D->GetName()));
#endif // WITH_EDITOR
		return false;
	}


	//~~~

	T2D->SRGB = false;

	T2D->CompressionSettings = TC_VectorDisplacementmap;

	//Update settings
	T2D->UpdateResource();

	FTexture2DMipMap& MipsMap = T2D->PlatformData->Mips[0];
	int32 TextureWidth = MipsMap.SizeX;
	int32 TextureHeight = MipsMap.SizeY;

	//Safety check!
	if (X >= TextureWidth || Y >= TextureHeight)
	{
#if WITH_EDITOR
		FMessageLog("PIE").Error(FText::FromString(FString("UVictoryBPFunctionLibrary::Victory_GetPixelFromT2D >> X or Y is outside of texture bounds! <3 Rama: {0}'" + (FString::FromInt(TextureWidth) + " x " + FString::FromInt(TextureHeight)))));
#endif // WITH_EDITOR
		return false;
	}

	FByteBulkData* RawImageData = &MipsMap.BulkData;

	if (!RawImageData)
	{
		return false;
	}

	int32 TotalCount = RawImageData->GetElementCount();
	if (TotalCount < 1)
	{
		return false;
	}

	uint8* RawByteArray = (uint8*)RawImageData->Lock(LOCK_READ_ONLY);

	//TC_VectorDisplacementmap	UMETA(DisplayName="VectorDisplacementmap (RGBA8)"),
	//! 4 because includes alpha <3 Rama
	/*
	for(int32 v = 0; v < TextureWidth * TextureHeight * RawImageData->GetElementSize() * 4; v++)
	{
		DebugString += FString::FromInt(RawByteArray[v]) + " ";
	}
	*/

	//Texture.cpp
	/*
	else if (FormatSettings.CompressionSettings == TC_VectorDisplacementmap)
	{
		TextureFormatName = NameBGRA8;
	}
	*/

	//Get!, converting FColor to FLinearColor 
	FColor ByteColor;
	ByteColor.B = RawByteArray[Y * TextureWidth * 4 + (X * 4)];
	ByteColor.G = RawByteArray[Y * TextureWidth * 4 + (X * 4) + 1];
	ByteColor.R = RawByteArray[Y * TextureWidth * 4 + (X * 4) + 2];
	ByteColor.A = RawByteArray[Y * TextureWidth * 4 + (X * 4) + 3];

	//Set!
	PixelColor = ByteColor.ReinterpretAsLinear();

	RawImageData->Unlock();

	return true;
}

