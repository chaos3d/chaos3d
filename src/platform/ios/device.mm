#include "platform/device.h"
#include <TargetConditionals.h>

#if TARGET_OS_IPHONE
#import <UIKit/UIKit.h>
#endif

device::vector2i device::screen_size() {
#if TARGET_OS_IPHONE
    CGRect bounds = [UIScreen mainScreen].bounds;
    return {bounds.size.width, bounds.size.height};
#else
    return {1024, 768};
#endif
}
