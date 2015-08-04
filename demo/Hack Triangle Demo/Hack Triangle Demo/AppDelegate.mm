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


inline void vShader(const VertexAttribute &vert, const Uniform &uniforms, HACK_vertex<VertexVarying> &output)
{
    output.position = vert.position;
    output.varying.color = vert.color;
}

inline void fShader(const VertexVarying &varying, const Uniform &uniforms, HACK_pixel &output)
{
    output.color.xyz = varying.color;
    output.color._a = 1.0;
    
    //NSLog(@"color is {%f, %f, %f, %f}", output.color.r, output.color.g, output.color.b, output.color.a);
}

@interface AppDelegate ()

@property (weak) IBOutlet NSWindow *window;
@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    // Insert code here to initialize your application
    
    NSLog(@"Starting");
    
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
    
    // JANKY HACK
    // TODO(karl): make this die in a fire
    HACK_Scanline<VertexVarying> scanlines[640];
    
    HACK_Context ctx = {640, 480};
    for (int i = 0; i < 100; ++i) {
        HACK_rasterize_triangles(ctx, vertices, uniforms, 3, &vShader, &fShader, scanlines);
    }
    
    NSLog(@"Finished");
}

- (void)applicationWillTerminate:(NSNotification *)aNotification {
    // Insert code here to tear down your application
}

@end
