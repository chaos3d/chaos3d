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
#include "io/IOManager.h"
#include "boost/bind.hpp"

@interface cViewController (){
    EngineLoop* _engineLoop;
    CADisplayLink* _displayLink;
}

@end

void testCallback(cViewController* view){
    NSLog(@"callback %@", view);
}

@implementation cViewController

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

// TODO: replace with nib
// Implement loadView to create a view hierarchy programmatically, without using a nib.
- (void) loadView {
	// create a full screen view for default
    // TODO: support <5.0
	self.view = [[[UIView alloc] initWithFrame: [UIScreen mainScreen].bounds] autorelease];
	self.view.userInteractionEnabled = YES;
	self.view.multipleTouchEnabled = YES;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
	// Do any additional setup after loading the view.
    
    // TODO: to put in a better place
    IOManager* io = new IOManager();
    io->addDirLocator([[[[NSBundle mainBundle] bundlePath] stringByAppendingString: @"/"] UTF8String]);
    
    // TODO: open lua state
    if(_engineLoop == NULL){
        EngineLoop::Config config;
        config.bootstrap = std::string([self.boostrap UTF8String]);
        config.startUp = boost::bind(testCallback, self);
        _engineLoop = new EngineLoop(config);
        _engineLoop->startUp();
    }
    
    _displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(frameLoop:)];
    _displayLink.frameInterval = 1;
    [_displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
    
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (void) frameLoop: (id) _ {
    _engineLoop->loop();
}

@end
