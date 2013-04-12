#include "platform/lplatform.h"

#include "lua/lua.h"
#include "lua/lauxlib.h"
#include "lua/lualib.h"

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

extern int luaopen_lplatform(struct lua_State* L){
    @autoreleasepool {
        luaL_Reg entris[] = {
            //{"resume", coresume},
            {NULL, NULL}
        };

        NSArray *paths = NSSearchPathForDirectoriesInDomains(
                                                             NSDocumentDirectory, NSUserDomainMask, YES);
        // add bundle path and document path to package.path
        // note: doc path is higher than bundle path so that files can be overridden.
        lua_getglobal(L, "package");
        lua_pushstring(L, [[[paths objectAtIndex:0] stringByAppendingString: @"/?.lua;"] UTF8String]);
        lua_pushstring(L, [[[[NSBundle mainBundle] bundlePath] stringByAppendingString:@"/?.lua;"] UTF8String]);
        lua_getfield(L, -3, "path");
        lua_concat(L, 3);
        lua_setfield(L, -2, "path");
        
        // add excutable into cpath so it could load symbols from itself
        lua_pushstring(L, [[[[NSBundle mainBundle] executablePath] stringByAppendingString:@";"] UTF8String]);
        lua_getfield(L, -2, "cpath");
        lua_concat(L, 2);
        lua_setfield(L, -2, "cpath");
        lua_pop(L, 1);

        luaL_register(L, "platform", entris);
        
        lua_getglobal(L, "platform");
        lua_pushstring(L,
                       [UIDevice currentDevice].userInterfaceIdiom == UIUserInterfaceIdiomPad ? "ipad" : "iphone"
                       );
        lua_setfield(L, -2, "device");

        lua_pushstring(L, [[UIDevice currentDevice].model UTF8String]);
        lua_setfield(L, -2, "model");
        lua_pushstring(L, [[[UIDevice currentDevice].systemName stringByAppendingString:
                            [UIDevice currentDevice].systemVersion] UTF8String]);
        lua_setfield(L, -2, "system");
    }
    
    return 1;
}