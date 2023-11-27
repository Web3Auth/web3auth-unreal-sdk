#pragma once

#if PLATFORM_WINDOWS
	#pragma warning(disable:4996)
	#pragma comment(lib, "crypt32.lib")
	#pragma comment(lib, "WS2_32.lib")
#endif

#include "CoreMinimal.h"

#define UI UI_ST
THIRD_PARTY_INCLUDES_START
#include <openssl/ec.h>
#include <openssl/ecdh.h>
#include <openssl/evp.h>
#include <openssl/crypto.h>
#include <openssl/sha.h>
#include <OpenSSL/obj_mac.h>
#include <OpenSSL/aes.h>
#include <OpenSSL/err.h>
#include <OpenSSL/asn1.h>
THIRD_PARTY_INCLUDES_END
#undef UI

#include "Misc/SecureHash.h"
#include <string>
#include "Keccak256.h"
#include "ECCrypto.generated.h"

UCLASS()
class WEB3AUTHSDK_API UECCrypto : public UObject
{
	GENERATED_BODY()
public:
	UECCrypto();
	~UECCrypto();

	FString decrypt(FString data, FString privateKeyHex, FString ephemPublicKeyHex, FString encryptionIvHex);
	FString encrypt(FString data, FString privateKeyHex, FString ephemPublicKeyHex, FString encryptionIvHex);

	FString generatePublicKey(const FString& privateKeyHex);
	FString generateECDSASignature(const FString& privateKeyHex, const FString& data);
};
