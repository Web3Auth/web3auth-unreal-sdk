#include "KeyStoreUtils.h"

UKeyStoreUtils::UKeyStoreUtils() {}

/*
UKeyStoreUtils::UKeyStoreUtils(FString filename, Int user) {
	SlotName = filename;
	UserIndex = user;
}
*/

UKeyStoreUtils::~UKeyStoreUtils() {}

UWeb3StorageAdapter* UKeyStoreUtils::GetInstance() {
	if(StorageInstance == nullptr) {
		// Try load
		// StorageInstance = Cast<UWeb3StorageAdapter>(UGameplayStatics::LoadGameFromSlot(SlotName, UserIndex);

		// Instantiate new
		if(StorageInstance == nullptr) {
			StorageInstance = Cast<UWeb3StorageAdapter>(UGameplayStatics::CreateSaveGameObject(UWeb3StorageAdapter::StaticClass()));
		}
	}

	return StorageInstance;
}

void UKeyStoreUtils::Assign(FString value) {
	GetInstance()->sessionId = value;
	//UGameplayStatics::SaveGameToSlot(GetInstance(), SlotName, UserIndex);
}

FString UKeyStoreUtils::Get() {
	return GetInstance()->sessionId;
}

void UKeyStoreUtils::Clear() {
	GetInstance()->sessionId = FString();
	//UGameplayStatics::SaveGameToSlot(GetInstance(), SlotName, UserIndex);
}
