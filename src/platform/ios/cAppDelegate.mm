//
//  cAppDelegate.m
//  platform-ios
//
//  Created by Hao Wu on 4/11/13.
//  Copyright (c) 2013 cHaos3D. All rights reserved.
//

#import "app/screen.h"
#import "app/application.h"
#import "asset/asset_locator.h"
#import "common/timer.h"
#import "go/component_manager.h"
#import "go/game_object.h"
#import "platform/ios/cAppDelegate.h"
#import "platform/ios/cViewController.h"
#import "re/render_device.h"
#import "re/render_target.h"

#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>

@implementation cAppDelegate

@synthesize window, controller, displayLink;
@synthesize mainWindow = _main_window, defaultDevice = _default_device;
@synthesize mainContext = _main_context;

- (void)dealloc
{
    self.controller = nil;
    self.window = nil;
    [super dealloc];
}

- (void) frameLoop: (CADisplayLink*) _{
    component_manager::managers().update(&game_object::root());
    
    application::instance().get_screen().loop();
    
    global_timer_base::instance().update();
    //_main_window->do_render(_main_context); // clear batch
    
}

- (void) startLoop {
    if (self.displayLink != nil){
        [displayLink invalidate];
    }
    
    self.displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(frameLoop:)];
    displayLink.frameInterval = 1;
    
    // run once before the view shows up
    [self frameLoop: self.displayLink];
    
    [displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
}

- (void) stopLoop {
    [displayLink invalidate];
    self.displayLink = nil;
}

- (int) renderType {
    return render_device::OpenGLES20;
}

- (void) initLocators {
    locator_mgr::instance().
    add(locator::dir_locator::app_dir())->
    add(locator::dir_locator::home_dir());
}

- (BOOL)application:(UIApplication *)application_ didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    application& app = application::instance();
    app.on_initialize();
    CGRect rt = [[UIScreen mainScreen] bounds];
    self.window = [[[UIWindow alloc] initWithFrame:rt] autorelease];
    
    // Override point for customization after application launch.
    self.window.backgroundColor = [UIColor blackColor];
    self.controller = [[[cViewController alloc] init] autorelease];

    self.window.rootViewController = controller;
    [self.window makeKeyAndVisible];
    
    // init the locators
    [self initLocators];
    
    // create the device
    _default_device = render_device::get_device([self renderType]);
    _default_device->init_context();

    _main_window = _default_device->create_window(render_target::target_size_t(rt.size.width, rt.size.height));
    
    // init the context for the current thread
    _main_context = _default_device->create_context(_main_window);
    _main_context->set_current();
    
    [self.controller.view addSubview: (UIView*)_main_window->native_handle()];
    
    application::instance().get_screen().on_start();
    
    [self startLoop];
    
    app.on_launch();
    
    return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application
{
    // Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
    // Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
    
    [self stopLoop];
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
    // Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later.
    // If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
    // Called as part of the transition from the background to the inactive state; here you can undo many of the changes made on entering the background.
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
    // Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
    
    [self startLoop];
}

- (void)applicationWillTerminate:(UIApplication *)application
{
    // Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
}

@end
