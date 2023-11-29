#pragma once

#if PLATFORM_WINDOWS
	#pragma warning(disable:4996)
	#pragma comment(lib, "crypt32.lib")
	#pragma comment(lib, "WS2_32.lib")
#endif

#include "CoreMinimal.h"

#pragma comment(lib, "crypt32.lib")
#pragma comment(lib, "WS2_32.lib")

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
#include <openssl/rand.h>
#include <openssl/hmac.h>
THIRD_PARTY_INCLUDES_END
#undef UI

#include "Misc/SecureHash.h"
#include <string>
#include <iostream>
#include <vector>
#include "Keccak256.h"
#include "ECCrypto.generated.h"

UCLASS()
class WEB3AUTHSDK_API UECCrypto : public UObject
{
	GENERATED_BODY()
public:
	UECCrypto();
	~UECCrypto();

	FString decrypt(FString data, FString privateKeyHex, FString ephemPublicKeyHex, FString encryptionIvHex, FString macKeyHex); // todo: add FString macKeyHex to params
	FString encrypt(FString data, FString privateKeyHex, FString ephemPublicKeyHex, FString encryptionIvHex, unsigned char* mac_key); // consider changing unsinged char* to FString reference.

	FString generatePublicKey(const FString& privateKeyHex);
	FString generateECDSASignature(const FString& privateKeyHex, const FString& data);

	FString generateRandomSessionKey();
	EC_KEY* generateECKeyPair();
	FString convertBigNumToHex(const BIGNUM* bn);
	FString generateRandomBytes();

	TArray<uint8> getCombinedData(FString CipherTextHex, FString EphemPublicKeyHex, FString EncryptionIvHex);
    TArray<uint8> getMac(FString CipherTextHex, FString EphemPublicKeyHex, FString EncryptionIvHex, FString macKeyHex);
    TArray<uint8> hmacSha256Sign(const unsigned char*, const TArray<uint8>& Data);
    bool hmacSha256Verify(const unsigned char* key, const TArray<uint8>& data, const FString& expectedMac);
    TArray<uint8> fStringToByteArray(const FString& inputString);
};
