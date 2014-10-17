#include "cAppLauncher.h"
#include "platform/launcher.h"
#include "platform/game_window.h"

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

class mac_launcher : public launcher {
private:
    mac_launcher() {
        static dispatch_once_t onceToken;
        dispatch_once(&onceToken, ^{
            [c3dApplication initialize: nil];
        });
    }
    
    virtual bool poll_event(bool wait) override {
        [c3dApplication runLoopOnce];
        return true;
    }
    
    virtual game_window::ptr create_game_window(vector2f const& size,
                                                vector2f const& origin = vector2f::Zero()) override {
        return nullptr;
    }
    
    friend launcher& launcher::initialize();
};

launcher& launcher::initialize() {
    static mac_launcher mac;
    return mac;
}
