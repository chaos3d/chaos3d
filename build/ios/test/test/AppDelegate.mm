//
//  AppDelegate.m
//  test
//
//  Created by Hao Wu on 1/12/13.
//  Copyright (c) 2013 cHaos3D. All rights reserved.
//

#import "AppDelegate.h"
#include "lua.h"
extern "C"{
#include "luasocket/luasocket.h"
#include "luasocket/src/luasocket.h"
#include "luasocket/src/mime.h"
}
@implementation AppDelegate

- (void)dealloc
{
    [_window release];
    [super dealloc];
}

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    self.window = [[[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]] autorelease];
    // Override point for customization after application launch.
    self.window.backgroundColor = [UIColor whiteColor];
    [self.window makeKeyAndVisible];

    displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(drawFrameLink:)];
    displayLink.frameInterval = 1;
    [displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
    
    mainThread = new ScriptThread();
    char const* str =
        "require 'io'"
        "print 'test'"
        "";
    str = [[NSString stringWithContentsOfFile: [[NSBundle mainBundle] pathForResource: @"test" ofType: @"lua"] encoding:NSUTF8StringEncoding error: NULL] UTF8String];
    ScriptState &ss = mainThread->getState();
	//lua_cpcall(ss, luaopen_socket_core, NULL);
    lua_cpcall(ss, luasocket, NULL);
    lua_cpcall(ss, luaopen_mime_core, NULL);
    
	luaL_findtable(ss, LUA_GLOBALSINDEX, "package.preload", 1);
	
	lua_pushcfunction(ss, luaopen_socket_core);
	lua_setfield(ss, -2, "socket.core");

    //lua_pushcfunction(ss, luaopen_mime_core);
	//lua_setfield(ss, -2, "mime.core");
    ss.pop(1);
    
    ScriptState::StringReader sr(str);
    ss.push_<ScriptCoroutine>();
    ScriptCoroutine sc = ss.get_<ScriptCoroutine>(-1);
    sc.getState().push_<ScriptState::SourceReader>(&sr);
    
    //ss.pop(1);
    mainThread->schedule(sc);

    lua_getglobal(ss, "package");
    lua_pushstring(ss, [[[[NSBundle mainBundle] bundlePath] stringByAppendingString:@"/?.lua;"] UTF8String]);
    lua_getfield(ss, -2, "path");
    lua_concat(ss, 2);
    lua_setfield(ss, -2, "path");
   
    /*
    lua_pushstring(ss, [[[[NSBundle mainBundle] bundlePath] stringByAppendingString:@"/test;"] UTF8String]);
    lua_getfield(ss, -2, "cpath");
    lua_concat(ss, 2);
    lua_setfield(ss, -2, "cpath");*/
    lua_pop(ss, 1);

    return YES;
}

- (void) drawFrameLink: (CADisplayLink*) _{
    mainThread->loop();
}

- (void)applicationWillResignActive:(UIApplication *)application
{
    // Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
    // Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
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
}

- (void)applicationWillTerminate:(UIApplication *)application
{
    // Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
}

@end
