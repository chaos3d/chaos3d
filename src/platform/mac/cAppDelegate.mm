//
//  AppDelegate.m
//  flappy mac
//
//  Created by Hao Wu on 4/8/14.
//  Copyright (c) 2014 cHaos3D. All rights reserved.
//

#import "platform/mac/cAppDelegate.h"
#import "app/screen.h"
#import "app/application.h"
#import "common/timer.h"
#import "go/component_manager.h"
#import "go/game_object.h"
#import "re/render_device.h"
#import "re/render_target.h"

#include "app/application.h"
#include "app/screen.h"
#include "platform/device.h"

@implementation cAppDelegate

@dynamic title;

- (void) startLoop {
    timer = [NSTimer scheduledTimerWithTimeInterval:(1.0 / 60.0)
                                             target: self
                                           selector: @selector(frameLoop)
                                           userInfo: nil
                                            repeats: YES];
    
    // run for the initial screen update
    [self frameLoop];
}

- (void) frameLoop {
    component_manager::managers().update(&game_object::root());
    
    application::instance().get_screen().loop();
    
    global_timer_base::instance().update();
}

- (NSString*) title {
    return @"chaos3d";
}

- (void) applicationDidFinishLaunching: (NSNotification *)aNotification {
    application& app = application::instance();
    app.on_initialize();
    
    auto size = device::screen_size();
    NSRect contentRect = NSMakeRect(0.f, 0.f, size(0), size(1));
    NSUInteger style = NSMiniaturizableWindowMask | NSTitledWindowMask | NSClosableWindowMask;
    NSRect frame = [NSWindow frameRectForContentRect: contentRect styleMask: style];
    _window = [[NSWindow alloc] initWithContentRect: frame
                                          styleMask: style
                                            backing: NSBackingStoreBuffered
                                              defer: NO];
    
    // Now we have a view, add it to our window
    [_window setContentView: (__bridge NSView*)app.main_window()->native_handle()];
    [_window makeKeyAndOrderFront: nil];
    [_window setTitle: self.title];
    
    // init the locators
    //[self initLocators];
    
    // TODO: move this to application
    application::instance().get_screen().on_start();
    
    app.on_launch();

    assert(app.main_context() != nullptr);
    assert(app.main_device() != nullptr);
    assert(app.main_window() != nullptr);
    
    [self startLoop];
}

@end
