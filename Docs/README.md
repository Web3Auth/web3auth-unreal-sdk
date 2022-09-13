# Web3Auth Unreal SDK

This guide will cover the basics of how to set up your Web3Auth SDK and Auth0 account for the integration and provide you with the links on how to develop a basic web application on the blockchain of your choice. Before starting, you can try the integration in our example here.

## Supported Platforms

*   Standalone (Windows, Mac, Linux)
*   Android ( API 21+ )
*   iOS ( SDK 14+ )

## Prerequisites

*   Basic Knowledge of Unreal Editor and Blueprints
*   Create a Web3Auth account on the Web3Auth Dashboard
*   Unreal Editor 5 or higher With Android/iOS build support

## Setup

### Setup your Web3Auth Dashboard

*   Create a Project from the Plug and Play Section of the Web3Auth Developer Dashboard [![](https://camo.githubusercontent.com/971624a070b3ed1c191d051ca349cbca15d7dc52ad0a4a3ec812b0ab08636ea9/68747470733a2f2f77656233617574682e696f2f646f63732f6173736574732f696d616765732f61757468302d77336164617368626f6172642d30312d37346637663130653531373966373765613064653563313632396338656532632e706e67)](https://camo.githubusercontent.com/971624a070b3ed1c191d051ca349cbca15d7dc52ad0a4a3ec812b0ab08636ea9/68747470733a2f2f77656233617574682e696f2f646f63732f6173736574732f696d616765732f61757468302d77336164617368626f6172642d30312d37346637663130653531373966373765613064653563313632396338656532632e706e67)
*   Enter your desired Project name
*   Select the blockchain(s) you'll be building this project on. For interoperability with Torus Wallet, you've an option of allowing the user's private key to be used in other applications using Torus Wallet. We currently have this option with EVM and Solana and Casper blockchains.
*   Finally, once you create the project, you've the option to whitelist your URLs for the project. Please whitelist the domains where your project will be hosted. 
*   You will require clientId of the Plug and Play Projects

### Using the Web3Auth SDK

To use the Web3Auth SDK, you need to add the dependency of the respective platform SDK of Web3Auth to your project. To know more about the available SDKs, please have a look at this [documentation page](https://web3auth.io/docs/developing-with-web3auth/understand-sdk).

For this guide here, we will be focused on the Web3Auth Unreal SDK and using the OpenLogin Provider alongside it.

### Installation

To install or integrate Web3Auth Unreal SDK, clone/download the repository to your computer locally, then follow steps below

*    Copy `Plugins/Web3AuthSDK` folder to  `<Your-Project-Path>/Plugins` directory. 
*    Enable Web3AuthSDK plugin from Unreal Editor 5 settings `Edit →  Plugins → All Plugins → Web3AuthSDK` 

![](/Docs/Images/5.png)

*   Restart the Editor
*   After restart, add Web3Auth actor class to the game level.

![](/Docs/Images/4.png)

### Blueprints

The following is the graph to set Web3Auth actor configuration. The graph currently uses string values to add parameters to blueprint blocks, but you can connect the values coming from different UI components. See more in [Try Sample Level](#) section.

![](/Docs/Images/8.png)

### Android

Setup android sdk and android-ndk for unreal editor for more details please see unreal documentation https://docs.unrealengine.com/5.0/en-US/how-to-set-up-android-sdk-and-ndk-for-your-unreal-engine-development-environment/


* To add redirect URI into android app, open file `<Your-Project-Path>/Plugins/Web3AuthSDK/Source/Web3AuthSDK_Android.xml`

*  Find the `<androidManifestUpdates>` tag and inside there will a `<data>` tag element. Replace the exisitng redirect URI with your own which you have registered on your Web3Auth Dashboard

![](/Docs/Images/3.png)

### IOS 

First setup development envirnment according to unreal official documentaion 
https://docs.unrealengine.com/5.0/en-US/setting-up-an-unreal-engine-project-for-ios/

To add redirect URI in iOS configuration, you just have to add the redirect URI schema. i-e if `torusapp://com.unreal.Web3Auth` is your registered redirect URI then the schema is `torusapp`.

*   Update the following xml object and replace the {schema} with your own redirectURI schema.

```plaintext
<key>CFBundleURLTypes</key>
<array>
	<dict>
		<key>CFBundleURLName</key>
		<string>{schema}</string>
		<key>CFBundleURLSchemes</key>
		<array>
			<string>{schema}</string>
		</array>
	</dict>
</array>
```

*   Paste the above string with updated schema into `Edit → Project Settings → IOS → Extra Plist Data`
![](/Docs/Images/2.png)

## Try Sample Level

 * Open 'Sample' level from plugin directory `Plugins →  Web3AuthSDK -→ Web3AuthSDK Content → Sample`
![](/Docs/Images/1.png)

 * Run Sample level from editor's toolbar menu
![](/Docs/Images/6.png)

 * To see the blueprint graph Open `AuthInterface` widget from `Web3Auth Content` directory and Switch to graph mode.

### Build
* If you are building sample scene for iOS and Android configure redirect_uri in app configuration as instructed in steps above. 
* Select item from `Select Platform` menu from toolbar and select the respective device and click on `Package Content`


![](/Docs/Images/7.png)




