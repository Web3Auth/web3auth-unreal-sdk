
#include "Web3Auth.h"

#include <Runtime/JsonUtilities/Public/JsonObjectConverter.h>
#include "Misc/Base64.h"

// Sets default values
AWeb3Auth::AWeb3Auth()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

}

AWeb3Auth::AWeb3Auth(FWeb3AuthOptions web3authOptions) {
    this->web3AuthOptions = web3AuthOptions;

    this->initParams = new TMap<FString, FString>();

    this->initParams->Add("clientId", web3AuthOptions->clientId);
    this->initParams->Add("network", StaticEnum<FNetwork>()->GetNameStringByValue(static_cast<__underlying_type(FNetwork)>(web3AuthOptions->network)));

    if (web3AuthOptions->redirectUrl != "")
        this->initParams->Add("redirectUrl", web3AuthOptions->redirectUrl);


    if (web3AuthOptions->whiteLabel != NULL) {
        FString output = "";
        FJsonObjectConverter::UStructToJsonObjectString(web3AuthOptions->whiteLabel, output);

        this->initParams->Add("whiteLabel", output);
    }

    if (!web3AuthOptions->loginConfig.IsEmpty()) {
        FString output = "";
        FJsonObjectConverter::UStructToJsonObjectString(web3AuthOptions->loginConfig, output);

        this->initParams->Add("loginConfig", output);
    }

}

void AWeb3Auth::request(FString  path, FLoginParams* loginParams = NULL, TMap<FString, std::any>* extraParams = NULL) {
    TMap<FString, std::any> paramMap;

    paramMap["init"] = this->initParams;
    paramMap["params"] = TMap<FString, std::any>();

    TMap<FString, std::any> params;
    for (auto extraParam : *extraParams) {
        params[extraParam.Key] = extraParam.Value;
    }

    params["appState"] = loginParams->appState;
    params["dappShare"] = loginParams->dappShare;
    params["extraLoginOptions"] = loginParams->extraLoginOptions;
    params["loginProvider"] = loginParams->loginProvider;
    params["mfaLevel"] = loginParams->mfaLevel;
    params["redirectUrl"] = loginParams->redirectUrl;


    FString json = "";
    FJsonObjectConverter::UStructToJsonObjectString(paramMap, json);

    const FString jsonOutput = json;
    FString base64 = FBase64::Encode(jsonOutput);

    FString url = this->web3AuthOptions->sdkUrl + "/" + path + "#" + base64;
    FPlatformProcess::LaunchURL(*url, NULL, NULL);
}

void AWeb3Auth::login(FLoginParams* loginParams) {
    this->request("login", loginParams);
}

void AWeb3Auth::logout(TMap<FString, std::any>* params) {
    this->request("logout", NULL, params);
}

void AWeb3Auth::logout(FString redirectUrl = "", FString appState = "") {
    TMap<FString, std::any> extraParams;

    if (redirectUrl != "")
        extraParams["redirectUrl"] = redirectUrl;

    if (appState != "")
        extraParams["appState"] = appState;

    this->logout(&extraParams);
}

void AWeb3Auth::setResultUrl(FString uri) {
    FString* path = new FString();
    FString* fragment = new FString();

    uri.Split("#", path, fragment);

    if (*fragment == "") {
        return;
    }

    FString json = "";
    const FString hash = *fragment;

    FBase64::Decode(hash, json);

    FJsonObjectConverter::JsonObjectStringToUStruct(json, this->web3AuthResponse);
    if (this->web3AuthResponse->error != "") {
        return;
    }

    if (this->web3AuthResponse->privKey == "" || this->web3AuthResponse->privKey.TrimChar('0') == "") {
        //logout
    }
    else {
        //login
    }

}

void AWeb3Auth::BeginPlay()
{
    Super::BeginPlay();

}

void AWeb3Auth::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

}

AWeb3Auth::~AWeb3Auth() {

}

