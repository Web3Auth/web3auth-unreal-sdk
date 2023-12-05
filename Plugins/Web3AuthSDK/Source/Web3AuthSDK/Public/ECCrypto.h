#pragma once

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

	FString decrypt(FString data, FString privateKeyHex, FString ephemPublicKeyHex, FString encryptionIvHex, FString macKeyHex); 
	FString encrypt(FString data, FString privateKeyHex, FString ephemPublicKeyHex, FString encryptionIvHex, FString& mac_key);

	FString generatePublicKey(const FString& privateKeyHex);
	FString generateECDSASignature(const FString& privateKeyHex, const FString& data);

	FString generateRandomSessionKey();
	EC_KEY* generateECKeyPair();
	FString convertBigNumToHex(const BIGNUM* bn);
	FString generateRandomBytes();

    unsigned char* getCombinedData(FString CipherTextHex, FString EphemPublicKeyHex, FString EncryptionIvHex);
    unsigned char* getMac(FString CipherTextHex, FString EphemPublicKeyHex, FString EncryptionIvHex, FString macKeyHex);
    unsigned char* hmacSha256Sign(const unsigned char* key, const unsigned char* data);
    bool hmacSha256Verify(const unsigned char* key, const unsigned char* data, const FString& expectedMac);
    TArray<uint8> fStringToByteArray(const FString& inputString);
};
