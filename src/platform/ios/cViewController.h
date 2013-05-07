//
//  cViewController.h
//  platform-ios
//
//  Created by Hao Wu on 3/27/13.
//  Copyright (c) 2013 cHaos3D. All rights reserved.
//

#import <UIKit/UIKit.h>

class EngineLoop;
class RenderDriver;

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
{
}

@property (assign) EngineLoop* engineLoop;
@property (assign, readonly) RenderDriver* renderDriver; // render driver works with cViewController(owner)

- (void) startLoop;
- (void) stopLoop;
- (void) pauseLoop: (BOOL) paused;
@end
