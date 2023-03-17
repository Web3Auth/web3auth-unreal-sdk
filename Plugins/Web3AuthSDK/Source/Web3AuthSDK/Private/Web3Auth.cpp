// Fill out your copyright notice in the Description page of Project Settings.


#include "Web3Auth.h"

#if PLATFORM_IOS
#include "IOS/ObjC/WebAuthenticate.h"
#endif

FOnLogin AWeb3Auth::loginEvent;
FOnLogout AWeb3Auth::logoutEvent;

UKeyStoreUtils* AWeb3Auth::keyStoreUtils;
UECCrypto* AWeb3Auth::crypto;

#if PLATFORM_ANDROID
JNI_METHOD void Java_com_epicgames_unreal_GameActivity_onDeepLink(JNIEnv* env, jclass clazz, jstring uri) {
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv(true)) {
		const char* UTFString = Env->GetStringUTFChars(uri, 0);

		FString result = FString(UTF8_TO_TCHAR(UTFString));
		UE_LOG(LogTemp, Warning, TEXT("redirect %s"), *result);

		AWeb3Auth::setResultUrl(result);

		Env->ReleaseStringUTFChars(uri, UTFString);
		Env->DeleteLocalRef(uri);
	}
}

void AWeb3Auth::CallJniVoidMethod(JNIEnv* Env, const jclass Class, jmethodID Method, ...) {
	va_list Args;
	va_start(Args, Method);
	Env->CallStaticVoidMethodV(Class, Method, Args);
	va_end(Args);

	Env->DeleteLocalRef(Class);
}
#endif

// Sets default values
AWeb3Auth::AWeb3Auth()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AWeb3Auth::setOptions(FWeb3AuthOptions options) {
	this->web3AuthOptions = options;
	this->keyStoreUtils = NewObject<UKeyStoreUtils>();
	this->crypto = NewObject<UECCrypto>();

	authorizeSession();
}

void AWeb3Auth::request(FString  path, FLoginParams* loginParams = NULL, TSharedPtr<FJsonObject> extraParams = NULL) {
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

	}

	if (web3AuthOptions.redirectUrl != "")
		initParams->SetStringField("redirectUrl", web3AuthOptions.redirectUrl);

#if !PLATFORM_ANDROID && !PLATFORM_IOS
	FString redirectUrl = startLocalWebServer();
	initParams->SetStringField("redirectUrl", redirectUrl);
#endif

	if (web3AuthOptions.whiteLabel.name != "") {
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

	paramMap->SetObjectField("init", initParams.ToSharedRef());

	
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

	TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&json);
	FJsonSerializer::Serialize(paramMap.ToSharedRef(), Writer);

	const FString jsonOutput = json;
	FString base64 = FBase64::Encode(jsonOutput);

	FString url = web3AuthOptions.sdkUrl + "/" + path + "#" + base64;

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

void AWeb3Auth::processLogin(FLoginParams loginParams) {
	UE_LOG(LogTemp, Warning, TEXT("login called"));
	this->request("login", &loginParams);
}

/*void AWeb3Auth::logout(FJsonObject params) {
	this->request("logout", NULL, &params);
}*/

void AWeb3Auth::proccessLogout(FString redirectUrl, FString appState) {
	TSharedPtr<FJsonObject> extraParams = MakeShareable(new FJsonObject);

	if (redirectUrl != "")
		extraParams->SetStringField("redirectUrl", redirectUrl);

	if (appState != "")
		extraParams->SetStringField("appState", appState);

	//this->request("logout", NULL, extraParams);
	sessionTimeout();
}

void AWeb3Auth::setResultUrl(FString hash) {
	
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


	FWeb3AuthResponse web3AuthResponse;

	if (!FJsonObjectConverter::JsonObjectStringToUStruct(json, &web3AuthResponse, 0, 0)) {
		UE_LOG(LogTemp, Warning, TEXT("failed to parse json"));
	}

	if (web3AuthResponse.error != "") {
		return;
	}

	if (web3AuthResponse.privKey.IsEmpty() || web3AuthResponse.privKey == "0000000000000000000000000000000000000000000000000000000000000000") {
		AsyncTask(ENamedThreads::GameThread, [=]() {
			AWeb3Auth::logoutEvent.ExecuteIfBound();
		});	
	}
	else {
		AWeb3Auth::keyStoreUtils->Add("sessionid", web3AuthResponse.sessionId);
		AsyncTask(ENamedThreads::GameThread, [=]() {
			AWeb3Auth::loginEvent.ExecuteIfBound(web3AuthResponse);
		});
	}
}

template <typename StructType>
void AWeb3Auth::GetJsonStringFromStruct(StructType FilledStruct, FString& StringOutput) {
	FJsonObjectConverter::UStructToJsonObjectString(StructType::StaticStruct(), &FilledStruct, StringOutput, 0, 0);
}

FString AWeb3Auth::startLocalWebServer() {
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


bool AWeb3Auth::requestAuthCallback(const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete) {
	FString code = Request.QueryParams["code"];

	if (!code.IsEmpty()) {
		AWeb3Auth::setResultUrl(code);
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

bool AWeb3Auth::requestCompleteCallback(const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete)
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

void AWeb3Auth::setLoginEvent(FOnLogin _event) {
	loginEvent = _event;
}

void AWeb3Auth::setLogoutEvent(FOnLogout _event) {
	logoutEvent = _event;
}

#if PLATFORM_IOS
void AWeb3Auth::callBackFromWebAuthenticateIOS(NSString* sResult) {
    FString result = FString(sResult);
    AWeb3Auth::setResultUrl(result);
}
#endif

void AWeb3Auth::BeginPlay() {
	Super::BeginPlay();
}


void AWeb3Auth::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	Super::EndPlay(EndPlayReason);

	FHttpServerModule::Get().StopAllListeners();

	for (auto route : httpRoutes) {
		route.Key->UnbindRoute(route.Value);
	}
	httpRoutes.Empty();
}

void AWeb3Auth::authorizeSession() {
	FString sessionId = AWeb3Auth::keyStoreUtils->Get("sessionid");
	if (!sessionId.IsEmpty()) {
		FString pubKey = crypto->generatePublicKey(sessionId);
		UE_LOG(LogTemp, Log, TEXT("public key %s"), *pubKey);

		web3AuthApi->AuthorizeSession(pubKey, [](FStoreApiResponse response)
			{
				UE_LOG(LogTemp, Log, TEXT("Response: %s"), *response.message);
			});

		FString jsonString = "{\"iv\":\"e9b09a4e58c0763c6c6547ed76f3c481\",\"ephemPublicKey\":\"044d52f016e2b7374381e0bf48b240803083702c7739afc36766c6f62f3080cb0c3b0a1da22f3644ad5d447c9d494b78ad82271043729d8fc373b8754c4eb8f5b3\",\"ciphertext\":\"3b1a647d7203dbeeac8e538164465d4040fe1697e15ea881c3e1926595dd92332226e04e3512b0b4934ee403b058be1da5934d8d44f7137f121084c433b190865483dab1baaf372468ecb511404519b56aca6eaaf4a69aaa8add30145a4795ed20da83b8689f1c175e8e8d95e5a798925b86768f83baa6dbb4f81fc92f1113ee2241691211bcadb6b7d2e7635ec5c9ef06f136f2e647844d31b6895402bcf1802ba08412bcf553a92e188ead80c212197ad053e8c4fbed67039e683d177b48121d8b823015d6e5dc850b50aaa2e95ca2af5f6951153386e0f98621332b2495caf45abe8a0b49790ca1b1e05c3af55138612682efd4d28367cbfbd273c95292e92a2ff073946ca689a2e6b9709a4b08ce2782c57f871f3c13fea55add85a32441d22d8ce56de588eebf55e3132bcaa1d03a6ca48726de6e05047c9fabbb7f4aeef0501fbcaa839a73ea9ce6b3947511f1cb341db8af35b3c2ab89365e784b0f1485fc3be2f7723541cce55ae3ede78efb09ecca8918b0514c620ce56e41efa98e715f91d04e813f27b78bec10da61e9259b1ec7aca2c3fb062590dda5ca510c3800955988830c0173dc2f5d911bfcb0fc1d23881a21df03f5e0e3a8a040ac223fce20199e47a8a1d16d511fa3758a937984f47a111f7da8bd6d87ff484b3c0b98fc889bd75a3893d16e51dcf1d493c4fb99f194cd474a77a7fce541c99a3924a5be5bd78477082f40bf3771f76d8a150372fbcf64a8a19f990a3170ae5867c168f371a28df90e78e31420eba031aff880b46cc9c96021dca5d975baba412e396f74092e360d62f72fea547e7698e80442ee251246627e0a095b72c374ce5da4cb877e49f8acefa442987028f8a3aed5a41e95752b4c70af8fcd995310b3c3f68104fd05bd3546e78624d61f1fd46dbd26add39207501fd4f16a07d57eeded8d5ed91b50f25ce8993c982ecde5cffa09f94c70843f3c61749920ded84e0de345361605e8a2b9054359d76901c8f6ea0df09f81f91b7e6e88d0cc5e2e07facebb57fce27f433406cdcc03397da92ecd6b26a0a79c760e913a562520fb3b0e9b944e027d854891726176f860474a105eb44a49f86d284c24f7d4b20e5a8e265f23f01c6ab166a7cdf4ef2f65e95860654f083b4b72f00b593bc74e269ef49c33bc3fe7fdaab84744c69b247a9f77dac681ceff5cba1f225d5bc62f582a6fd64b5d942c8fd03883e7d0618cbb87647eb4c037964e1cef35ab42423ab375005678b80cd9d5c6f9b574d32489339583dba81be4f637d898dcd350ddfd38bf18f67636dcc398a0e16c984d7c7f5dee6e2bb8b7b63d9c581e47da06c5c6734de858a12d3c2ab09b38163d3be1741195c4eb07152f38e5fc65372807d5517ff2f8ebb047a5868b773983f78d663140c6532dd09078ae54b1c9fedf3e2a2ba9857010ee6c4ee09533a0df2418e523b3e5f9e297e22a0de3ed398e8302b95507689833476679577b479266a5b1b65d95227f36451dad497eeec9838778c59c7ebdc3989c20e66f423fde732ad5a9a85cbd71e9418903787d09db4526b4d6f987ace4874309b9a52d635bdccbd2734dc73a4cfbf373d601a7e6e7c8eb075b6438bb329528426a63b0392896c064b86de68a004842a1380feb4710a5e6ffe7df13e5540d0d1b4f77d007d8d76427c985fd0a6a31b92a400bd24a6edd69c507687448bdc7c2e556753a2f62fce9e7d7525a9f90b5f5572b62f5c38aa954168cadd775782379cfe2846016d437b2a2df58ed8884f90e58fd85109ae8a39ac4802a9ba0a32f707d39e843e419d097e1a24acb05a5ea9305a839a659be5475d58bb89548c5348204ff8aad84138824329605e4363b88b770903057a1231a219223616dc041f1dc279234c3432859e2e94a6b0686ecfa35f02e6ec771f3d60e05b0b619e8be9adff848879e08fc5464ff57b9a12376398b30d7c94120217e9ebdad023911ab7f54fb6c287fda2149ea7f96123b0c92022367baee0089dbd99bf572d79278c235fe51baf6b13f60aa64204d5ed57ac82d5b7251a5d2b93a65fd323f8878e20718489a72ae482356d5a78b11a2c3bfad66a6f5e3fb617256ad0cf989b01144a052d8d5d28295bbe5a61cfcc16475394044678d006488a36110eae7490b1e58fee997098b63e65eb917d7236e77f909d0fa47771d1d57c68d931e26760f010582dd4d8493b3181eee36679c2aa3d9af2dccfa8fbc2fd5b380de2bf27e38f1b6ef8537c940cac5cdb5517142f86ab9c6dd26cfb9ad7bbc96b8a750c9f1713d623599642db6d79b2f0c62ce69ff042d73afca214504e2a9da7d30ff17bc5abffaa6895bdb34e3af9aced0af52c2c3944ec2e9aa3619219f662bb3c7b692cc1bd39934807897fb32d8ff08d0aa76aa928c6a094398db968c97d6faea7c4eaf8a2e714199cef772e08e7481fc1033dcacbb8e847aac9c68b021e2ec6b644e3b458e2a456694b6cc7ffffbda8798cba6a9d3f0f9f75e5873eaac006d7f7e7f0619ab44293981dbdf5bac1793561274e435c793f67ae755a7c5256166035e3149da3f03a9bd0504\", \"mac\":\"50bc2e8d29368875bf214eea319f8aa5336337a95d2685359438d84a61bd17a3\"}";

		FShareMetaData shareMetaData;

		if (!FJsonObjectConverter::JsonObjectStringToUStruct(jsonString, &shareMetaData, 0, 0)) {
			UE_LOG(LogTemp, Error, TEXT("failed to parse json"));
			return;
		}

		FString output = crypto->decrypt(shareMetaData.ciphertext, sessionId, shareMetaData.ephemPublicKey, shareMetaData.iv);
		UE_LOG(LogTemp, Log, TEXT("output %s"), *output);
		
		TSharedPtr<FJsonObject> tempJson;
		TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(output);

		if (FJsonSerializer::Deserialize(JsonReader, tempJson) && tempJson.IsValid()) {
			tempJson->SetObjectField("userInfo", tempJson->GetObjectField("store"));
			tempJson->RemoveField("store");

			FString json;
			TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&json);
			FJsonSerializer::Serialize(tempJson.ToSharedRef(), Writer);

			FWeb3AuthResponse web3AuthResponse;

			if (!FJsonObjectConverter::JsonObjectStringToUStruct(json, &web3AuthResponse, 0, 0)) {
				UE_LOG(LogTemp, Error, TEXT("failed to parse json"));
				return;
			}

			if (web3AuthResponse.error != "") {
				return;
			}

			AsyncTask(ENamedThreads::GameThread, [=]() {
				AWeb3Auth::loginEvent.ExecuteIfBound(web3AuthResponse);
				});
		}
	}
}

void AWeb3Auth::sessionTimeout() {
	FString sessionId = AWeb3Auth::keyStoreUtils->Get("sessionid");

	if (!sessionId.IsEmpty()) {
		FString pubKey = crypto->generatePublicKey(sessionId);
		UE_LOG(LogTemp, Log, TEXT("public key %s"), *pubKey);

		web3AuthApi->AuthorizeSession(pubKey, [](FStoreApiResponse response)
			{
				UE_LOG(LogTemp, Log, TEXT("Response: %s"), *response.message);
			});

		FString jsonOutput = "{\"iv\":\"e9b09a4e58c0763c6c6547ed76f3c481\",\"ephemPublicKey\":\"044d52f016e2b7374381e0bf48b240803083702c7739afc36766c6f62f3080cb0c3b0a1da22f3644ad5d447c9d494b78ad82271043729d8fc373b8754c4eb8f5b3\",\"ciphertext\":\"3b1a647d7203dbeeac8e538164465d4040fe1697e15ea881c3e1926595dd92332226e04e3512b0b4934ee403b058be1da5934d8d44f7137f121084c433b190865483dab1baaf372468ecb511404519b56aca6eaaf4a69aaa8add30145a4795ed20da83b8689f1c175e8e8d95e5a798925b86768f83baa6dbb4f81fc92f1113ee2241691211bcadb6b7d2e7635ec5c9ef06f136f2e647844d31b6895402bcf1802ba08412bcf553a92e188ead80c212197ad053e8c4fbed67039e683d177b48121d8b823015d6e5dc850b50aaa2e95ca2af5f6951153386e0f98621332b2495caf45abe8a0b49790ca1b1e05c3af55138612682efd4d28367cbfbd273c95292e92a2ff073946ca689a2e6b9709a4b08ce2782c57f871f3c13fea55add85a32441d22d8ce56de588eebf55e3132bcaa1d03a6ca48726de6e05047c9fabbb7f4aeef0501fbcaa839a73ea9ce6b3947511f1cb341db8af35b3c2ab89365e784b0f1485fc3be2f7723541cce55ae3ede78efb09ecca8918b0514c620ce56e41efa98e715f91d04e813f27b78bec10da61e9259b1ec7aca2c3fb062590dda5ca510c3800955988830c0173dc2f5d911bfcb0fc1d23881a21df03f5e0e3a8a040ac223fce20199e47a8a1d16d511fa3758a937984f47a111f7da8bd6d87ff484b3c0b98fc889bd75a3893d16e51dcf1d493c4fb99f194cd474a77a7fce541c99a3924a5be5bd78477082f40bf3771f76d8a150372fbcf64a8a19f990a3170ae5867c168f371a28df90e78e31420eba031aff880b46cc9c96021dca5d975baba412e396f74092e360d62f72fea547e7698e80442ee251246627e0a095b72c374ce5da4cb877e49f8acefa442987028f8a3aed5a41e95752b4c70af8fcd995310b3c3f68104fd05bd3546e78624d61f1fd46dbd26add39207501fd4f16a07d57eeded8d5ed91b50f25ce8993c982ecde5cffa09f94c70843f3c61749920ded84e0de345361605e8a2b9054359d76901c8f6ea0df09f81f91b7e6e88d0cc5e2e07facebb57fce27f433406cdcc03397da92ecd6b26a0a79c760e913a562520fb3b0e9b944e027d854891726176f860474a105eb44a49f86d284c24f7d4b20e5a8e265f23f01c6ab166a7cdf4ef2f65e95860654f083b4b72f00b593bc74e269ef49c33bc3fe7fdaab84744c69b247a9f77dac681ceff5cba1f225d5bc62f582a6fd64b5d942c8fd03883e7d0618cbb87647eb4c037964e1cef35ab42423ab375005678b80cd9d5c6f9b574d32489339583dba81be4f637d898dcd350ddfd38bf18f67636dcc398a0e16c984d7c7f5dee6e2bb8b7b63d9c581e47da06c5c6734de858a12d3c2ab09b38163d3be1741195c4eb07152f38e5fc65372807d5517ff2f8ebb047a5868b773983f78d663140c6532dd09078ae54b1c9fedf3e2a2ba9857010ee6c4ee09533a0df2418e523b3e5f9e297e22a0de3ed398e8302b95507689833476679577b479266a5b1b65d95227f36451dad497eeec9838778c59c7ebdc3989c20e66f423fde732ad5a9a85cbd71e9418903787d09db4526b4d6f987ace4874309b9a52d635bdccbd2734dc73a4cfbf373d601a7e6e7c8eb075b6438bb329528426a63b0392896c064b86de68a004842a1380feb4710a5e6ffe7df13e5540d0d1b4f77d007d8d76427c985fd0a6a31b92a400bd24a6edd69c507687448bdc7c2e556753a2f62fce9e7d7525a9f90b5f5572b62f5c38aa954168cadd775782379cfe2846016d437b2a2df58ed8884f90e58fd85109ae8a39ac4802a9ba0a32f707d39e843e419d097e1a24acb05a5ea9305a839a659be5475d58bb89548c5348204ff8aad84138824329605e4363b88b770903057a1231a219223616dc041f1dc279234c3432859e2e94a6b0686ecfa35f02e6ec771f3d60e05b0b619e8be9adff848879e08fc5464ff57b9a12376398b30d7c94120217e9ebdad023911ab7f54fb6c287fda2149ea7f96123b0c92022367baee0089dbd99bf572d79278c235fe51baf6b13f60aa64204d5ed57ac82d5b7251a5d2b93a65fd323f8878e20718489a72ae482356d5a78b11a2c3bfad66a6f5e3fb617256ad0cf989b01144a052d8d5d28295bbe5a61cfcc16475394044678d006488a36110eae7490b1e58fee997098b63e65eb917d7236e77f909d0fa47771d1d57c68d931e26760f010582dd4d8493b3181eee36679c2aa3d9af2dccfa8fbc2fd5b380de2bf27e38f1b6ef8537c940cac5cdb5517142f86ab9c6dd26cfb9ad7bbc96b8a750c9f1713d623599642db6d79b2f0c62ce69ff042d73afca214504e2a9da7d30ff17bc5abffaa6895bdb34e3af9aced0af52c2c3944ec2e9aa3619219f662bb3c7b692cc1bd39934807897fb32d8ff08d0aa76aa928c6a094398db968c97d6faea7c4eaf8a2e714199cef772e08e7481fc1033dcacbb8e847aac9c68b021e2ec6b644e3b458e2a456694b6cc7ffffbda8798cba6a9d3f0f9f75e5873eaac006d7f7e7f0619ab44293981dbdf5bac1793561274e435c793f67ae755a7c5256166035e3149da3f03a9bd0504\", \"mac\":\"50bc2e8d29368875bf214eea319f8aa5336337a95d2685359438d84a61bd17a3\"}";

		FShareMetaData shareMetaData;

		if (!FJsonObjectConverter::JsonObjectStringToUStruct(jsonOutput, &shareMetaData, 0, 0)) {
			UE_LOG(LogTemp, Error, TEXT("failed to parse json"));
			return;
		}

		FString encryptedData = crypto->encrypt("", sessionId, shareMetaData.ephemPublicKey, shareMetaData.iv);
		shareMetaData.ciphertext = encryptedData;


		TSharedPtr<FJsonObject> jsonObject = MakeShareable(new FJsonObject);
		FJsonObjectConverter::UStructToJsonObject(FShareMetaData::StaticStruct(), &shareMetaData, jsonObject.ToSharedRef(), 0, 0);

		FString jsonString;
		TSharedRef<TJsonWriter<TCHAR>> jsonWriter = TJsonWriterFactory<>::Create(&jsonString);
		FJsonSerializer::Serialize(jsonObject.ToSharedRef(), jsonWriter);

		FLogoutApiRequest request;
		request.data = jsonString;
		request.key = pubKey;
		request.signature = crypto->generateECDSASignature(sessionId, jsonString);
		request.timeout = 1;

		web3AuthApi->Logout(request, [](FString response)
			{
				UE_LOG(LogTemp, Log, TEXT("Response: %s"), *response);
				AWeb3Auth::keyStoreUtils->Remove("sessionId");
				AsyncTask(ENamedThreads::GameThread, [=]() {
					AWeb3Auth::logoutEvent.ExecuteIfBound();
				});
			});

	}
}


void AWeb3Auth::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

AWeb3Auth::~AWeb3Auth() {
}

