#include "ECCrypto.h"
#include <string>
#include <vector>
#include <Web3Auth.h>

#pragma comment(lib, "crypt32.lib")
#pragma comment(lib, "WS2_32.lib")


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

FString UECCrypto::decrypt(FString data, FString privateKeyHex, FString ephemPublicKeyHex, FString encryptionIvHex)
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

	// Create a new encryption context for AES-256 CBC mode with the key and IV
	EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
	EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv);

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

FString UECCrypto::encrypt(FString data, FString privateKeyHex, FString ephemPublicKeyHex, FString encryptionIvHex)
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

	// Create the shared secret
	unsigned char* secret = new unsigned char[32];
	int secret_len = ECDH_compute_key(secret, EVP_MAX_KEY_LENGTH, EC_KEY_get0_public_key(pub_key), priv_key, NULL);

	// Calculate SHA-512 hash of secret
	unsigned char hash[SHA512_DIGEST_LENGTH];
	SHA512(secret, 32, hash);

	// Copy first 32 bytes of the hash into a new buffer
	unsigned char key[32];
	memcpy(key, hash, 32);

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

	return FString(UTF8_TO_TCHAR(dst.c_str()));
}

FString UECCrypto::generatePublicKey(const FString& privateKeyHex) {
	BIGNUM* bn_private_key = NULL;
	BN_hex2bn(&bn_private_key, TCHAR_TO_ANSI(*privateKeyHex));

	EC_KEY* ec_key = EC_KEY_new_by_curve_name(NID_secp256k1);
	EC_KEY_set_private_key(ec_key, bn_private_key);

	EC_POINT* ec_point = EC_POINT_new(EC_KEY_get0_group(ec_key));
	EC_POINT_mul(EC_KEY_get0_group(ec_key), ec_point, EC_KEY_get0_private_key(ec_key), NULL, NULL, NULL);
	EC_KEY_set_public_key(ec_key, ec_point);

	BIGNUM* bn = EC_POINT_point2bn(EC_KEY_get0_group(ec_key), EC_KEY_get0_public_key(ec_key), POINT_CONVERSION_UNCOMPRESSED, NULL, NULL);

	char* hex = BN_bn2hex(bn);
	FString result(UTF8_TO_TCHAR(hex));

	OPENSSL_free(hex);
	BN_free(bn_private_key);

	return result.ToLower();
}

FString UECCrypto::generateECDSASignature(const FString& privateKeyHex, const FString& data) {
	// Initialize OpenSSL's elliptic curve library
	EC_KEY* key = EC_KEY_new_by_curve_name(NID_secp256k1);

	BIGNUM* priv_bn = BN_new();
	BN_hex2bn(&priv_bn, FStringToCharArray(privateKeyHex));

	EC_KEY_set_private_key(key, priv_bn);

	const unsigned char* msg = (const unsigned char* ) FStringToCharArray(data);
	size_t msglen = data.Len();

	unsigned char hash[SHA256_DIGEST_LENGTH];
	Keccak256::getHash(msg, msglen, hash);

	unsigned char* sig_buf = nullptr;

	ECDSA_SIG* signature = ECDSA_do_sign(hash, SHA256_DIGEST_LENGTH, key);
	int n = i2d_ECDSA_SIG(signature, &sig_buf);

	//// Convert signature to hex string    
	FString signature_hex;    
	for (int i = 0; i < n; ++i) {        
		signature_hex += FString::Printf(TEXT("%02x"), sig_buf[i]);    
	}

	EC_KEY_free(key);
	ECDSA_SIG_free(signature);

	return signature_hex;
}

UECCrypto::~UECCrypto()
{
}


