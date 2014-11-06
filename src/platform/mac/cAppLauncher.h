#ifndef _CHAOS3D_PLATFORM_MAC_CAPPLAUNCHER_H
#define _CHAOS3D_PLATFORM_MAC_CAPPLAUNCHER_H

#include <AppKit/AppKit.h>

class timer;

@interface c3dApplication : NSApplication

// helper to start from nib file
@property (weak) IBOutlet NSWindow *window;

+ (NSApplication*) initialize: (Class) delegateClass;
+ (BOOL) runLoopOnce: (timer*) timer_;

@end

#endif
