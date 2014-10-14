#include "cAppLauncher.h"

@implementation c3dApplication

+ (NSApplication*) initialize: (Class) delegateClass {
    // instantiate the application
    NSApplication *applicationObject = nil;
    NSDictionary *infoDictionary = [[NSBundle mainBundle] infoDictionary];
    Class principalClass = NSClassFromString(infoDictionary[@"NSPrincipalClass"]);
    if (principalClass != nil) {
        NSAssert([principalClass isSubclassOfClass: self],
                 @"Principal class must be inherited from c3dApplication.");
        applicationObject = [principalClass sharedApplication];
    } else {
        applicationObject = [c3dApplication sharedApplication];
    }
    
    // load main nib file if any
    NSString *mainNibName = infoDictionary[@"NSMainNibFile"];
    if (mainNibName != nil) {
        NSNib *mainNib = [[NSNib alloc] initWithNibNamed: mainNibName bundle: [NSBundle mainBundle]];
        [mainNib instantiateWithOwner: applicationObject topLevelObjects: nil];
    }
    
    if (delegateClass != nil) {
        static id delegate = [delegateClass new]; // hold strong ref to the delegate
        applicationObject.delegate = delegate;
    }
    
    [applicationObject setActivationPolicy: NSApplicationActivationPolicyRegular];
    [applicationObject finishLaunching];

    return applicationObject;
}

+ (BOOL) runLoopOnce {
    @autoreleasepool {
        NSApplication* app = [NSApplication sharedApplication];
        for (;;) {
            NSEvent *event = [app nextEventMatchingMask: NSAnyEventMask
                                              untilDate: [NSDate distantPast]
                                                 inMode: NSDefaultRunLoopMode
                                                dequeue: YES];
            
            if (event == nil) {
                break;
            }
            
            [app sendEvent:event];
        }
        
        return TRUE;
    }
}

// From http://cocoadev.com/GameKeyboardHandlingAlmost
// This works around an AppKit bug, where key up events while holding
// down the command key don't get sent to the key window.
- (void) sendEvent: (NSEvent *)event
{
    if ([event type] == NSKeyUp && ([event modifierFlags] & NSCommandKeyMask))
        [[self keyWindow] sendEvent: event];
    else
        [super sendEvent: event];
}

@end

extern "C" void c3d_initialize() {
    [c3dApplication initialize: nil];
}

extern "C" void c3d_poll_event() {
    [c3dApplication runLoopOnce];
}
