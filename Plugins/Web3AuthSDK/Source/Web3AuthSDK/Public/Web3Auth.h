// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Web3Auth.generated.h"

USTRUCT(BlueprintType)
struct FExtraLoginOptions
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

};


USTRUCT(BlueprintType)
struct FLoginConfigItem
{
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FString verifier;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FTypeOfLogin typeOfLogin;

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
        int32 typeOfLogin;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        bool mainOption;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        bool showOnModal;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        bool showOnDesktop;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        bool showOnMobile;

    FLoginConfigItem() {};

};


USTRUCT(BlueprintType)
struct FLoginParams
{
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FProvider loginProvider;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FString dappShare;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FExtraLoginOptions extraLoginOptions;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FString appState;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FString redirectUrl;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        int32 loginProvider;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FMFALevel mfaLevel;

    FLoginParams() {};

};


USTRUCT(BlueprintType)
struct FUserInfo
{
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

    FUserInfo() {};

};


USTRUCT(BlueprintType)
struct FWeb3AuthOptions
{

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FString clientId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FString redirectUrl;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FString sdkUrl;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FNetwork network;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FWhiteLabelData* whiteLabel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        TMap<FString, FLoginConfigItem> loginConfig;


    FWeb3AuthOptions() {};

};


USTRUCT(BlueprintType)
struct FWeb3AuthResponse
{

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FString privKey;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FString ed25519PrivKey;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FString error;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FUserInfo userInfo;

    FWeb3AuthResponse() {};

};



USTRUCT(BlueprintType)
struct FWhiteLabelData
{
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FString name;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FString logoLight;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FString logoDark;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FString defaultLanguage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        bool dark;

    FWhiteLabelData() {};

};

UENUM(BlueprintType)
enum class FDisplay
{
    PAGE,
    POPUP,
    TOUCH,
    WAP
};

UENUM(BlueprintType)
enum class FPrompt
{
    NONE,
    LOGIN,
    CONSENT,
    SELECT_ACCOUNT
};

UENUM(BlueprintType)
enum class FProvider
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
    JWT
};

UENUM(BlueprintType)
enum class FTypeOfLogin
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
    JWT
};

UENUM(BlueprintType)
enum class FMFALevel
{
    DEFAULT,
    OPTIONAL,
    MANDATORY,
    NONE
};

UENUM(BlueprintType)
enum class FNetwork
{
    MAINNET = 0, TESTNET = 1, CYAN = 2
};


UCLASS()
class WEB3AUTHSDK_API AWeb3Auth : public AActor
{
    GENERATED_BODY()

        FWeb3AuthOptions* web3AuthOptions;
    TMap<FString, FString>* initParams;

    FWeb3AuthResponse* web3AuthResponse;

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

public:
    AWeb3Auth();
    AWeb3Auth(FWeb3AuthOptions web3authOptions);

    UFUNCTION(BlueprintCallable)
        void login(FLoginParams* loginParams);

    UFUNCTION(BlueprintCallable)
        void logout(TMap<FString, std::any>* params);

    UFUNCTION(BlueprintCallable)
        void logout(FString redirectUrl = "", FString appState = "");

    void setResultUrl(FString uri);


    ~AWeb3Auth();
private:
    void request(FString  path, FLoginParams* loginParams = NULL, TMap<FString, std::any>* extraParams = NULL);

};
