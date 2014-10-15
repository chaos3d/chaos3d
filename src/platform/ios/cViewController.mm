//
//  cViewController.m
//  platform-ios
//
//  Created by Hao Wu on 3/27/13.
//  Copyright (c) 2013 cHaos3D. All rights reserved.
//

#import "cViewController.h"
#import <QuartzCore/QuartzCore.h>

@implementation cViewController

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
	self.view = [[UIView alloc] initWithFrame: [UIScreen mainScreen].bounds];
	self.view.userInteractionEnabled = YES;
	self.view.multipleTouchEnabled = YES;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
	// Do any additional setup after loading the view.
    
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (void)viewDidAppear:(BOOL)animated{
    
}

- (void)viewDidDisappear:(BOOL)animated{
    
}

@end
