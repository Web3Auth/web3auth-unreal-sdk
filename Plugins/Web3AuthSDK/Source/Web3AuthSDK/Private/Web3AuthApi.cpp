// Fill out your copyright notice in the Description page of Project Settings.


#include "Web3AuthApi.h"

UWeb3AuthApi* UWeb3AuthApi::Instance = nullptr;

UWeb3AuthApi* UWeb3AuthApi::GetInstance()
{
    if (!Instance) {
        Instance = NewObject<UWeb3AuthApi>();
    }
    return Instance;
}


void UWeb3AuthApi::AuthorizeSession(const FString& key, const TFunction<void(FStoreApiResponse)> callback)
{
    TSharedRef<IHttpRequest> request = FHttpModule::Get().CreateRequest();
    request->SetVerb(TEXT("POST"));
    request->SetURL(TEXT("https://session.web3auth.io/store/get?key=" + key));
    
    FString FormString = "key=" + key;

    request->SetHeader(TEXT("Content-Type"), TEXT("application/x-www-form-urlencoded"));
    request->SetContentAsString(FormString);

    request->OnProcessRequestComplete().BindLambda([callback](FHttpRequestPtr request, FHttpResponsePtr response, bool success) {
        FString response_string = response->GetContentAsString();
        FString url = request->GetURL();
        UE_LOG(LogTemp, Log, TEXT("Response: %s"), *response_string);

        if (success && response->GetResponseCode() == EHttpResponseCodes::Ok) {
            FStoreApiResponse api_response;
            if (FJsonObjectConverter::JsonObjectStringToUStruct(response_string, &api_response, 0, 0)) {
                callback(api_response);
            }
            else {
                UE_LOG(LogTemp, Error, TEXT("Failed to parse response"));
            }
        }
        else {
            UE_LOG(LogTemp, Error, TEXT("Request failed"));
        }
    });

    request->ProcessRequest();
}

void UWeb3AuthApi::Logout(const FLogoutApiRequest logoutApiRequest, const TFunction<void(FString)> callback)
{
    TSharedRef<IHttpRequest> request = FHttpModule::Get().CreateRequest();
    request->SetVerb(TEXT("POST"));
    request->SetURL(TEXT("https://session.web3auth.io/store/set"));

    FString FormString = "key=" + logoutApiRequest.key + "&data=" + FGenericPlatformHttp::UrlEncode(logoutApiRequest.data) + "&signature=" + logoutApiRequest.signature + "&timeout=" + FString::FromInt(logoutApiRequest.timeout);

    request->SetHeader(TEXT("Content-Type"), TEXT("application/x-www-form-urlencoded"));
    request->SetContentAsString(FormString);

    request->OnProcessRequestComplete().BindLambda([callback](FHttpRequestPtr request, FHttpResponsePtr response, bool success) {
        FString response_string = response->GetContentAsString();
        UE_LOG(LogTemp, Log, TEXT("Response: %s "), *response_string);
        UE_LOG(LogTemp, Log, TEXT("Status code: %d "), response->GetResponseCode());

        if (success && response->GetResponseCode() == EHttpResponseCodes::Created) {
            callback(response_string);
        }
        else {
            UE_LOG(LogTemp, Error, TEXT("Request failed"));
        }
     });

    request->ProcessRequest();
}

void UWeb3AuthApi::CreateSession(const FLogoutApiRequest logoutApiRequest, const TFunction<void(FString)> callback)
{
    TSharedRef<IHttpRequest> request = FHttpModule::Get().CreateRequest();
    request->SetVerb(TEXT("POST"));
    request->SetURL(TEXT("https://session.web3auth.io/store/set"));

    FString FormString = "key=" + logoutApiRequest.key + "&data=" + FGenericPlatformHttp::UrlEncode(logoutApiRequest.data) + "&signature=" + logoutApiRequest.signature + "&timeout=" + FString::FromInt(logoutApiRequest.timeout);

    request->SetHeader(TEXT("Content-Type"), TEXT("application/x-www-form-urlencoded"));
    request->SetContentAsString(FormString);

    request->OnProcessRequestComplete().BindLambda([callback](FHttpRequestPtr request, FHttpResponsePtr response, bool success) {
        FString response_string = response->GetContentAsString();
        UE_LOG(LogTemp, Log, TEXT("Response: %s "), *response_string);
        UE_LOG(LogTemp, Log, TEXT("Status code: %d "), response->GetResponseCode());

        if (success && response->GetResponseCode() == EHttpResponseCodes::Created) {
            callback(response_string);
        }
        else {
            UE_LOG(LogTemp, Error, TEXT("Request failed"));
        }
     });

    request->ProcessRequest();
}

void UWeb3AuthApi::FetchProjectConfig(const FString& projectId, const FString& network, bool whitelist, const TFunction<void(FProjectConfigResponse)> callback)
{
    TSharedRef<IHttpRequest> request = FHttpModule::Get().CreateRequest();
    request->SetVerb(TEXT("GET"));

    FString baseUrl = SIGNER_MAP[network];
    FString path = FString::Printf(TEXT("/api/configuration?project_id=%s&network=%s&whitelist=%s"),
                                    *FString::UrlEncode(projectId),
                                    *FString::UrlEncode(network),
                                    whitelist ? TEXT("true") : TEXT("false"));
    FString URL = baseUrl + path;
    request->SetURL(URL);

    request->OnProcessRequestComplete().BindLambda([callback](FHttpRequestPtr request, FHttpResponsePtr response, bool success) {
        if (success && response->GetResponseCode() == EHttpResponseCodes::Ok) {
            FString responseString = response->GetContentAsString();
            UE_LOG(LogTemp, Log, TEXT("Response: %s"), *responseString);

            FProjectConfigResponse configResponse;
            if (FJsonObjectConverter::JsonObjectStringToUStruct(responseString, &configResponse, 0, 0)) {
                callback(configResponse);
            }
            else {
                UE_LOG(LogTemp, Error, TEXT("Failed to parse response"));
            }
        }
        else {
            UE_LOG(LogTemp, Error, TEXT("Request failed"));
        }
    });

    request->ProcessRequest();
}

TMap<FString, FString> SIGNER_MAP = {
    { "mainnet", "https://signer.web3auth.io" },
    { "testnet", "https://signer.web3auth.io" },
    { "cyan", "https://signer-polygon.web3auth.io" },
    { "aqua", "https://signer-polygon.web3auth.io" },
    { "sapphire_mainnet", "https://signer.web3auth.io" },
    { "sapphire_devnet", "https://signer.web3auth.io" }
};

UWeb3AuthApi::UWeb3AuthApi()
{
}
