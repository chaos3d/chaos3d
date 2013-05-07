//
//  cViewController.m
//  platform-ios
//
//  Created by Hao Wu on 3/27/13.
//  Copyright (c) 2013 cHaos3D. All rights reserved.
//

#import "cViewController.h"
#import <QuartzCore/QuartzCore.h>

#include "framework/EngineLoop.h"
#include "re/frontend/iOSRenderDriver.h"
#include "boost/bind.hpp"

@interface cViewController (){
}
@property (retain) CADisplayLink* displayLink;
@end

@implementation cViewController

@synthesize engineLoop, displayLink, renderDriver;

- (void) dealloc{
    [super dealloc];
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

- (RenderDriver*) getRenderDriver{
    if(renderDriver == NULL){
        
    }
    
    return renderDriver;
}

- (void) pauseLoop: (BOOL) paused{
    if(self.displayLink != NULL){
        [displayLink invalidate];
    }

    if(!pause){
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
    if(engineLoop != NULL){
        engineLoop->startUp();
    }

    assert(self.displayLink == NULL);
    [self pauseLoop:FALSE];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (void) frameLoop: (id) _ {
    assert(engineLoop != NULL);
    engineLoop->loop();
}

- (void)viewDidAppear:(BOOL)animated{
    
}

- (void)viewDidDisappear:(BOOL)animated{
    
}

@end
