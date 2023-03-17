// Fill out your copyright notice in the Description page of Project Settings.


#include "Web3AuthApi.h"
#include "GenericPlatform/GenericPlatformHttp.h"

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
    request->SetVerb(TEXT("GET"));
    request->SetURL(TEXT("https://broadcast-server.tor.us/store/get?key=" + key));
    auto headers = request->GetAllHeaders();
    for (auto header : headers) {
        UE_LOG(LogTemp, Log, TEXT("length: %s %s"), *header);

    }
   

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
    request->SetURL(TEXT("https://broadcast-server.tor.us/store/set"));

    FString FormString = "key=" + logoutApiRequest.key + "&data=" + FGenericPlatformHttp::UrlEncode(logoutApiRequest.data) + "&signature=" + logoutApiRequest.signature + "&timeout=" + FString::FromInt(logoutApiRequest.timeout);
    UE_LOG(LogTemp, Error, TEXT("%s"), *FormString);

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

UWeb3AuthApi::UWeb3AuthApi()
{
}
