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
    
    VertexAttribute vertices[3];
    vertices[0].position = {-0.5, -0.5, 0.5};
    vertices[1].position = {0.0, 0.5, 0.5};
    vertices[2].position = {0.5, -0.5, 0.5};
    
    vertices[0].color = {1.0, 0.0, 0.0};
    vertices[1].color = {0.0, 1.0, 0.0};
    vertices[2].color = {0.0, 0.0, 1.0};
    
    Uniform uniforms;
    
    HACK_Context ctx = {640, 480};
    
    NSTimeInterval startTime = [[NSDate date] timeIntervalSince1970];
    
    for (int i = 0; i < 1000; ++i) {
        HACK_rasterize_triangles<VertexAttribute, VertexVarying, Uniform>(ctx, vertices, uniforms, 3);
    }
    
    
    NSTimeInterval endTime = [[NSDate date] timeIntervalSince1970];
    
    NSLog(@"Finished, total time %fms", (endTime - startTime) * 1000);
    exit(0);
}

- (void)applicationWillTerminate:(NSNotification *)aNotification {
    // Insert code here to tear down your application
}

@end
