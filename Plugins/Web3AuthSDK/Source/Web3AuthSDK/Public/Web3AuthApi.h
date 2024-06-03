#pragma once

#include "CoreMinimal.h"
#include "Http.h"
#include "JsonUtilities.h"
#include "GenericPlatform/GenericPlatformHttp.h"
#include "Web3AuthApi.generated.h"

extern TMap<FString, FString> SIGNER_MAP;

UENUM(BlueprintType)
enum class FLanguage : uint8
{
	en, de, ja, ko, zh, es, fr, pt, nl, tr
};

UENUM(BlueprintType)
enum class FThemeModes : uint8
{
	light, dark
};


USTRUCT()
struct FWhitelistResponse
{
    GENERATED_BODY()

    UPROPERTY()
    TArray<FString> urls;

    UPROPERTY()
    TMap<FString, FString> signed_urls;
};

USTRUCT(BlueprintType)
struct FWhiteLabelData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString appName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString logoLight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString logoDark;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
    	FLanguage defaultLanguage = FLanguage::en;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    	FThemeModes mode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TMap<FString, FString> theme;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
    	FString appUrl;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    	bool useLogoLoader;

	FWhiteLabelData() {};

	void operator= (const FWhiteLabelData& other) {
		appName = other.appName;
		logoLight = other.logoLight;
		logoDark = other.logoDark;
		defaultLanguage = other.defaultLanguage;
		mode = other.mode;
		theme = other.theme;
		appUrl = other.appUrl;
		useLogoLoader = other.useLogoLoader;
	}

};

USTRUCT()
struct FProjectConfigResponse
{
    GENERATED_BODY()

    UPROPERTY()
    FWhiteLabelData whitelabel;

    UPROPERTY()
    bool sms_otp_enabled;

    UPROPERTY()
    bool wallet_connect_enabled;

    UPROPERTY()
    FString wallet_connect_project_id;

    UPROPERTY()
    FWhitelistResponse whitelist;
};

USTRUCT()
struct FStoreApiResponse
{
    GENERATED_BODY()

    UPROPERTY()
        FString message;

    UPROPERTY()
        bool success;
};

USTRUCT()
struct FLogoutApiRequest
{
    GENERATED_BODY()

    UPROPERTY()
        FString key;

    UPROPERTY()
        FString data;

    UPROPERTY()
        FString signature;

    UPROPERTY()
        int32 timeout;
};

USTRUCT()
struct FSessionResponse
{
    GENERATED_BODY()

    UPROPERTY()
    FString sessionId;
};

UCLASS()
class WEB3AUTHSDK_API UWeb3AuthApi : public UObject
{
    GENERATED_BODY()

public:
    static UWeb3AuthApi* GetInstance();

    // Authorize the user session
    void AuthorizeSession(const FString& key, const TFunction<void(FStoreApiResponse)> callback);

    // Logout the user session
    void Logout(const FLogoutApiRequest logoutApiRequest, const TFunction<void(FString)> callback);

    // Create the user session
    void CreateSession(const FLogoutApiRequest logoutApiRequest, const TFunction<void(FString)> callback);

    // Fetch the project config API
    void FetchProjectConfig(const FString& key, const FString& network, bool whitelist, const TFunction<void(FProjectConfigResponse)> callback);

private:
    // Private constructor to enforce singleton pattern
    UWeb3AuthApi();

    // Singleton instance of the Web3AuthApi class
    static UWeb3AuthApi* Instance;
};