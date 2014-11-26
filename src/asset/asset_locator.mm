#include "asset/asset_locator.h"
#import <Foundation/Foundation.h>

namespace locator {
    dir_locator::ptr dir_locator::app_dir(int priority, char const* sub) {
        @autoreleasepool{
            asset_locator* locator = new dir_locator(std::string([[[NSBundle mainBundle] resourcePath] UTF8String]) + sub,
                                                     priority);

            return ptr(locator);
        }
    }

    dir_locator::ptr dir_locator::home_dir(int priority, char const* sub) {
        @autoreleasepool{
            NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory,
                                                                 NSUserDomainMask,
                                                                 YES);
            NSString *documentsDirectory = [paths objectAtIndex:0];
            asset_locator* locator = new dir_locator(std::string([documentsDirectory UTF8String]) + sub,
                                                     priority);
            
            return ptr(locator);
        }
    }
}