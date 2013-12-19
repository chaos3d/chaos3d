#include "platform/device.h"

#import <UIKit/UIKit.h>
#import "platform/ios/cAppDelegate.h"

device::vector2i device::screen_size() {
    CGRect bounds = [UIScreen mainScreen].bounds;
    return {bounds.size.width, bounds.size.height};
}

render_window* device::main_window() {
    return ((cAppDelegate*)[UIApplication sharedApplication].delegate).mainWindow;
}
