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

    FString getCombinedData(FString CipherTextHex, FString EphemPublicKeyHex, FString EncryptionIvHex);
	FString getMac(FString CipherTextHex, FString EphemPublicKeyHex, FString EncryptionIvHex, FString macKeyHex);
	FString hmacSha256Sign(FString key, FString data);
    bool hmacSha256Verify(FString key, FString data, FString expectedMac);
};
