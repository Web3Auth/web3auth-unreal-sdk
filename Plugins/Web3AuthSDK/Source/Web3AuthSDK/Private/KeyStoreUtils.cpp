#include "KeyStoreUtils.h"

// Note: Slot name and user index (2nd and 3rd parameters) should be configurable,
// since a game could have multiple different profiles and users on the same machine.

// Note: Class should have a lock on accessing the KeyValuePair collection. Probably is not thread-safe.

// Note: This class should also most likely be a singleton that persists for the lifetime of the application

UKeyStoreUtils::UKeyStoreUtils() {
	StorageInstance = Cast<UWeb3StorageAdapter>(UGameplayStatics::LoadGameFromSlot(TEXT("Web3AuthDataSlot"), 0));
	if(StorageInstance == nullptr) {
		StorageInstance = Cast<UWeb3StorageAdapter>(UGameplayStatics::CreateSaveGameObject(UWeb3StorageAdapter::StaticClass()));
	}
}

UKeyStoreUtils::~UKeyStoreUtils() {
}

void UKeyStoreUtils::Add(FString key, FString value) {
	if (StorageInstance == nullptr)
	{
		// log error here
		return;
	}
	StorageInstance->KeyValuePairs.Add(key, value);
	UGameplayStatics::SaveGameToSlot(StorageInstance, TEXT("Web3AuthDataSlot"), 0);
}

FString UKeyStoreUtils::Get(FString key) {
	if (StorageInstance == nullptr)
	{
		// log error here
		return FString();
	}
	FString* value = StorageInstance->KeyValuePairs.Find(key);
	if (value == nullptr) {
		return FString();
	}

	return *value;
}

void UKeyStoreUtils::Remove(FString key) {
	if (StorageInstance == nullptr)
	{
		// log error here
		return;
	}
	StorageInstance->KeyValuePairs.Remove(key);
	UGameplayStatics::SaveGameToSlot(StorageInstance, TEXT("Web3AuthDataSlot"), 0);
}
