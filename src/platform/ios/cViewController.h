//
//  cViewController.h
//  platform-ios
//
//  Created by Hao Wu on 3/27/13.
//  Copyright (c) 2013 cHaos3D. All rights reserved.
//

#import <UIKit/UIKit.h>

#if __IPHONE_OS_VERSION_MIN_REQUIRED >= __IPHONE_5_0
#import <GLKit/GLKit.h>
#endif

@interface cViewController :
#if __IPHONE_OS_VERSION_MIN_REQUIRED >= __IPHONE_5_0
    GLKViewController
#else
    // TODO: support <5.0 later
    UIViewController
#endif
{
}

@property (copy) NSString* boostrap;

@end
