// Fill out your copyright notice in the Description page of Project Settings.


#include "Web3Auth.h"
#include "Web3AuthError.h"

#if PLATFORM_IOS
#include "IOS/ObjC/WebAuthenticate.h"
#endif

#if PLATFORM_ANDROID
JNI_METHOD void Java_com_epicgames_unreal_GameActivity_onDeepLink(JNIEnv* env, jclass clazz, jstring uri) {
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv(true)) {
		const char* UTFString = Env->GetStringUTFChars(uri, 0);

		FString result = FString(UTF8_TO_TCHAR(UTFString));
		UE_LOG(LogTemp, Warning, TEXT("redirect %s"), *result);

		UWeb3Auth::setResultUrl(result);

		Env->ReleaseStringUTFChars(uri, UTFString);
		Env->DeleteLocalRef(uri);
	}
}

void UWeb3Auth::CallJniVoidMethod(JNIEnv* Env, const jclass Class, jmethodID Method, ...) {
	va_list Args;
	va_start(Args, Method);
	Env->CallStaticVoidMethodV(Class, Method, Args);
	va_end(Args);

	Env->DeleteLocalRef(Class);
}
#endif

void UWeb3Auth::setOptions(FWeb3AuthOptions options) {
	this->web3AuthOptions = options;
	authorizeSession();
}

void UWeb3Auth::request(FString  path, FLoginParams* loginParams = NULL, TSharedPtr<FJsonObject> extraParams = NULL) {
	TSharedPtr<FJsonObject> paramMap = MakeShareable(new FJsonObject);


	TSharedPtr<FJsonObject> initParams = MakeShareable(new FJsonObject);
	initParams->SetStringField("clientId", web3AuthOptions.clientId);

	switch (web3AuthOptions.network) {
		case FNetwork::MAINNET:
			initParams->SetStringField("network", "mainnet");
			break;
		case FNetwork::TESTNET:
			initParams->SetStringField("network", "testnet");
			break;
		case FNetwork::CYAN:
			initParams->SetStringField("network", "cyan");
			break;
        case FNetwork::AQUA:
            initParams->SetStringField("network", "aqua");
            break;
        case FNetwork::SAPPHIRE_DEVNET:
            initParams->SetStringField("network", "sapphire_devnet");
            break;
        case FNetwork::SAPPHIRE_MAINNET:
            initParams->SetStringField("network", "sapphire_mainnet");
            break;
	}

	if (web3AuthOptions.redirectUrl != "")
		initParams->SetStringField("redirectUrl", web3AuthOptions.redirectUrl);

	FMfaSettings defaultMFA;

	if (!(web3AuthOptions.mfaSettings == defaultMFA))
    {
        FString mfaSettingsJson;
        FJsonObjectConverter::UStructToJsonObjectString(web3AuthOptions.mfaSettings, mfaSettingsJson);
        initParams->SetStringField(TEXT("mfaSettings"), mfaSettingsJson);
    }

    if (web3AuthOptions.sessionTime > 0)
    {
         initParams->SetNumberField(TEXT("sessionTime"), web3AuthOptions.sessionTime);
    }

#if !PLATFORM_ANDROID && !PLATFORM_IOS
	FString redirectUrl = startLocalWebServer();
	initParams->SetStringField("redirectUrl", redirectUrl);
#endif

    switch (web3AuthOptions.buildEnv) {
        case FBuildEnv::PRODUCTION:
        	initParams->SetStringField("buildEnv", "production");
        	break;
        case FBuildEnv::TESTING:
        	initParams->SetStringField("buildEnv", "testing");
        	break;
        case FBuildEnv::STAGING:
        	initParams->SetStringField("buildEnv", "staging");
        	break;
    }

	if (web3AuthOptions.whiteLabel.appName != "") {
		FString output;
		this->GetJsonStringFromStruct(web3AuthOptions.whiteLabel, output);

		initParams->SetStringField("whiteLabel", output);
	}

	if (!web3AuthOptions.loginConfig.IsEmpty()) {
		FString output;

		TSharedPtr<FJsonObject> loginConfigMap = MakeShareable(new FJsonObject);

		for (auto item : web3AuthOptions.loginConfig) {
			TSharedPtr<FJsonObject> loginConfigObject = MakeShareable(new FJsonObject);
			FJsonObjectConverter::UStructToJsonObject(FLoginConfigItem::StaticStruct(), &item.Value, loginConfigObject.ToSharedRef(), 0, 0);

			loginConfigMap->SetObjectField(item.Key, loginConfigObject);
		}

		TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&output);
		FJsonSerializer::Serialize(loginConfigMap.ToSharedRef(), Writer);

		initParams->SetStringField("loginConfig", output);
	}

	paramMap->SetObjectField("options", initParams.ToSharedRef());
	paramMap->SetStringField("actionType", "login");

	TSharedPtr<FJsonObject> params = MakeShareable(new FJsonObject);

	if (extraParams != NULL) {
		params = extraParams;
	}

	if (loginParams != NULL) {
		for (auto o : loginParams->getJsonObject().Values) {
			params->SetField(o.Key, o.Value);
		}
	}

	paramMap->SetObjectField("params", params.ToSharedRef());

	FString json;
    TSharedRef< TJsonWriter<> > jsonWriter = TJsonWriterFactory<>::Create(&json);
	FJsonSerializer::Serialize(paramMap.ToSharedRef(), jsonWriter);

	//const FString jsonOutput = json;
	//FString base64 = FBase64::Encode(jsonOutput);

	if (web3AuthOptions.buildEnv == FBuildEnv::STAGING) {
        web3AuthOptions.sdkUrl = "https://staging-auth.web3auth.io/v5";
    }
    else if(web3AuthOptions.buildEnv == FBuildEnv::TESTING) {
        web3AuthOptions.sdkUrl = "https://develop-auth.web3auth.io";
    } else {
        web3AuthOptions.sdkUrl = "https://auth.web3auth.io/v5";
    }

    FString loginId = createSession(json, 600);
    if (!loginId.IsEmpty())
    {
        TSharedPtr<FJsonObject> loginIdObject = MakeShareable(new FJsonObject);
        loginIdObject->SetStringField(TEXT("loginId"), loginId);

        // Convert to Base64
        FString output;
        TSharedRef< TJsonWriter<> > outputWriter = TJsonWriterFactory<>::Create(&output);
        FJsonSerializer::Serialize(loginIdObject.ToSharedRef(), outputWriter);
        FString encode = FBase64::Encode(output);

        // Build the URI
        FString url = web3AuthOptions.sdkUrl + "/" + path + "#" + "b64Params=" + encode;

        #if PLATFORM_ANDROID
            if (JNIEnv* Env = FAndroidApplication::GetJavaEnv(true)) {
            jstring jurl = Env->NewStringUTF(TCHAR_TO_UTF8(*url));

            jclass jbrowserViewClass = FAndroidApplication::FindJavaClass("com/Plugins/Web3AuthSDK/BrowserView");
            static jmethodID jlaunchUrl = FJavaWrapper::FindStaticMethod(Env, jbrowserViewClass, "launchUrl", "(Landroid/app/Activity;Ljava/lang/String;)V", false);

            CallJniVoidMethod(Env, jbrowserViewClass, jlaunchUrl, FJavaWrapper::GameActivityThis, jurl);
        }
        #elif PLATFORM_IOS
            [[WebAuthenticate Singleton] launchUrl:TCHAR_TO_ANSI(*url)];
        #else
            FPlatformProcess::LaunchURL(*url, NULL, NULL);
            #endif
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create session."));
    }
}

void UWeb3Auth::processLogin(FLoginParams loginParams) {
	UE_LOG(LogTemp, Warning, TEXT("login called"));
	this->request("start", &loginParams);
}

void UWeb3Auth::processLogout() {
	sessionTimeout();
}

void UWeb3Auth::setResultUrl(FString hash) {
	
	if (hash.IsEmpty()) {
		return;
	}


	UE_LOG(LogTemp, Warning, TEXT("respose base64 %s"), *hash);

	FString json = "";

	FString output = hash;
	output = output.Replace(TEXT("-"), TEXT("+"));
	output = output.Replace(TEXT("_"), TEXT("/"));
	switch (output.Len() % 4)	{
		case 0: break;
		case 2: output += "=="; break;
		case 3: output += "="; break;
		default: 
			return;
	}

	FBase64::Decode(output, json);

	UE_LOG(LogTemp, Warning, TEXT("respose json %s"), *json);

	if (!FJsonObjectConverter::JsonObjectStringToUStruct(json, &web3AuthResponse, 0, 0)) {
		UE_LOG(LogTemp, Warning, TEXT("failed to parse json"));
	}

	if (web3AuthResponse.error != "") {
		return;
	}

	if (web3AuthResponse.privKey.IsEmpty() || web3AuthResponse.privKey == "0000000000000000000000000000000000000000000000000000000000000000") {
		this->logoutEvent.ExecuteIfBound();
	}
	else {
		this->sessionId = web3AuthResponse.sessionId;
		this->loginEvent.ExecuteIfBound(web3AuthResponse);
	}
}

template <typename StructType>
void UWeb3Auth::GetJsonStringFromStruct(StructType FilledStruct, FString& StringOutput) {
	FJsonObjectConverter::UStructToJsonObjectString(StructType::StaticStruct(), &FilledStruct, StringOutput, 0, 0);
}

FString UWeb3Auth::startLocalWebServer() {
	FHttpServerModule& httpServerModule = FHttpServerModule::Get();

	httpServerModule.StopAllListeners();

	int32 port = FMath::RandRange(1024, 65535);
	httpRouter = httpServerModule.GetHttpRouter(port);

	while (!httpRouter.IsValid()) {
		httpRouter.Reset();

		port = FMath::RandRange(1024, 65535);
		httpRouter = httpServerModule.GetHttpRouter(port);
	}

	if (httpRouter.IsValid()) {
		auto x = httpRouter->BindRoute(FHttpPath(TEXT("/auth")), EHttpServerRequestVerbs::VERB_GET,
			[this](const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete) { return requestAuthCallback(Request, OnComplete); });

		auto y = httpRouter->BindRoute(FHttpPath(TEXT("/complete")), EHttpServerRequestVerbs::VERB_GET,
			[this](const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete) { return requestCompleteCallback(Request, OnComplete); });

		httpRoutes.Add(TPairInitializer<TSharedPtr<IHttpRouter>, FHttpRouteHandle>(httpRouter, x));
		httpRoutes.Add(TPairInitializer<TSharedPtr<IHttpRouter>, FHttpRouteHandle>(httpRouter, y));
	}

	httpServerModule.StartAllListeners();

	return "http://localhost:"+ FString::FromInt(port) + "/complete";
}


bool UWeb3Auth::requestAuthCallback(const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete) {
	FString code = Request.QueryParams["code"];

	if (!code.IsEmpty()) {
		UWeb3Auth::setResultUrl(code);
	}

	TUniquePtr<FHttpServerResponse> response = FHttpServerResponse::Create(TEXT("OK"), TEXT("text/html"));
	OnComplete(MoveTemp(response));

	FHttpServerModule::Get().StopAllListeners();
	for (auto route : httpRoutes) {
		route.Key->UnbindRoute(route.Value);
	}
	httpRoutes.Empty();

	return true;
}

bool UWeb3Auth::requestCompleteCallback(const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete)
{
	FString text = R"html(
		<!DOCTYPE html>
		<html>
		<head>
			<meta charset="utf-8">
			<meta name='viewport' content='width=device-width'>
			<title>Web3Auth</title>
			<link href='https://fonts.googleapis.com/css2?family=DM+Sans:wght@500&display=swap' rel='stylesheet'>
		</head>
		<body style="padding:0;margin:0;font-size:10pt;font-family: 'DM Sans', sans-serif;">
			<div style="display:flex;align-items:center;justify-content:center;height:100vh;display: none;" id="success">
			<div style="text-align:center">
				<h2 style="margin-bottom:0"> Authenticated successfully</h2>
				<p> You can close this tab/window now </p>
			</div>
			</div>
			<div style="display:flex;align-items:center;justify-content:center;height:100vh;display: none;" id="error">
			<div style="text-align:center">
				<h2 style="margin-bottom:0"> Authentication failed</h2>
				<p> Please try again </p>
			</div>
			</div>
			<script>
			if (window.location.hash.trim() == "") {
				document.querySelector("#error").style.display="flex";
			} else {
				fetch(`http://${window.location.host}/auth/?code=${window.location.hash.slice(1,window.location.hash.length)}`).then(function(response) {
					console.log(response);
					document.querySelector("#success").style.display="flex";
				}).catch(function(error) {
					console.log(error);
					document.querySelector("#error").style.display="flex";
				});
			}
                    
			</script>
		</body>
		</html>)html";

	TUniquePtr<FHttpServerResponse> response = FHttpServerResponse::Create(*text, TEXT("text/html"));
	OnComplete(MoveTemp(response));

	return true;
}

void UWeb3Auth::setLoginEvent(FOnLogin _event) {
	loginEvent = _event;
}

void UWeb3Auth::setLogoutEvent(FOnLogout _event) {
	logoutEvent = _event;
}

#if PLATFORM_IOS
void UWeb3Auth::callBackFromWebAuthenticateIOS(NSString* sResult) {
    FString result = FString(sResult);
    UWeb3Auth::setResultUrl(result);
}
#endif


FString UWeb3Auth::getPrivKey() {
	if (web3AuthResponse.coreKitKey.IsEmpty() || web3AuthResponse.privKey.IsEmpty()) {
		return "";
	}

	return web3AuthOptions.useCoreKitKey ? web3AuthResponse.coreKitKey : web3AuthResponse.privKey;
}

FString UWeb3Auth::getEd25519PrivKey() {
	if (web3AuthResponse.coreKitEd25519PrivKey.IsEmpty() || web3AuthResponse.ed25519PrivKey.IsEmpty()) {
		return "";
	}

	return web3AuthOptions.useCoreKitKey ? web3AuthResponse.coreKitEd25519PrivKey : web3AuthResponse.ed25519PrivKey;
}

FUserInfo UWeb3Auth::getUserInfo() {
	if (web3AuthResponse.userInfo.IsEmpty()) {
		FString error = Web3AuthError::getError(ErrorCode::NOUSERFOUND);
		UE_LOG(LogTemp, Fatal, TEXT("%s"), *error);

		return FUserInfo();
	}

	return web3AuthResponse.userInfo;
}

void UWeb3Auth::authorizeSession() {
	if (!this->sessionId.IsEmpty()) {
		FString pubKey = crypto->generatePublicKey(this->sessionId);
		UE_LOG(LogTemp, Log, TEXT("public key %s"), *pubKey);
		FString session = this->sessionId;
		web3AuthApi->AuthorizeSession(pubKey, [session, this](FStoreApiResponse response)
			{
				UE_LOG(LogTemp, Log, TEXT("Response: %s"), *response.message);

				FShareMetaData shareMetaData;

				if (!FJsonObjectConverter::JsonObjectStringToUStruct(response.message, &shareMetaData, 0, 0)) {
					UE_LOG(LogTemp, Error, TEXT("failed to parse json"));
					return;
				}

				FString output = crypto->decrypt(shareMetaData.ciphertext, session, shareMetaData.ephemPublicKey, shareMetaData.iv, shareMetaData.mac);
				UE_LOG(LogTemp, Log, TEXT("output %s"), *output);
		
				TSharedPtr<FJsonObject> tempJson;
				TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(output);

				if (FJsonSerializer::Deserialize(JsonReader, tempJson) && tempJson.IsValid()) {
					tempJson->SetObjectField("userInfo", tempJson->GetObjectField("store"));
					tempJson->RemoveField("store");

					FString json;
					TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&json);
					FJsonSerializer::Serialize(tempJson.ToSharedRef(), Writer);

					if (!FJsonObjectConverter::JsonObjectStringToUStruct(json, &web3AuthResponse, 0, 0)) {
						UE_LOG(LogTemp, Error, TEXT("failed to parse json"));
						return;
					}

					if (web3AuthResponse.error != "") {
						return;
					}

					this->loginEvent.ExecuteIfBound(web3AuthResponse);
				}

		});
	}
}

void UWeb3Auth::sessionTimeout() {

	if (!this->sessionId.IsEmpty()) {
		FString pubKey = crypto->generatePublicKey(this->sessionId);
		UE_LOG(LogTemp, Log, TEXT("public key %s"), *pubKey);
		web3AuthApi->AuthorizeSession(pubKey, [pubKey, this](FStoreApiResponse response)
			{
				FShareMetaData shareMetaData;

				if (!FJsonObjectConverter::JsonObjectStringToUStruct(response.message, &shareMetaData, 0, 0)) {
					UE_LOG(LogTemp, Error, TEXT("failed to parse json"));
					return;
				}

				unsigned char* mac_key = nullptr;
				FString encryptedData = crypto->encrypt("", this->sessionId, shareMetaData.ephemPublicKey, shareMetaData.iv, mac_key);
				shareMetaData.ciphertext = encryptedData;

				TSharedPtr<FJsonObject> jsonObject = MakeShareable(new FJsonObject);
				FJsonObjectConverter::UStructToJsonObject(FShareMetaData::StaticStruct(), &shareMetaData, jsonObject.ToSharedRef(), 0, 0);

				FString jsonString;
				TSharedRef<TJsonWriter<TCHAR>> jsonWriter = TJsonWriterFactory<>::Create(&jsonString);
				FJsonSerializer::Serialize(jsonObject.ToSharedRef(), jsonWriter);
				FString sig = crypto->generateECDSASignature(this->sessionId, jsonString);

				FLogoutApiRequest request;
				request.data = jsonString;
				request.key = pubKey;
				request.signature = sig;
				request.timeout = 1;

				web3AuthApi->Logout(request, [this](FString response)
					{
						UE_LOG(LogTemp, Log, TEXT("Response: %s"), *response);
						this->logoutEvent.ExecuteIfBound();
						this->sessionId = FString();
					});
		});
	}
}

FString UWeb3Auth::createSession(const FString& jsonData, int32 sessionTime) {
    FString newSessionKey = crypto->generateRandomSessionKey();
    UE_LOG(LogTemp, Warning, TEXT("newSessionKey => %s"), *newSessionKey);

    FString ephemPublicKey = crypto->generatePublicKey(newSessionKey);
    UE_LOG(LogTemp, Warning, TEXT("ephemPublicKey => %s"), *ephemPublicKey);

    FString ivKey = crypto->generateRandomBytes();
    UE_LOG(LogTemp, Warning, TEXT("ivKey => %s"), *ivKey);

	unsigned char* mac_key = nullptr;
    FString encryptedData = crypto->encrypt(jsonData, newSessionKey, ephemPublicKey, ivKey, mac_key);
    UE_LOG(LogTemp, Warning, TEXT("encryptedData => %s"), *encryptedData);

    FString macKeyHex;
    for (int i = 0; i < sizeof(mac_key); ++i) {
   		macKeyHex += FString::Printf(TEXT("%02x"), mac_key[i]);
   	}
    UE_LOG(LogTemp, Warning, TEXT("macKeyHex => %s"), *macKeyHex);

    TArray<uint8> macKey = crypto->getMac(encryptedData, ephemPublicKey, ivKey, macKeyHex);
	unsigned char* macKeyPtr = macKey.GetData();
    FString finalMac;
        for (int i = 0; i < sizeof(macKeyPtr); ++i) {
       		finalMac += FString::Printf(TEXT("%02x"), macKeyPtr[i]);
       	}

    FShareMetaData shareMetaData;
    shareMetaData.ciphertext = encryptedData;
    shareMetaData.ephemPublicKey = ephemPublicKey;
    shareMetaData.iv = ivKey;
    shareMetaData.mac = finalMac;

    TSharedPtr<FJsonObject> jsonObject = MakeShareable(new FJsonObject);
    FJsonObjectConverter::UStructToJsonObject(FShareMetaData::StaticStruct(), &shareMetaData, jsonObject.ToSharedRef(), 0, 0);

    FString jsonString;
    TSharedRef<TJsonWriter<TCHAR>> jsonWriter = TJsonWriterFactory<>::Create(&jsonString);
    FJsonSerializer::Serialize(jsonObject.ToSharedRef(), jsonWriter);
    FString sig = crypto->generateECDSASignature(newSessionKey, jsonString);

    FLogoutApiRequest request;
    request.data = jsonString;
    request.key = ephemPublicKey;
    request.signature = sig;
    request.timeout = FMath::Min(sessionTime, 7 * 86400);

    web3AuthApi->CreateSession(request, [this, newSessionKey](FString response)
    	{
    	    UE_LOG(LogTemp, Log, TEXT("Response: %s"), *response);
    	    return FString(newSessionKey);
    	});

    return FString("");
}

void UWeb3Auth::Initialize(FSubsystemCollectionBase& Collection) {
	Super::Initialize(Collection);
	this->crypto = NewObject<UECCrypto>();
}

void UWeb3Auth::Deinitialize() {
	Super::Deinitialize();
	FHttpServerModule::Get().StopAllListeners();

	for (auto route : httpRoutes) {
		route.Key->UnbindRoute(route.Value);
	}
	httpRoutes.Empty();
	this->crypto = nullptr;
	this->web3AuthApi = nullptr;
}

