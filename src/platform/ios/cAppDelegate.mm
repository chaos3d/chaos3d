//
//  cAppDelegate.m
//  platform-ios
//
//  Created by Hao Wu on 4/11/13.
//  Copyright (c) 2013 cHaos3D. All rights reserved.
//

#import "common/log.h"
#import "common/timer.h"
#import "go/game_object.h"
#import "go/component_manager.h"

#import "platform/lua_module.h"
#import "platform/ios/cAppDelegate.h"
#import "platform/ios/cViewController.h"

#import "script/state.h"
#import "asset/asset_locator.h"
#include <liblua/lua/lua.hpp>

#if 0
// traceback from lua
static int traceback (lua_State *L) {
    if (!lua_isstring(L, 1))  /* 'message' not a string? */
        return 1;  /* keep it intact */
    lua_getfield(L, LUA_GLOBALSINDEX, "debug");
    if (!lua_istable(L, -1)) {
        lua_pop(L, 1);
        return 1;
    }
    lua_getfield(L, -1, "traceback");
    if (!lua_isfunction(L, -1)) {
        lua_pop(L, 2);
        return 1;
    }
    lua_pushvalue(L, 1);  /* pass error message */
    lua_pushinteger(L, 2);  /* skip this function and traceback */
    lua_call(L, 2, 1);  /* call debug.traceback */
    return 1;
}
#endif

static std::unique_ptr<script::coroutine> _main_coroutine(nullptr);

@interface cAppDelegate() {
    lua_State* _L;
}

@end

@implementation cAppDelegate

@synthesize window, controller, displayLink;

- (void)dealloc {
    lua_close(_L);
    
    self.controller = nil;
    self.window = nil;
}

- (void)frameLoop:(CADisplayLink*) _{
    if (_main_coroutine && _main_coroutine->is_yielded()) {
        _main_coroutine->resume(); // FIXME: bool and timer*
    }
    
    component_manager::managers().update(&game_object::root());
    
    global_timer_base::instance().update();
}

- (void)startLoop {
    if (self.displayLink != nil){
        [displayLink invalidate];
    }
    
    self.displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(frameLoop:)];
    displayLink.frameInterval = 1;
    
    // run once before the view shows up
    [self frameLoop: self.displayLink];
    
    [displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
}

- (void)stopLoop {
    [displayLink invalidate];
    self.displayLink = nil;
}

- (void)start:(NSString*) filePath {
    // add default locators, this way, locator_mgr can be used
    // without calling initiliazation
    locator_mgr::instance()
    .add(locator::dir_locator::app_dir(0, "/res"))
    ->add(locator::dir_locator::app_dir())
    ->add(locator::dir_locator::home_dir())
    ;

    _L = luaL_newstate();
    
    luaL_openlibs(_L);
    lua_cpcall(_L, luaopen_chaos3d, NULL); // load the lib
    
    script::state* st = nullptr;
    lua_getallocf(_L, (void**)&st); // state is being saved by opening the lib

    _main_coroutine.reset(new script::coroutine(st->load(locator_mgr::instance()
                                                         .from([filePath UTF8String]).get())
                                                ));
    _main_coroutine->resume();  // returns bool and timer*
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
