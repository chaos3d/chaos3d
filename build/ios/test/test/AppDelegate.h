//
//  AppDelegate.h
//  test
//
//  Created by Hao Wu on 1/12/13.
//  Copyright (c) 2013 cHaos3D. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>
#import "ScriptThread.h"

@interface AppDelegate : UIResponder <UIApplicationDelegate> {
    ScriptThread* mainThread;
    CADisplayLink *displayLink;
}

@property (strong, nonatomic) UIWindow *window;

@end
