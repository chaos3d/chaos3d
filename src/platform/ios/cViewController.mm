//
//  cViewController.m
//  platform-ios
//
//  Created by Hao Wu on 3/27/13.
//  Copyright (c) 2013 cHaos3D. All rights reserved.
//

#import "cViewController.h"
#import <QuartzCore/QuartzCore.h>

//#include "framework/EngineLoop.h"
//#include "re/frontend/iOSRenderDriver.h"
//#include "boost/bind.hpp"

@interface cViewController (){
}
@property (retain) CADisplayLink* displayLink;
@end

@implementation cViewController

@synthesize displayLink, delegate;

- (void) dealloc{
    self.displayLink = nil;
    [super dealloc];
}

- (void) update{
    
}

+ (cViewController*) controllerWithDelegate: (id<cControllerDelegate>) delegate_ {
    cViewController* controller = [[self alloc] init];
    controller.delegate = delegate_;
    return [controller autorelease];
}

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization
    }
    return self;
}

// Implement loadView to create a view hierarchy programmatically, without using a nib.
- (void) loadView {
	// create a full screen view by default
	self.view = [[[UIView alloc] initWithFrame: [UIScreen mainScreen].bounds] autorelease];
	self.view.userInteractionEnabled = YES;
	self.view.multipleTouchEnabled = YES;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
	// Do any additional setup after loading the view.
    
}

- (void) pauseLoop: (BOOL) paused{
    if(self.displayLink != nil){
        [displayLink invalidate];
    }

    if(!paused){
        self.displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(frameLoop:)];
        displayLink.frameInterval = 1;
        [displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
    }
}

- (void) stopLoop{
    // TODO
    [self.displayLink invalidate];
    self.displayLink = nil;
}

- (void) startLoop{

    assert(self.displayLink == NULL);
    [self pauseLoop:FALSE];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (void) frameLoop: (id) _ {
    if( delegate )
        [delegate update: self];
    else
        [self update];
}

- (void)viewDidAppear:(BOOL)animated{
    
}

- (void)viewDidDisappear:(BOOL)animated{
    
}

@end
