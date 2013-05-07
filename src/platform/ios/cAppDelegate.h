//
//  cAppDelegate.h
//  platform-ios
//
//  Created by Hao Wu on 4/11/13.
//  Copyright (c) 2013 cHaos3D. All rights reserved.
//

#import <UIKit/UIKit.h>

@class cViewController;

@interface cAppDelegate : UIResponder <UIApplicationDelegate>

@property (nonatomic, retain) UIWindow* window;
@property (nonatomic, retain) cViewController* controller;
@end
