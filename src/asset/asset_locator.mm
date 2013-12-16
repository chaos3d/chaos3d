#include "asset/asset_locator.h"
#import <Foundation/Foundation.h>

namespace locator {
    dir_locator* dir_locator::app_dir() {
        NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
        
        dir_locator* locator = new dir_locator([[[NSBundle mainBundle] bundlePath] UTF8String]);

        [pool release];
        return locator;
    }

    dir_locator* dir_locator::home_dir() {
        NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
        NSArray *paths = NSSearchPathForDirectoriesInDomains(
                                                             NSDocumentDirectory, NSUserDomainMask, YES);
        NSString *documentsDirectory = [paths objectAtIndex:0];
        dir_locator* locator = new dir_locator([documentsDirectory UTF8String]);
        
        [pool release];

        return locator;
    }
}