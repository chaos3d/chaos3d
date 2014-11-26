//
//  cAppDelegate.h
//  platform-ios
//
//  Created by Hao Wu on 4/11/13.
//  Copyright (c) 2013 cHaos3D. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>

@class cViewController;

/**
 * iOS delegate wrapper
 *
 * common application skeleton functions to easily
 * set up the app
 * 1. inherit from cAppDelegate
 * 2. [self start: path_to_boostrap.lua]
 * 3. [self startLoop]
 */
@interface cAppDelegate : UIResponder <UIApplicationDelegate>

@property (nonatomic, retain) IBOutlet UIWindow* window;
@property (nonatomic, retain) cViewController* controller;
@property (retain) CADisplayLink* displayLink;

/// start the script vm from the given source
- (void)start:(NSString*) filePath;

/// start the timer loop
- (void)startLoop;

/// stop the timer
- (void)stopLoop;

@end
