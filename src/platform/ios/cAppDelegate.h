//
//  cAppDelegate.h
//  platform-ios
//
//  Created by Hao Wu on 4/11/13.
//  Copyright (c) 2013 cHaos3D. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>

class screen;
class render_window;
@class cViewController;

/**
 * iOS delegate wrapper
 *
 * common application skeleton functions to easily
 * set up the app
 */
@interface cAppDelegate : UIResponder <UIApplicationDelegate> {
    screen* _main_screen;
    render_window* _main_window;
}

@property (nonatomic, retain) UIWindow* window;
@property (nonatomic, retain) cViewController* controller;
@property (retain) CADisplayLink* displayLink;
@property (nonatomic, assign) render_window* mainWindow;

- (screen*) createScreen;

- (void) startLoop;
- (void) stopLoop;
@end
