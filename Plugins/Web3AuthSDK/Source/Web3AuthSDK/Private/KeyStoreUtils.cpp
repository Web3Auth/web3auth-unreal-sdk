#include "KeyStoreUtils.h"

UKeyStoreUtils::UKeyStoreUtils() {
    UWeb3StorageAdapter::StaticClass();
}

UKeyStoreUtils::~UKeyStoreUtils() {}

void UKeyStoreUtils::Assign(FString sessionId, FString redirectUrl) {
    UWeb3StorageAdapter* saveGameInstance = Cast<UWeb3StorageAdapter>(UGameplayStatics::CreateSaveGameObject(UWeb3StorageAdapter::StaticClass()));

    if (saveGameInstance)
    {
        saveGameInstance->sessionId = sessionId;
        saveGameInstance->redirectUrl = redirectUrl;
        UGameplayStatics::SaveGameToSlot(saveGameInstance, TEXT("Web3AuthDataSlot"), 0);
    }
}

FString UKeyStoreUtils::GetSessionId() {
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
        saveGameInstance->redirectUrl = "";
        UGameplayStatics::SaveGameToSlot(saveGameInstance, TEXT("Web3AuthDataSlot"), 0);
    }
}

FString UKeyStoreUtils::GetRedirectUrl() {
    UWeb3StorageAdapter* saveGameInstance = Cast<UWeb3StorageAdapter>(UGameplayStatics::LoadGameFromSlot(TEXT("Web3AuthDataSlot"), 0));

    if (saveGameInstance)
    {
        return saveGameInstance->redirectUrl;
    }
    return "";
}
