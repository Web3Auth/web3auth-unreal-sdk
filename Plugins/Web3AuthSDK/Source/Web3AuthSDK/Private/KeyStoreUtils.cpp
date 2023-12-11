#include "KeyStoreUtils.h"

UKeyStoreUtils::UKeyStoreUtils() {
    UWeb3StorageAdapter::StaticClass();
}

UKeyStoreUtils::~UKeyStoreUtils() {}

void UKeyStoreUtils::Assign(FString value) {
    UWeb3StorageAdapter* saveGameInstance = Cast<UWeb3StorageAdapter>(UGameplayStatics::CreateSaveGameObject(UWeb3StorageAdapter::StaticClass()));

    if (saveGameInstance)
    {
        saveGameInstance->sessionId = value;
        UGameplayStatics::SaveGameToSlot(saveGameInstance, TEXT("Web3AuthDataSlot"), 0);
    }
}

FString UKeyStoreUtils::Get() {
    UWeb3StorageAdapter* saveGameInstance = Cast<UWeb3StorageAdapter>(UGameplayStatics::LoadGameFromSlot(TEXT("Web3AuthDataSlot"), 0));

    if (saveGameInstance)
    {
        return saveGameInstance->sessionId;
    }
    return "";
}

void UKeyStoreUtils::Clear() {
    UWeb3StorageAdapter* saveGameInstance = Cast<UWeb3StorageAdapter>(UGameplayStatics::CreateSaveGameObject(UWeb3StorageAdapter::StaticClass()));

    if (saveGameInstance)
    {
        saveGameInstance->sessionId = "";
        UGameplayStatics::SaveGameToSlot(saveGameInstance, TEXT("Web3AuthDataSlot"), 0);
    }
}