#pragma once

#include "CoreMinimal.h"
#include "Http.h"
#include "JsonUtilities.h"
#include "GenericPlatform/GenericPlatformHttp.h"
#include "Web3AuthApi.generated.h"

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

private:
    // Private constructor to enforce singleton pattern
    UWeb3AuthApi();

    // Singleton instance of the Web3AuthApi class
    static UWeb3AuthApi* Instance;
};