//
//  MWMAlertViewController.h
//  Maps
//
//  Created by v.mikhaylenko on 05.03.15.
//  Copyright (c) 2015 MapsWithMe. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "MWMAlert.h"

#include "routing/router.hpp"
#include "storage/storage.hpp"

typedef void (^CloseAlertCompletion)();

@interface MWMAlertViewController : UIViewController

@property (nonnull, weak, nonatomic, readonly) UIViewController * ownerViewController;

- (nonnull instancetype)initWithViewController:(nonnull UIViewController *)viewController;
- (void)presentAlert:(routing::IRouter::ResultCode)type;
- (void)presentDownloaderAlertWithCountries:(vector<storage::TIndex> const &)countries
                                     routes:(vector<storage::TIndex> const &)routes
                                       code:(routing::IRouter::ResultCode)code;
- (void)presentRateAlert;
- (void)presentFacebookAlert;
- (void)presentFeedbackAlertWithStarsCount:(NSUInteger)starsCount;
- (void)presentRoutingDisclaimerAlert;
- (void)presentDisabledLocationAlert;
- (void)presentLocationAlert;
- (void)presentLocationServiceNotSupportedAlert;
- (void)presentNoConnectionAlert;
- (void)presentnoWiFiAlertWithName:(nonnull NSString *)name downloadBlock:(nullable RightButtonAction)block;
- (void)presentPedestrianToastAlert:(BOOL)isFirstLaunch;
- (void)closeAlertWithCompletion:(nullable CloseAlertCompletion)completion;

- (nonnull instancetype)init __attribute__((unavailable("call -initWithViewController: instead!")));
+ (nonnull instancetype)new __attribute__((unavailable("call -initWithViewController: instead!")));
- (nonnull instancetype)initWithCoder:(nonnull NSCoder *)aDecoder __attribute__((unavailable("call -initWithViewController: instead!")));
- (nonnull instancetype)initWithNibName:(nullable NSString *)nibNameOrNil bundle:(nullable NSBundle *)nibBundleOrNil __attribute__((unavailable("call -initWithViewController: instead!")));
@end
