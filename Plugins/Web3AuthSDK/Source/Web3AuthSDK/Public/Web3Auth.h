#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "JsonUtilities.h"

#include "ECCrypto.h"
#include "KeyStoreUtils.h"
#include "Web3AuthApi.h"
#include "Misc/Base64.h"
#include "Dom/JsonValue.h"

#include "Containers/UnrealString.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

#include "Runtime/Online/HTTPServer/Public/IHttpRouter.h"
#include "HttpServerModule.h"

#if PLATFORM_ANDROID
#include "../../../Launch/Public/Android/AndroidJNI.h"
#include "Android/AndroidApplication.h"
#endif

#include "Web3Auth.generated.h"

UENUM(BlueprintType)
enum class FDisplay : uint8
{
	PAGE,
	POPUP,
	TOUCH,
	WAP
};

UENUM(BlueprintType)
enum class FPrompt : uint8
{
	NONE,
	LOGIN,
	CONSENT,
	SELECT_ACCOUNT
};

UENUM(BlueprintType)
enum class FProvider : uint8
{
	GOOGLE,
	FACEBOOK,
	REDDIT,
	DISCORD,
	TWITCH,
	APPLE,
	LINE,
	GITHUB,
	KAKAO,
	LINKEDIN,
	TWITTER,
	WEIBO,
	WECHAT,
	EMAIL_PASSWORDLESS,
	EMAIL_PASSWORD,
	JWT,
    SMS_PASSWORDLESS,
    FARCASTER
};

UENUM(BlueprintType)
enum class FTypeOfLogin : uint8
{
	GOOGLE,
	FACEBOOK,
	REDDIT,
	DISCORD,
	TWITCH,
	APPLE,
	LINE,
	GITHUB,
	KAKAO,
	LINKEDIN,
	TWITTER,
	WEIBO,
	WECHAT,
	EMAIL_PASSWORDLESS,
	EMAIL_PASSWORD,
	JWT,
    SMS_PASSWORDLESS,
    FARCASTER
};

UENUM(BlueprintType)
enum class FCurve : uint8
{
    SECP256K1,
    ED25519
};

UENUM(BlueprintType)
enum class FMFALevel : uint8
{
	DEFAULT,
	OPTIONAL,
	MANDATORY,
	NONE
};

UENUM(BlueprintType)
enum class FNetwork : uint8
{
	MAINNET = 0, TESTNET = 1, CYAN = 2, AQUA = 3, SAPPHIRE_DEVNET = 4, SAPPHIRE_MAINNET = 5
};

UENUM(BlueprintType)
enum class FChainNamespace : uint8
{
    EIP155,
    SOLANA
};

UENUM(BlueprintType)
enum class FBuildEnv : uint8
{
    PRODUCTION,
    STAGING,
	TESTING
};

USTRUCT(BlueprintType)
struct WEB3AUTHSDK_API FExtraLoginOptions
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString domain;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString client_id;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString leeway;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString verifierIdField;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString max_age;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString ui_locales;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FString id_token;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString id_token_hint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString login_hint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString acr_values;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString scope;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString audience;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString connection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString state;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString response_type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString nonce;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString redirect_uri;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool isVerifierIdCaseSensitive;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FDisplay display;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FPrompt prompt;

	FExtraLoginOptions() {};

	TSharedPtr<FJsonObject> getJsonObject() {
		TSharedPtr<FJsonObject> output = MakeShareable(new FJsonObject);

		if (!domain.IsEmpty())
			output->SetStringField("domain", domain);

		if (!client_id.IsEmpty())
			output->SetStringField("client_id", client_id);

		if (!leeway.IsEmpty())
			output->SetStringField("leeway", leeway);

		if (!verifierIdField.IsEmpty())
			output->SetStringField("verifierIdField", verifierIdField);

		if (!max_age.IsEmpty())
			output->SetStringField("max_age", max_age);

		if (!ui_locales.IsEmpty())
			output->SetStringField("ui_locales", ui_locales);

        if (!id_token.IsEmpty())
            output->SetStringField("id_token", id_token);

		if (!id_token_hint.IsEmpty())
			output->SetStringField("id_token_hint", id_token_hint);

		if (!login_hint.IsEmpty())
			output->SetStringField("login_hint", login_hint);

		if (!acr_values.IsEmpty())
			output->SetStringField("acr_values", acr_values);

		if (!scope.IsEmpty())
			output->SetStringField("scope", scope);

		if (!audience.IsEmpty())
			output->SetStringField("audience", audience);

		if (!connection.IsEmpty())
			output->SetStringField("connection", connection);

		if (!state.IsEmpty())
			output->SetStringField("state", state);

		if (!response_type.IsEmpty())
			output->SetStringField("response_type", response_type);

		if (!nonce.IsEmpty())
			output->SetStringField("nonce", nonce);

		if (!redirect_uri.IsEmpty())
			output->SetStringField("redirect_uri", redirect_uri);


		if (output->Values.IsEmpty()) {
			return nullptr;
		}

		output->SetBoolField("isVerifierIdCaseSensitive", isVerifierIdCaseSensitive);

		switch (display) {
		case FDisplay::PAGE:
			output->SetStringField("display", "page");
			break;
		case FDisplay::POPUP:
			output->SetStringField("display", "popup");
			break;
		case FDisplay::TOUCH:
			output->SetStringField("display", "touch");
			break;
		case FDisplay::WAP:
			output->SetStringField("display", "wap");
			break;
		}

		switch (prompt) {
		case FPrompt::CONSENT:
			output->SetStringField("prompt", "consent");
			break;
		case FPrompt::LOGIN:
			output->SetStringField("prompt", "login");
			break;
		case FPrompt::NONE:
			output->SetStringField("prompt", "none");
			break;
		case FPrompt::SELECT_ACCOUNT:
			output->SetStringField("prompt", "select_acocunt");
			break;
		}

		return output;
	}

};

USTRUCT(BlueprintType)
struct FLoginConfigItem
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString verifier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString typeOfLogin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString clientId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString verifierSubIdentifier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString logoHover;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString logoLight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString logoDark;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool mainOption;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool showOnModal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool showOnDesktop;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool showOnMobile;

	FLoginConfigItem() {};

	bool operator== (const FLoginConfigItem& other) {
		return other.clientId == clientId;
	}

};


FORCEINLINE uint32 GetTypeHash(const FLoginConfigItem& other) {
	return GetTypeHash(other.clientId);
}


USTRUCT(BlueprintType)
struct FLoginParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString loginProvider;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString dappShare;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FExtraLoginOptions extraLoginOptions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString appState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString redirectUrl;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FMFALevel mfaLevel = FMFALevel::OPTIONAL;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FCurve curve = FCurve::SECP256K1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FString dappUrl;

	FLoginParams() {};

	FJsonObject getJsonObject() {
		FJsonObject output;

		if (!appState.IsEmpty())
			output.SetStringField("appState", appState);

		if (!dappShare.IsEmpty())
			output.SetStringField("dappShare", dappShare);

		if (!loginProvider.IsEmpty())
			output.SetStringField("loginProvider", loginProvider);

		if (!redirectUrl.IsEmpty())
			output.SetStringField("redirectUrl", redirectUrl);

		if (extraLoginOptions.getJsonObject() != nullptr)
			output.SetObjectField("extraLoginOptions", extraLoginOptions.getJsonObject());

		if(!dappUrl.IsEmpty())
            output.SetStringField("dappUrl", dappUrl);

		return output;
	}
};


USTRUCT(BlueprintType)
struct FUserInfo
{
	GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FString email;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString profileImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString aggregateVerifier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString verifier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString verifierId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString typeOfLogin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString dappShare;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString idToken;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FString oAuthIdToken;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString oAuthAccessToken;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
    	bool isMfaEnabled;

	FUserInfo() {};

	bool IsEmpty() const {
    		return email.IsEmpty()
    			&& name.IsEmpty()
    			&& profileImage.IsEmpty()
    			&& aggregateVerifier.IsEmpty()
    			&& verifier.IsEmpty()
    			&& verifierId.IsEmpty()
    			&& typeOfLogin.IsEmpty()
    			&& dappShare.IsEmpty()
    			&& idToken.IsEmpty()
    			&& oAuthIdToken.IsEmpty()
    			&& oAuthAccessToken.IsEmpty();
    }

	FString ToString() const {
		return FString::Printf(TEXT("email: %s, name: %s, profileImage: %s, aggregateVerifier: %s, verifier: %s, verifierId: %s, typeOfLogin: %s, dappShare: %s, idToken: %s, oAuthIdToken: %s, oAuthAccessToken: %s, isMfaEnabled: %d"),
			*email, *name, *profileImage, *aggregateVerifier, *verifier, *verifierId, *typeOfLogin, *dappShare, *idToken, *oAuthIdToken, *oAuthAccessToken, isMfaEnabled);
	}
};

USTRUCT(BlueprintType)
struct FChainConfig
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FChainNamespace chainNamespace = FChainNamespace::EIP155;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 decimals = 18;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString blockExplorerUrl;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString chainId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString displayName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString logo;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString rpcTarget;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString ticker;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString tickerName;

    FChainConfig() {};

    void operator= (const FChainConfig& other) {
        chainNamespace = other.chainNamespace;
        decimals = other.decimals;
        blockExplorerUrl = other.blockExplorerUrl;
        chainId = other.chainId;
        displayName = other.displayName;
        logo = other.logo;
        rpcTarget = other.rpcTarget;
        ticker = other.ticker;
        tickerName = other.tickerName;
    }
};

USTRUCT(BlueprintType)
struct FMfaSetting
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
    	bool enable;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        int32 priority;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        bool mandatory;

	FMfaSetting() {};

	void operator= (const FMfaSetting& other) {
		enable = other.enable;
		priority = other.priority;
		mandatory = other.mandatory;
	}

	bool operator==(const FMfaSetting& other) const
    {
    if (enable == other.enable &&
	    priority == other.priority &&
	    mandatory == other.mandatory) 
		{
			return true;
		}
	return false;
    }
};

USTRUCT(BlueprintType)
struct FMfaSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
    	FMfaSetting deviceShareFactor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    	FMfaSetting backUpShareFactor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FMfaSetting socialBackupFactor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FMfaSetting passwordFactor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FMfaSetting passkeysFactor;

   UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FMfaSetting authenticatorFactor;

	FMfaSettings() {};

	void operator= (const FMfaSettings& other) {
		deviceShareFactor = other.deviceShareFactor;
		backUpShareFactor = other.backUpShareFactor;
		socialBackupFactor = other.socialBackupFactor;
		passwordFactor = other.passwordFactor;
		passkeysFactor = other.passkeysFactor;
		authenticatorFactor = other.authenticatorFactor;
	}

	bool operator==(const FMfaSettings& other) const
    {
    if (deviceShareFactor == other.deviceShareFactor &&
	    backUpShareFactor == other.backUpShareFactor  &&
	    socialBackupFactor == other.socialBackupFactor &&
	    passwordFactor == other.passwordFactor &&
	    passkeysFactor == other.passkeysFactor &&
	    authenticatorFactor == other.authenticatorFactor)
		{
			return true;
		}
	return false;
    }

};

USTRUCT(BlueprintType)
struct FWeb3AuthOptions
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString clientId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString redirectUrl;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString sdkUrl = "https://sdk.openlogin.com/v8";

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FString walletSdkUrl = "https://wallet.web3auth.io/v1";

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FNetwork network;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FBuildEnv buildEnv;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FWhiteLabelData whiteLabel;

	UPROPERTY(BlueprintReadWrite)
		TMap<FString, FLoginConfigItem> loginConfig;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FChainNamespace chainNamespace;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        bool useCoreKitKey;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    	FMfaSettings mfaSettings;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        int32 sessionTime = 86400;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    	FChainConfig chainConfig;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TMap<FString, FString> originData;

	FWeb3AuthOptions() {};

	void operator= (const FWeb3AuthOptions& other) {
		clientId = other.clientId;
		redirectUrl = other.redirectUrl;
		sdkUrl = other.sdkUrl;
		redirectUrl = other.redirectUrl;
		network = other.network;
		buildEnv = other.buildEnv;
		whiteLabel = other.whiteLabel;
		loginConfig = other.loginConfig;
		chainNamespace = other.chainNamespace;
        useCoreKitKey = other.useCoreKitKey;
        mfaSettings = other.mfaSettings;
        sessionTime = other.sessionTime;
        chainConfig = other.chainConfig;
		originData = other.originData;
	}

};

USTRUCT(BlueprintType)
struct FWeb3AuthResponse
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString privKey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString ed25519PrivKey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString error;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString sessionId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FUserInfo userInfo;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FString coreKitKey;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FString coreKitEd25519PrivKey;

	FWeb3AuthResponse() {};

};
USTRUCT(BlueprintType)
struct FShareMetaData
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
		FString iv;

	UPROPERTY(BlueprintReadWrite)
		FString ephemPublicKey;

	UPROPERTY(BlueprintReadWrite)
		FString ciphertext;

	UPROPERTY(BlueprintReadWrite)
		FString mac;
};


DECLARE_DYNAMIC_DELEGATE_OneParam(FOnLogin, FWeb3AuthResponse, response);
DECLARE_DYNAMIC_DELEGATE(FOnLogout);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnMfaSetup, bool, response);

UCLASS()
class WEB3AUTHSDK_API UWeb3Auth : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	TSharedPtr<IHttpRouter> httpRouter;
	TArray<TPair<TSharedPtr<IHttpRouter>, FHttpRouteHandle>> httpRoutes;
	UPROPERTY(Transient)
	UECCrypto* crypto;
	UPROPERTY(Transient)
	UWeb3AuthApi* web3AuthApi = UWeb3AuthApi::GetInstance();
	FWeb3AuthOptions web3AuthOptions;
	FOnLogin loginEvent;
	FOnLogout logoutEvent;
    FOnMfaSetup mfaEvent;
    UKeyStoreUtils* keyStoreUtils;

protected:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;
public:
	UWeb3Auth();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FWeb3AuthResponse web3AuthResponse;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString sessionId = FString();

	UFUNCTION(BlueprintCallable)
		void setOptions(FWeb3AuthOptions web3authOptions);

	UFUNCTION(BlueprintCallable)
		void processLogin(FLoginParams loginParams);

	UFUNCTION(BlueprintCallable)
		void processLogout();

	UFUNCTION(BlueprintCallable)
        void enableMFA(FLoginParams loginParams);

    UFUNCTION(BlueprintCallable)
        void launchWalletServices(FChainConfig chainConfig);

	UFUNCTION(BlueprintCallable)
	void request(FChainConfig chainConfig, FString method, TArray<FString> requestParams, FString path = "wallet/request");

	UFUNCTION(BlueprintCallable)
		void setResultUrl(FString code);

	UFUNCTION(BlueprintCallable, Category = "Web3Auth")
		void setLoginEvent(FOnLogin _event);

	UFUNCTION(BlueprintCallable, Category = "Web3Auth")
		void setLogoutEvent(FOnLogout _event);

    UFUNCTION(BlueprintCallable, Category = "Web3Auth")
        void setMfaEvent(FOnMfaSetup _event);

	UFUNCTION(BlueprintCallable)
		FString Web3AuthResponseToJsonString(FWeb3AuthResponse response) {
		FString output;
		FJsonObjectConverter::UStructToJsonObjectString(FWeb3AuthResponse::StaticStruct(), &response, output, 0, 0);

		return output;
	}

    UFUNCTION(BlueprintCallable)
    		FString getPrivKey();

    UFUNCTION(BlueprintCallable)
    		FString getEd25519PrivKey();

    UFUNCTION(BlueprintCallable)
    		FUserInfo getUserInfo();

	UFUNCTION(BlueprintCallable)
	static void setSignResponse(const FSignResponse Response);

	UFUNCTION(BlueprintPure)
	static FSignResponse getSignResponse();

    #if PLATFORM_IOS
    static void callBackFromWebAuthenticateIOS(NSString* sResult);
    #endif
private:
	void processRequest(FString  path, FLoginParams* loginParams, TSharedPtr<FJsonObject> extraParam);

	bool bIsRequestResponse;
	static FSignResponse signResponse;

	template <typename StructType>
	void GetJsonStringFromStruct(StructType FilledStruct, FString& StringOutput);

#if PLATFORM_ANDROID
	void CallJniVoidMethod(JNIEnv* Env, const jclass Class, jmethodID Method, ...);
#endif

	FString startLocalWebServer();

	bool requestAuthCallback(const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete);
	static bool requestCompleteCallback(const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete);

	void authorizeSession();
	void sessionTimeout();
	void createSession(const FString& jsonData, int32 sessionTime, bool isWalletService);
void handleCreateSessionResponse(const FString& path, const FString& newSessionKey, bool isWalletService);
    void fetchProjectConfig();
	FWhiteLabelData mergeWhiteLabelData(const FWhiteLabelData& other);
	static TMap<FString, FString> mergeMaps(const TMap<FString, FString>& map1, const TMap<FString, FString>& map2);
};
