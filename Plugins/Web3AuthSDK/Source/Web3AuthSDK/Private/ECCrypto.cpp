#include "ECCrypto.h"
#include <string>
#include <vector>
#include <iostream>

unsigned char* toByteArray(const std::string& s) {
	size_t len = s.length() / 2;
	unsigned char* data = new unsigned char[len];

	for (size_t i = 0; i < len * 2; i += 2) {
		int hi = std::stoi(s.substr(i, 1), nullptr, 16);
		int lo = std::stoi(s.substr(i + 1, 1), nullptr, 16);
		data[i / 2] = (unsigned char)((hi << 4) + lo);
	}

	return data;

}

char* FStringToCharArray(const FString& InString) {
	char* CharArray = new char[InString.Len() + 1];    
	
	strcpy(CharArray, TCHAR_TO_ANSI(*InString));    
	return CharArray;
}

UECCrypto::UECCrypto() {
}

FString UECCrypto::decrypt(FString data, FString privateKeyHex, FString ephemPublicKeyHex, FString encryptionIvHex, FString macKeyHex)
{
	// Convert to bytes array
	const char* priv_hex = FStringToCharArray(privateKeyHex);
	const char* pub_hex  = FStringToCharArray(ephemPublicKeyHex);

	// Decode IV key
	const unsigned char* iv = toByteArray(FStringToCharArray(encryptionIvHex));

	// Decode cipher text
	const unsigned char* src = toByteArray(FStringToCharArray(data));
	int srclen = data.Len() / 2;

	// Convert to BIGNUM
	BIGNUM* priv_bn = BN_new();
	BIGNUM* pub_bn = BN_new();
	BN_hex2bn(&priv_bn, priv_hex);
	BN_hex2bn(&pub_bn, pub_hex);

	// Create EC_KEY objects from the BIGNUMs
	EC_KEY* priv_key = EC_KEY_new_by_curve_name(NID_secp256k1);
	EC_KEY* pub_key = EC_KEY_new_by_curve_name(NID_secp256k1);
	EC_KEY_set_private_key(priv_key, priv_bn);
	EC_KEY_set_public_key(pub_key, EC_POINT_bn2point(EC_KEY_get0_group(pub_key), pub_bn, NULL, NULL));

	// Create the shared secret
	unsigned char* secret = new unsigned char[32];
	int secret_len = ECDH_compute_key(secret, EVP_MAX_KEY_LENGTH, EC_KEY_get0_public_key(pub_key), priv_key, NULL);

	// Calculate SHA-512 hash of secret
	unsigned char hash[SHA512_DIGEST_LENGTH];
	SHA512(secret, 32, hash);

	// Copy first 32 bytes of the hash into a new buffer
	unsigned char key[32];
	memcpy(key, hash, 32);

	unsigned char* mac_key = new unsigned char[SHA512_DIGEST_LENGTH - 32];
	memcpy(mac_key, hash + 32, SHA512_DIGEST_LENGTH - 32);

    //verifying mac
	if (!hmacSha256Verify(mac_key, getCombinedData(data, ephemPublicKeyHex, encryptionIvHex), macKeyHex))
    {
        // throw new BadMacException
        FString errorMessage = TEXT("Bad MAC error during decrypt");
        throw std::runtime_error(TCHAR_TO_UTF8(*errorMessage));
    }

	// Create a new encryption context for AES-256 CBC mode with the key and IV
	EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
	EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv);
	EVP_CIPHER_CTX_set_padding(ctx, 0);
	// Allocate a string buffer for the decrypted data
	std::string dst;
	dst.resize(srclen + EVP_CIPHER_block_size(EVP_aes_256_cbc()));

	// Decrypt the input data
	int outlen;
	EVP_DecryptUpdate(ctx, (unsigned char*)dst.data(), &outlen, src, srclen);

	// Finalize the decryption and retrieve any remaining data
	int finaloutlen;
	EVP_DecryptFinal_ex(ctx, (unsigned char*)dst.data() + outlen, &finaloutlen);

	// Resize the buffer to the actual decrypted length
	dst.resize(outlen + finaloutlen);

	// Free the encryption context
	EVP_CIPHER_CTX_free(ctx);

	// Clean up resources
	BN_free(priv_bn);
	BN_free(pub_bn);
	EC_KEY_free(priv_key);
	EC_KEY_free(pub_key);
	EVP_cleanup();

	return FString(dst.c_str());
}

FString UECCrypto::encrypt(FString data, FString privateKeyHex, FString ephemPublicKeyHex, FString encryptionIvHex, unsigned char* mac_key)
{
	// Convert to bytes array
	const char* priv_hex = FStringToCharArray(privateKeyHex);
	const char* pub_hex = FStringToCharArray(ephemPublicKeyHex);

	// Decode IV key
	const unsigned char* iv = toByteArray(FStringToCharArray(encryptionIvHex));

	// Decode cipher text
	const unsigned char* src = (unsigned char*)FStringToCharArray(data);
	int srclen = data.Len() / 2;

	// Convert to BIGNUM
	BIGNUM* priv_bn = BN_new();
	BIGNUM* pub_bn = BN_new();
	BN_hex2bn(&priv_bn, priv_hex);
	BN_hex2bn(&pub_bn, pub_hex);

	// Create EC_KEY objects from the BIGNUMs
	EC_KEY* priv_key = EC_KEY_new_by_curve_name(NID_secp256k1);
	EC_KEY* pub_key = EC_KEY_new_by_curve_name(NID_secp256k1);
	EC_KEY_set_private_key(priv_key, priv_bn);
	EC_KEY_set_public_key(pub_key, EC_POINT_bn2point(EC_KEY_get0_group(pub_key), pub_bn, NULL, NULL));

    mac_key = new unsigned char[SHA512_DIGEST_LENGTH - 32];
	// Create the shared secret
	unsigned char* secret = new unsigned char[32];
	int secret_len = ECDH_compute_key(secret, EVP_MAX_KEY_LENGTH, EC_KEY_get0_public_key(pub_key), priv_key, NULL);

	// Calculate SHA-512 hash of secret
	unsigned char hash[SHA512_DIGEST_LENGTH];
	SHA512(secret, 32, hash);

	// Copy first 32 bytes of the hash into a new buffer
	unsigned char key[32];
	memcpy(key, hash, 32);

	memcpy(mac_key, hash + 32, SHA512_DIGEST_LENGTH - 32);

	// Create a new encryption context for AES-256 CBC mode with the key and IV
	EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
	EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv);

	// Allocate a string buffer for the decrypted data
	std::string dst;
	dst.resize(srclen + EVP_CIPHER_block_size(EVP_aes_256_cbc()));

	// Decrypt the input data
	int outlen;
	EVP_EncryptUpdate(ctx, (unsigned char*)dst.data(), &outlen, src, srclen);

	// Finalize the decryption and retrieve any remaining data
	int finaloutlen;
	EVP_EncryptFinal_ex(ctx, (unsigned char*)dst.data() + outlen, &finaloutlen);

	// Resize the buffer to the actual decrypted length
	dst.resize(outlen + finaloutlen);

	// Free the encryption context
	EVP_CIPHER_CTX_free(ctx);

	// Clean up resources
	BN_free(priv_bn);
	BN_free(pub_bn);
	EC_KEY_free(priv_key);
	EC_KEY_free(pub_key);
	EVP_cleanup();

	const char* buf = dst.c_str();

	FString hex;
	for (int i = 0; i < strlen(buf); ++i) {
		hex += FString::Printf(TEXT("%02x"), buf[i]);
	}
	return hex;
}

FString UECCrypto::generatePublicKey(const FString& privateKeyHex) {
	BIGNUM* bn_private_key = nullptr;
	BN_hex2bn(&bn_private_key, FStringToCharArray(*privateKeyHex));

	EC_KEY* ec_key = EC_KEY_new_by_curve_name(NID_secp256k1);
	EC_KEY_set_private_key(ec_key, bn_private_key);

	EC_POINT* ec_point = EC_POINT_new(EC_KEY_get0_group(ec_key));
	EC_POINT_mul(EC_KEY_get0_group(ec_key), ec_point, EC_KEY_get0_private_key(ec_key), nullptr, nullptr, nullptr);
	EC_KEY_set_public_key(ec_key, ec_point);

	BIGNUM* bn = EC_POINT_point2bn(EC_KEY_get0_group(ec_key), EC_KEY_get0_public_key(ec_key), POINT_CONVERSION_UNCOMPRESSED, nullptr, nullptr);

	char* hex = BN_bn2hex(bn);
	FString result(UTF8_TO_TCHAR(hex));

	OPENSSL_free(hex);
	BN_free(bn_private_key);

	return result.ToLower();
}

FString UECCrypto::generateECDSASignature(const FString& privateKeyHex, const FString& data) {
	// Initialize OpenSSL's elliptic curve library
	EC_KEY* key = EC_KEY_new_by_curve_name(NID_secp256k1);

	BIGNUM* priv_bn = nullptr;
	BN_hex2bn(&priv_bn, FStringToCharArray(privateKeyHex));

	EC_KEY_set_private_key(key, priv_bn);

	const unsigned char* msg = (const unsigned char* ) FStringToCharArray(data);
	size_t msglen = data.Len();

	unsigned char hash[SHA256_DIGEST_LENGTH];
	Keccak256::getHash(msg, msglen, hash);

	unsigned char* sig_buf = nullptr;

	ECDSA_SIG* signature = ECDSA_do_sign(hash, SHA256_DIGEST_LENGTH, key);

    BN_CTX *ctx = BN_CTX_new();
    BN_CTX_start(ctx);
    const EC_GROUP *group = EC_KEY_get0_group(key);
    BIGNUM *order = BN_CTX_get(ctx);
    BIGNUM *halforder = BN_CTX_get(ctx);
    EC_GROUP_get_order(group, order, ctx);
    BN_rshift1(halforder, order);
	const BIGNUM* pr = nullptr;
	const BIGNUM* ps = nullptr;
	ECDSA_SIG_get0(signature, &pr, &ps);
    if (BN_cmp(ps, halforder) > 0) {
        // enforce low S values. libsecp256k1 does this by default, openssl does not.
        BN_sub(const_cast<BIGNUM*>(ps), order, ps);
    }
    BN_CTX_end(ctx);
    BN_CTX_free(ctx);

	int n = i2d_ECDSA_SIG(signature, &sig_buf);

	//// Convert signature to hex string
	FString signature_hex;
	for (int i = 0; i < n; ++i) {
		signature_hex += FString::Printf(TEXT("%02x"), sig_buf[i]);
	}

	EC_KEY_free(key);
	ECDSA_SIG_free(signature);
	BN_free(priv_bn);

	return signature_hex;
}

// Generate session key (private key) for ECDH locally
FString UECCrypto::generateRandomSessionKey() {
    EC_KEY* ecKeyPair = generateECKeyPair();

    if (!ecKeyPair) {
        return FString(); // handle error
    }

    const BIGNUM* privateKeyBN = EC_KEY_get0_private_key(ecKeyPair);

    FString privateKeyStr = convertBigNumToHex(privateKeyBN);

    EC_KEY_free(ecKeyPair);

    return privateKeyStr;
}

// Generate a new EC key pair
EC_KEY* UECCrypto::generateECKeyPair() {
    EC_KEY* ecKey = EC_KEY_new_by_curve_name(NID_secp256k1);

    if (!ecKey) {
        UE_LOG(LogTemp, Error, TEXT("Error creating EC_KEY structure."));
        return nullptr;
    }

    if (EC_KEY_generate_key(ecKey) != 1) {
        UE_LOG(LogTemp, Error, TEXT("Error generating EC key pair."));
        EC_KEY_free(ecKey);
        return nullptr;
    }

    return ecKey;
}

// Convert a BIGNUM to a hexadecimal FString
FString UECCrypto::convertBigNumToHex(const BIGNUM* bn) {
    char* hex = BN_bn2hex(bn);
    FString result(hex);
    OPENSSL_free(hex);
    return result;
}

FString UECCrypto::generateRandomBytes() {
    // Generate random bytes
    const int32 numBytes = 32;
    unsigned char* buffer = new unsigned char[numBytes];
    RAND_bytes(buffer, numBytes);

    // Convert to hexadecimal FString
    FString HexString;
    for (int32 i = 0; i < numBytes; ++i) {
        HexString += FString::Printf(TEXT("%02x"), buffer[i]);
    }

    delete[] buffer;
    return HexString;
}

// getCombinedData combines the IV, ephemeral public key, and cipher text into a single byte array
TArray<uint8> UECCrypto::getCombinedData(FString cipherTextHex, FString ephemPublicKeyHex, FString encryptionIvHex)
{
    TArray<uint8> combinedData;

	const unsigned char* cipherBytes = toByteArray(FStringToCharArray(cipherTextHex));

    // Decode IV key
    const unsigned char* iv = toByteArray(FStringToCharArray(encryptionIvHex));

    // Decode ephem key
    const unsigned char* ephem = toByteArray(FStringToCharArray(ephemPublicKeyHex));

    combinedData.Append(iv, sizeof(iv));
    combinedData.Append(ephem, sizeof(ephem));
    combinedData.Append(cipherBytes, sizeof(cipherBytes));

    return combinedData;
}

TArray<uint8> UECCrypto::getMac(FString cipherTextHex, FString ephemPublicKeyHex, FString encryptionIvHex, FString macKeyHex)
{
    TArray<uint8> combinedData = getCombinedData(cipherTextHex, ephemPublicKeyHex, encryptionIvHex);

	const unsigned char* mac = toByteArray(FStringToCharArray(macKeyHex));
    return hmacSha256Sign(mac, combinedData);
}

TArray<uint8> UECCrypto::hmacSha256Sign(const unsigned char* key, const TArray<uint8>& data)
{
    TArray<uint8> result;
    unsigned int resultLen = SHA256_DIGEST_LENGTH; // 256-bit hash

    HMAC_CTX* HmacCtx = HMAC_CTX_new();
    if (!HmacCtx)
    {
        UE_LOG(LogTemp, Error, TEXT("Error creating HMAC context."));
        return result;
    }

    HMAC_Init_ex(HmacCtx, key, sizeof(key), EVP_sha256(), nullptr);
    HMAC_Update(HmacCtx, data.GetData(), data.Num());
    HMAC_Final(HmacCtx, result.GetData(), &resultLen);

    HMAC_CTX_free(HmacCtx);

    result.SetNumUninitialized(resultLen);
    return result;
}

// hmacSha256Verify verifies that the calculated MAC matches the expected MAC
bool UECCrypto::hmacSha256Verify(const unsigned char* key, const TArray<uint8>& data, const FString& expectedMac)
{
    const TArray<uint8>& expectedMacBytes = fStringToByteArray(expectedMac);
    if (sizeof(key) == 0 || data.Num() == 0 || expectedMacBytes.Num() == 0)
    {
        return false;
    }

    TArray<uint8> calculatedMac = hmacSha256Sign(key, data);

    if (calculatedMac.Num() != expectedMacBytes.Num())
    {
        // The lengths of the calculated and expected MACs are different
        return false;
    }

    return FMemory::Memcmp(calculatedMac.GetData(), expectedMacBytes.GetData(), calculatedMac.Num()) == 0;
}

TArray<uint8> UECCrypto::fStringToByteArray(const FString& inputString)
{
    const TCHAR* CharArray = *inputString;
    int32 Size = FCString::Strlen(CharArray);

    // Convert to UTF-8
    TArray<uint8> resultArray;
    FTCHARToUTF8 converter(CharArray);

    resultArray.SetNumUninitialized(converter.Length());

    if (resultArray.Num() > 0)
    {
        FMemory::Memcpy(resultArray.GetData(), converter.Get(), resultArray.Num());
    }

    return resultArray;
}

UECCrypto::~UECCrypto()
{
}


