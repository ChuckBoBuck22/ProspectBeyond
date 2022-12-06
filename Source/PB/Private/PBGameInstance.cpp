// Fill out your copyright notice in the Description page of Project Settings.


#include "PBGameInstance.h"
#include "Engine/ObjectLibrary.h"
#include "UObject/LazyObjectPtr.h"


TArray<FString> UPBGameInstance::GetAllMapNames()
{
	UObjectLibrary* ObjectLibrary = UObjectLibrary::CreateLibrary(UWorld::StaticClass(), false, true);
	ObjectLibrary->LoadAssetDataFromPath(TEXT("/Game/Maps"));
	TArray<FAssetData> AssetDatas;
	ObjectLibrary->GetAssetDataList(AssetDatas);
	
	UE_LOG(LogTemp, Warning, TEXT("Found maps: %d"), AssetDatas.Num());

	TArray<FString> Names = TArray<FString>();

	for (int32 i = 0; i < AssetDatas.Num(); ++i)
	{
		FAssetData& AssetData = AssetDatas[i];

		auto name = AssetData.AssetName.ToString();
		//FGuid GUID = AssetData.GetPackage()->GetGuid();
		
		Names.Add(name);
	}
	return Names;
}
