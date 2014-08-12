//
//  AppDelegate.h
//  platform-mac
//
//  Created by Hao Wu on 4/8/14.
//  Copyright (c) 2014 cHaos3D. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import <AppKit/AppKit.h>

@interface cAppDelegate : NSObject <NSApplicationDelegate> {
    NSTimer* timer;
}

@property (nonatomic, retain) NSWindow* window;
@property (nonatomic, readonly) NSString* title;

@end
