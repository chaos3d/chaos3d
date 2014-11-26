//
//  AppDelegate.m
//  launcher
//
//  Created by Hao Wu on 11/24/14.
//  Copyright (c) 2014 chaos3d. All rights reserved.
//

#import "AppDelegate.h"

@interface AppDelegate ()

@end

@implementation AppDelegate

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    [self start: @"bootstrap.lua"];
    [self startLoop];
    
    // Override point for customization after application launch.
    return YES;
}

@end
