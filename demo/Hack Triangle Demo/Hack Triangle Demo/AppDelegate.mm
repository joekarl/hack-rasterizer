//
//  AppDelegate.m
//  Hack Triangle Demo
//
//  Created by Karl Kirch on 8/3/15.
//  Copyright © 2015 Karl Kirch. All rights reserved.
//

#import "AppDelegate.h"
#include "demoTypes.hpp"
#include "hack.hpp"
#include "shaders.hpp"

@interface AppDelegate ()

@property (weak) IBOutlet NSWindow *window;
@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    // Insert code here to initialize your application
}

- (void)applicationWillTerminate:(NSNotification *)aNotification {
    // Insert code here to tear down your application
}

- (BOOL) applicationShouldTerminateAfterLastWindowClosed: (NSApplication *) theApplication {
    return YES;
}

@end
