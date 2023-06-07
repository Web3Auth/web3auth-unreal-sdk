// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Kismet/GameplayStatics.h"

#include "KeyStoreUtils.generated.h"

UCLASS()
class UWeb3StorageAdapter : public USaveGame
{
	GENERATED_BODY()
public:
	UPROPERTY()
		TMap<FString, FString> KeyValuePairs;
};

UCLASS()
class WEB3AUTHSDK_API UKeyStoreUtils : public UObject
{
	GENERATED_BODY()
private:
	UWeb3StorageAdapter* StorageInstance;
public:
	void Add(FString key, FString value);
	FString Get(FString key);
	void Remove(FString key);
public:
	UKeyStoreUtils();
	~UKeyStoreUtils();
};
