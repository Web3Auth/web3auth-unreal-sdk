#include "KeyStoreUtils.h"

UKeyStoreUtils::UKeyStoreUtils() {
	StorageInstance = Cast<UWeb3StorageAdapter>(UGameplayStatics::LoadGameFromSlot(TEXT("Web3AuthDataSlot"), 0));
	if(StorageInstance == nullptr) {
		StorageInstance = Cast<UWeb3StorageAdapter>(UGameplayStatics::CreateSaveGameObject(UWeb3StorageAdapter::StaticClass()));
	}
}

UKeyStoreUtils::~UKeyStoreUtils() {
}

void UKeyStoreUtils::Add(FString key, FString value) {
	if (StorageInstance->KeyValuePairs.Contains(key)) {
		StorageInstance->KeyValuePairs.Remove(key);
	}
	StorageInstance->KeyValuePairs.Add(key, value);
	UGameplayStatics::SaveGameToSlot(StorageInstance, TEXT("Web3AuthDataSlot"), 0);
}

FString UKeyStoreUtils::Get(FString key) {
	if (StorageInstance->KeyValuePairs.Contains(key)) {
		return StorageInstance->KeyValuePairs[key];
	}
	return "";
}

void UKeyStoreUtils::Remove(FString key) {
	if (StorageInstance->KeyValuePairs.Contains(key)) {
		StorageInstance->KeyValuePairs.Remove(key);
		UGameplayStatics::SaveGameToSlot(StorageInstance, TEXT("Web3AuthDataSlot"), 0);
	}
}
