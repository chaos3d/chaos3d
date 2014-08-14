#include "platform/device.h"
#include <TargetConditionals.h>

#if TARGET_OS_IPHONE
#import <UIKit/UIKit.h>
#else
#import <AppKit/AppKit.h>
#endif

device::vector2i device::screen_size() {
#if TARGET_OS_IPHONE
    CGRect bounds = [UIScreen mainScreen].bounds;
    return {bounds.size.width, bounds.size.height};
#else
    return {1024, 768};
#endif
}

float device::best_pixel_ratio() {
#if TARGET_OS_IPHONE
    return [[UIScreen mainScreen] scale];
#else
    return [[NSScreen mainScreen] backingScaleFactor];
#endif
}