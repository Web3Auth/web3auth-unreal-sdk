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
    UPROPERTY(VisibleAnywhere, Category = Basic)
        FString sessionId;
};

UCLASS()
class WEB3AUTHSDK_API UKeyStoreUtils : public UObject
{
    GENERATED_BODY()
private:
    UWeb3StorageAdapter* StorageInstance;
public:
    UKeyStoreUtils();
    ~UKeyStoreUtils();

    void Assign(FString value);
    FString Get();
    void Clear();
};

