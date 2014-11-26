//
//  cAppDelegate.h
//  platform-ios
//
//  Created by Hao Wu on 4/11/13.
//  Copyright (c) 2013 cHaos3D. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>

struct lua_State;
class data_stream;
@class cViewController;

/**
 * iOS delegate wrapper
 *
 * common application skeleton functions to easily
 * set up the app
 */
@interface cAppDelegate : UIResponder <UIApplicationDelegate> {
    lua_State* _L;
}

@property (nonatomic, retain) IBOutlet UIWindow* window;
@property (nonatomic, retain) cViewController* controller;
@property (retain) CADisplayLink* displayLink;

/// start the script vm from the given source
- (void)start:(data_stream*) ds;

/// start the timer loop
- (void)startLoop;

/// stop the timer
- (void)stopLoop;

@end
