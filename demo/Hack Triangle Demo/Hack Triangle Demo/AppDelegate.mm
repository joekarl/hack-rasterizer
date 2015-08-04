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


void vShader(const Attribute &attr, const Uniform &uniforms, HACK_vertex<Varying> &output)
{
    NSLog(@"x is %d", attr.x);
    output.varying.vX = attr.x * 10;
}

void fShader(const Varying &varying, const Uniform &uniforms, HACK_pixel &output)
{
    NSLog(@"vx is %d", varying.vX);
    output.z = varying.vX;
}

@interface AppDelegate ()

@property (weak) IBOutlet NSWindow *window;
@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    // Insert code here to initialize your application
    
    Attribute attrs[3];
    attrs[0].x = 1;
    attrs[1].x = 2;
    attrs[2].x = 3;
    Uniform uniforms;
    
    HACK_Context ctx;
    HACK_rasterize_triangles(ctx, attrs, uniforms, 3, &vShader, &fShader);
}

- (void)applicationWillTerminate:(NSNotification *)aNotification {
    // Insert code here to tear down your application
}

@end
