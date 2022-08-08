# Web3Auth SDK for Unreal Engine 5.0 

[1] Installation (Win 10/11) 

Prerequisites: 
- Unreal Engine 5.0 
- Visual Studio 
- [Windows destop runtime](https://dotnet.microsoft.com/en-us/download/dotnet/3.1/runtime)

When running the project for the 1st time, right click the project file and click "Generate Visual Studio project files" (in Win 11 this is under "Show more options") 
![Screenshot 2022-08-04 at 1 25 28 PM](https://user-images.githubusercontent.com/106652994/182769715-6f6bf779-0d9a-4990-9dc8-d5a4f79be899.png)

After opening the project click "Content Drawer" at the bottom left, nav to "Web3AuthSDK Content" and open the Sample level. 
![Screenshot 2022-08-04 at 1 26 31 PM](https://user-images.githubusercontent.com/106652994/182769731-7b9c66ff-9de6-459c-9c3c-1ee854bcd97f.png)

***

[2] Building for Windows 

To build the project for Windows, near the Play icon at the top, click Platforms => Windows => Package Project 
![Screenshot 2022-08-04 at 1 31 04 PM](https://user-images.githubusercontent.com/106652994/182770019-12b511ea-797b-4395-8966-21f3a36bcc0b.png)

This is how it looks like in the Windows build; it opens the default browser for the user to log in to the chosen provider (eg. Google) 
![Screenshot 2022-08-04 at 1 30 19 PM](https://user-images.githubusercontent.com/106652994/182770031-54a2ddf3-73c1-4670-a4a7-658ac2200cb8.png)

Support for other platforms coming soon... 

