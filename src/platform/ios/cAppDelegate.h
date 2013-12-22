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
class render_device;
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
    render_device* _default_device;
}

@property (nonatomic, retain) UIWindow* window;
@property (nonatomic, retain) cViewController* controller;
@property (retain) CADisplayLink* displayLink;

@property (nonatomic, readonly, assign) render_window* mainWindow;
@property (nonatomic, readonly, assign) render_device* defaultDevice;

- (screen*) createScreen;

- (void) startLoop;
- (void) stopLoop;

- (int) renderType; // render_device::OpenGLES20
@end
