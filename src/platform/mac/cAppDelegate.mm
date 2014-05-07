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

#include "app/application.h"
#include "app/screen.h"

@implementation cAppDelegate

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

- (void) applicationDidFinishLaunching: (NSNotification *)aNotification {
    application& app = application::instance();
    app.on_initialize();
    
    // init the locators
    //[self initLocators];
    
    // TODO: move this to application
    application::instance().get_screen().on_start();
    
    app.on_launch();

    [self startLoop];
}

@end
