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
	StorageInstance->KeyValuePairs.Add(key, value);
	UGameplayStatics::SaveGameToSlot(StorageInstance, TEXT("Web3AuthDataSlot"), 0);
}

FString UKeyStoreUtils::Get(FString key) {
	FString* value = StorageInstance->KeyValuePairs.Find(key);
	if (value == nullptr) {
		return FString();
	}

	return *value;
}

void UKeyStoreUtils::Remove(FString key) {
	StorageInstance->KeyValuePairs.Remove(key);
	UGameplayStatics::SaveGameToSlot(StorageInstance, TEXT("Web3AuthDataSlot"), 0);
}
