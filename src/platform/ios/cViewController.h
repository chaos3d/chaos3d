//
//  cViewController.h
//  platform-ios
//
//  Created by Hao Wu on 3/27/13.
//  Copyright (c) 2013 cHaos3D. All rights reserved.
//

#import <UIKit/UIKit.h>

@class cViewController;

@protocol cControllerDelegate <NSObject>

@required
- (void) update: (cViewController*) controller;

@optional

@end

/**
 * This is a run loop wrapper for iOS
 *
 * This will start/stop the looping and be
 * a context of render views, UI layout etc.
 * cAppDelegate or client's should create
 * and maintain this
 *
 * this also has timer controller which the loop can run
 * frame by frame, and simulate and other debug facilities
 */
@interface cViewController : UIViewController /// use standard view controller
@property (nonatomic, assign) id<cControllerDelegate> delegate;

+ (cViewController*) controllerWithDelegate: (id<cControllerDelegate>) delegate_;

- (void) startLoop;
- (void) stopLoop;
- (void) pauseLoop: (BOOL) paused;
- (void) update;
@end
