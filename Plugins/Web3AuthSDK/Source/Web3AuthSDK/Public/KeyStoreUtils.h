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
		FString sessionId;
};

UCLASS()
class WEB3AUTHSDK_API UKeyStoreUtils : public UObject
{
	GENERATED_BODY()
private:
	UPROPERTY()
    	UWeb3StorageAdapter* StorageInstance;
		// Int UserIndex
		// FString SlotName

	UWeb3StorageAdapter* GetInstance();
public:
	void Assign(FString value);
	FString Get();
	void Clear();
public:
    // UKeyStoreUtils(FString filename, Int user);
	UKeyStoreUtils();
	~UKeyStoreUtils();
};
