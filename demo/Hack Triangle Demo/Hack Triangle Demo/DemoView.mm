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

@interface DemoView () {
    NSTimer *sceneTimer;
    VertexAttribute *vertices;
    Uniform *uniform;
    HACK_Context<VertexVarying> *ctx;
    GLuint textureId;
    double lastFrameTime;
    int accumulator;
    int fps;
    bool useWireframe;
}

@end

@implementation DemoView

-(id)initWithCoder:(NSCoder *)coder
{
    if (self = [super initWithCoder:coder]) {
    }
    return self;
}

- (void)awakeFromNib {
    [super awakeFromNib];
    if (self) {
        [self initScene];
        [self initGl];
        
        sceneTimer = [NSTimer timerWithTimeInterval:1.0/30.0 target:self selector:@selector(updateRenderScene) userInfo:nil repeats:YES];
        [[NSRunLoop mainRunLoop] addTimer:sceneTimer forMode:NSDefaultRunLoopMode];
        NSLog(@"Finished initing demo view");
    }
}

- (void)initGl {
    NSOpenGLPixelFormatAttribute pfa[] = {
        NSOpenGLPFADoubleBuffer,
        NSOpenGLPFAAccelerated,
        0
    };
    NSOpenGLPixelFormat *pf = [[NSOpenGLPixelFormat alloc] initWithAttributes:pfa];
    if (!pf) {
        NSLog(@"Couldn't init opengl with double buffering, trying single buffer");
        NSOpenGLPixelFormatAttribute pfaFallback[] = {
            0
        };
        pf = [[NSOpenGLPixelFormat alloc] initWithAttributes:pfaFallback];
    }
    if (!pf) {
        NSLog(@"Couldn't init opengl at all o_O, sorry :(");
        abort();
    }
    
    NSOpenGLContext *glContext = [[NSOpenGLContext alloc] initWithFormat:pf shareContext:nil];
    [self setPixelFormat:pf];
    [self setOpenGLContext:glContext];
}

- (void)initScene {
    ctx = (HACK_Context<VertexVarying>*) calloc(1, sizeof(HACK_Context<VertexVarying>));
    ctx->width = 1024;
    ctx->height = 1024;
    ctx->scanlines = (HACK_Scanline<VertexVarying>*) calloc(ctx->height, sizeof(HACK_Scanline<VertexVarying>));
    ctx->enableBackfaceCulling = false;
    ctx->colorBuffer = (unsigned char *) calloc(ctx->width * ctx->height * 4, sizeof(char));

    
    vertices = (VertexAttribute*) calloc(3, sizeof(VertexAttribute));
    vertices[0].position = {-0.5, -0.5, 0.5};
    vertices[1].position = {0.5, -0.5, 0.5};
    vertices[2].position = {0, 0.5, 0.5};
    
    vertices[0].color = {1.0, 0.0, 0.0};
    vertices[1].color = {0.0, 0.0, 1.0};
    vertices[2].color = {0.0, 1.0, 0.0};
    
    uniform = (Uniform*) calloc(1, sizeof(Uniform));
}

-(void)prepareOpenGL
{
    [super prepareOpenGL];
    
    // make current context for use
    [[self openGLContext] makeCurrentContext];
    
    // lock context just in case something else is using it (shouldn't be though)
    CGLLockContext([[self openGLContext] CGLContextObj]);
    
    glDisable(GL_DEPTH_TEST);
    glLoadIdentity();
    
    // setup viewport for opengl
    NSSize size = [self frame].size;
    glViewport(0, 0, size.width, size.height);
    
    // set to use vsync (will default to 60hz in theory)
    GLint vsync = 1;
    [[self openGLContext] setValues:&vsync forParameter:NSOpenGLCPSwapInterval];
    
    // I don't know what this does o_0
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    
    // create texture for later so we can draw into it
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    
    // clear texture initially to get rid of junk from previous memory
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGBA,
                 ctx->width, ctx->height,
                 0,
                 GL_BGRA_EXT,
                 GL_UNSIGNED_BYTE,
                 NULL);
    
    // setup texture modes
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    
    // release lock on the opengl context
    CGLUnlockContext([[self openGLContext] CGLContextObj]);
}

/**
 * Called when window is resized
 * This resizes the opengl context for us
 */
-(void)reshape
{
    CGLLockContext([[self openGLContext] CGLContextObj]);
    
    // make current for subsequent opengl calls
    [[self openGLContext] makeCurrentContext];
    
    glDisable(GL_DEPTH_TEST);
    glLoadIdentity();
    
    // setup viewport for opengl
    NSSize size = [self frame].size;
    glViewport(0, 0, size.width, size.height);
    
    CGLUnlockContext([[self openGLContext] CGLContextObj]);
    
    [self renderScene];
}

- (void) updateRenderScene {
    double startTime = CACurrentMediaTime();
    
    [self updateScene:(startTime - lastFrameTime)];
    [self renderScene];
    
    double endTime = CACurrentMediaTime();
    lastFrameTime = endTime;
}

/**
 * Update the color buffer with the contents of our scene
 */
- (void)updateScene:(double)dt {
    accumulator += dt * 1000;
    fps++;
    if (accumulator > 1000) {
        NSLog(@"FPS: %d", fps);
        useWireframe = !useWireframe;
        accumulator = accumulator % 1000;
        fps = 0;
    }
    
    HACK_clear_color_buffer(*ctx);
    HACK_rasterize_triangles<VertexAttribute, VertexVarying, Uniform>(*ctx, vertices, *uniform, 3, useWireframe);
    
}

/**
 * Render our color buffer via opengl to the screen
 */
- (void)renderScene {
    CGLLockContext([[self openGLContext] CGLContextObj]);
    
    // make current for subsequent opengl calls
    [[self openGLContext] makeCurrentContext];
    
    GLfloat glVertices[] =
    {
        -1, -1, 0,
        -1, 1, 0,
        1, 1, 0,
        1, -1, 0,
    };
    
    GLfloat glTexCoords[] =
    {
        1, 1,
        1, 0,
        0, 0,
        0, 1,
    };
    
    glVertexPointer(3, GL_FLOAT, 0, glVertices);
    glTexCoordPointer(2, GL_FLOAT, 0, glTexCoords);
    
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    
    glBindTexture(GL_TEXTURE_2D, textureId);
    
    glEnable(GL_TEXTURE_2D);
    
    // update texture
    glTexSubImage2D(GL_TEXTURE_2D,
                    0,
                    0,
                    0,
                    ctx->width, ctx->height,
                    GL_ABGR_EXT,
                    GL_UNSIGNED_BYTE,
                    ctx->colorBuffer);
    
    glColor4f(1, 1, 1, 1);
    GLushort triangleIndices[] = { 0, 1, 2, 0, 2, 3 };
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, triangleIndices);
    glDisable(GL_TEXTURE_2D);
    
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    
    glFlush();
    
    // we're double buffered so need to flush to screen
    [[self openGLContext] flushBuffer];
    
    CGLUnlockContext([[self openGLContext] CGLContextObj]);
}

@end