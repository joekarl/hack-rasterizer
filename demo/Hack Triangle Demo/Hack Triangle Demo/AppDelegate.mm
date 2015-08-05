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
    
    
    NSLog(@"Starting");
    
    VertexAttribute vertices[6];
    vertices[0].position = {-1, -1, 0.5};
    vertices[1].position = {1, 1, 0.5};
    vertices[2].position = {-1, 1, 0.5};
    vertices[3].position = {-1, -1, 0.5};
    vertices[4].position = {1, 1, 0.5};
    vertices[5].position = {1, -1, 0.5};
    
    vertices[0].color = {1.0, 0.0, 0.0};
    vertices[1].color = {0.0, 1.0, 0.0};
    vertices[2].color = {0.0, 0.0, 1.0};
    vertices[3].color = {1.0, 0.0, 0.0};
    vertices[4].color = {0.0, 1.0, 0.0};
    vertices[5].color = {0.0, 0.0, 1.0};
    
    Uniform uniforms;
    
    HACK_Context ctx = {1920, 1080};

    // sigh appears we have to alloc scanline memory outside of hack
    // or the compiler optimizes out the memory allocation and explodes :/
    HACK_Scanline<VertexVarying> scanlines[1920];
    
    NSTimeInterval startTime = [[NSDate date] timeIntervalSince1970];
    
    for (int i = 0; i < 100; ++i) {
        HACK_rasterize_triangles<VertexAttribute, VertexVarying, Uniform>(ctx, vertices, uniforms, 6, scanlines);
    }
    
    
    NSTimeInterval endTime = [[NSDate date] timeIntervalSince1970];
    
    NSLog(@"Finished, total time %fms", (endTime - startTime) * 1000);
    exit(0);
}

- (void)applicationWillTerminate:(NSNotification *)aNotification {
    // Insert code here to tear down your application
}

@end
