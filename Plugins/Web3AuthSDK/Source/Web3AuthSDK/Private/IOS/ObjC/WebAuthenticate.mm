#import "WebAuthenticate.h"
#import <AuthenticationServices/ASWebAuthenticationSession.h>
#include <Web3Auth.h>

API_AVAILABLE(ios(12.0))
ASWebAuthenticationSession *_authSession;

@implementation WebAuthenticate

+ (WebAuthenticate *)Singleton
{
    static WebAuthenticate *theWebAuthenticate;
    @synchronized(self) {
        if (!theWebAuthenticate)
            theWebAuthenticate = [[self alloc] init];
    }
    return theWebAuthenticate;
}

+ (void) launchUrl:(const char*)url
{
    NSURL * URL = [NSURL URLWithString: [[NSString alloc] initWithUTF8String:url]];
    NSString *bundleIdentifier = [[NSBundle mainBundle] bundleIdentifier];

    ASWebAuthenticationSession* authSession = [[ASWebAuthenticationSession alloc]initWithURL:URL callbackURLScheme:bundleIdentifier completionHandler:^(NSURL * _Nullable callbackURL, NSError * _Nullable error) {
        
        if(callbackURL) {
            NSLog(@"%@", callbackURL.absoluteString);
            AWeb3Auth::callBackFromWebAuthenticateIOS(callbackURL.absoluteString);
        } else {
            return;
        }
    }];
    
    _authSession = authSession;
    if (@available(iOS 13, *)) {
        _authSession.presentationContextProvider = (id) self;
    }

    [_authSession start];
}

#if __IPHONE_OS_VERSION_MAX_ALLOWED >= 130000
- (ASPresentationAnchor)presentationAnchorForWebAuthenticationSession:(ASWebAuthenticationSession *)session {
    for (UIScene *uiScene in UIApplication.sharedApplication.connectedScenes) {
        if (uiScene.activationState != UISceneActivationStateForegroundActive) {
            continue;
        }
        if ([uiScene isKindOfClass:UIWindowScene.class]) {
            UIWindowScene *uiWindowScene = (UIWindowScene *)uiScene;
            for (UIWindow *uiWindow in uiWindowScene.windows) {
                if(uiWindow.isKeyWindow){
                    return uiWindow;
                }
            }
        }
    }
    return nil;
}
#endif

@end
