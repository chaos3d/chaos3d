//
//  cViewController.m
//  platform-ios
//
//  Created by Hao Wu on 3/27/13.
//  Copyright (c) 2013 cHaos3D. All rights reserved.
//

#import "cViewController.h"
#include "lyield/lyield.h"
#include "lua/lua.h"

@interface cViewController ()

@end

@implementation cViewController

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
    
    // TODO: open lua state
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (void) frameLoop: (id) _ {
    lua_cpcall(NULL, lyield_run, NULL);
}

@end
