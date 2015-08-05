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
    
    VertexAttribute vertices[3];
    vertices[0].position.x = -0.5;
    vertices[0].position.y = -0.5;
    vertices[0].position.z = 0.5;
    vertices[1].position.x = 0.0;
    vertices[1].position.y = 0.5;
    vertices[1].position.z = 0.5;
    vertices[2].position.x = 0.5;
    vertices[2].position.y = -0.5;
    vertices[2].position.z = 0.5;
    
    vertices[0].color.r = 1.0;
    vertices[0].color.g = 0.0;
    vertices[0].color.b = 0.0;
    vertices[1].color.r = 0.0;
    vertices[1].color.g = 1.0;
    vertices[1].color.b = 0.0;
    vertices[2].color.r = 0.0;
    vertices[2].color.g = 0.0;
    vertices[2].color.b = 1.0;
    
    Uniform uniforms;
    
    HACK_Context ctx = {640, 480};
    
    NSLog(@"Starting");
    for (int i = 0; i < 1000; ++i) {
        HACK_rasterize_triangles<VertexAttribute, VertexVarying, Uniform>(ctx, vertices, uniforms, 3);
    }
    
    NSLog(@"Finished");
    exit(0);
}

- (void)applicationWillTerminate:(NSNotification *)aNotification {
    // Insert code here to tear down your application
}

@end
