#include "asset/asset_locator.h"
#import <Foundation/Foundation.h>

namespace locator {
    dir_locator::ptr dir_locator::app_dir(int priority) {
        @autoreleasepool{
            dir_locator* locator = new dir_locator([[[NSBundle mainBundle] bundlePath] UTF8String],
                                                   priority);

            return ptr(locator);
        }
    }

    dir_locator::ptr dir_locator::home_dir(int priority) {
        @autoreleasepool{
            NSArray *paths = NSSearchPathForDirectoriesInDomains(
                                                                 NSDocumentDirectory, NSUserDomainMask, YES);
            NSString *documentsDirectory = [paths objectAtIndex:0];
            dir_locator* locator = new dir_locator([documentsDirectory UTF8String],
                                                   priority);
            return ptr(locator);
        }
    }
}