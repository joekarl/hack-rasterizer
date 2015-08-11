//
//  DemoView.m
//  Hack Triangle Demo
//
//  Created by Karl Kirch on 8/10/15.
//  Copyright © 2015 Karl Kirch. All rights reserved.
//

#import "DemoView.h"
#import "hack.hpp"
#import "demoTypes.hpp"
#import "shaders.hpp"

@interface DemoView ()

- (void)renderScene;
- (void)initScene;

@end

@implementation DemoView

@synthesize vertices;
@synthesize uniform;
@synthesize ctx;
@synthesize sceneTimer;
@synthesize bitmapCtx;

- (void)awakeFromNib {
    [super awakeFromNib];
    if (self) {
        ctx = (HACK_Context<VertexVarying>*) calloc(1, sizeof(HACK_Context<VertexVarying>));
        ctx->width = 320;
        ctx->height = 240;
        ctx->scanlines = (HACK_Scanline<VertexVarying>*) calloc(ctx->height, sizeof(HACK_Scanline<VertexVarying>));
        ctx->enableBackfaceCulling = false;
        ctx->colorBuffer = (unsigned char *) calloc(ctx->width * ctx->height * 4, sizeof(char));
        
        CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
        bitmapCtx = CGBitmapContextCreate(ctx->colorBuffer, ctx->width, ctx->height, 8, ctx->width * 4, colorSpace, kCGImageAlphaNoneSkipFirst | kCGBitmapByteOrder32Big);

        [self initScene];
        
        sceneTimer = [NSTimer timerWithTimeInterval:1.0 / 30.0 target:self selector:@selector(renderScene) userInfo:nil repeats:YES];
        [[NSRunLoop mainRunLoop] addTimer:sceneTimer forMode:NSDefaultRunLoopMode];
        NSLog(@"Finished initing demo view");
    }
}

- (void)initScene {
    vertices = (VertexAttribute*) calloc(3, sizeof(VertexAttribute));
    vertices[0].position = {-0.5, -0.5, 0.5};
    vertices[1].position = {0.5, -0.5, 0.5};
    vertices[2].position = {0, 0.5, 0.5};
    
    vertices[0].color = {1.0, 0.0, 0.0};
    vertices[1].color = {0.0, 0.0, 1.0};
    vertices[2].color = {0.0, 1.0, 0.0};
    
    uniform = (Uniform*) calloc(1, sizeof(Uniform));
}

- (void)renderScene {
    
    double startTime = CACurrentMediaTime();
    
    HACK_rasterize_triangles<VertexAttribute, VertexVarying, Uniform>(*ctx, vertices, *uniform, 3);
    
    double endTime = CACurrentMediaTime();
    double totalTime = endTime - startTime;
    NSLog(@"FrameTime: %fms", totalTime * 1000);
    
    [self setNeedsDisplay:YES];
}

- (void)drawRect:(NSRect)dirtyRect {
    CGContextRef currentContext = (CGContextRef) [[NSGraphicsContext currentContext] graphicsPort];
    CGImageRef bitmapRef = CGBitmapContextCreateImage(bitmapCtx);
    CGContextDrawImage(currentContext, self.bounds, bitmapRef);
    CGImageRelease(bitmapRef);
}

@end