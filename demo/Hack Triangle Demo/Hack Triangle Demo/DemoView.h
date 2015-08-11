//
//  DemoView.h
//  Hack Triangle Demo
//
//  Created by Karl Kirch on 8/10/15.
//  Copyright © 2015 Karl Kirch. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "demoTypes.hpp"

@interface DemoView : NSView

@property NSTimer *sceneTimer;
@property VertexAttribute *vertices;
@property Uniform *uniform;
@property HACK_Context<VertexVarying> *ctx;
@property CGContextRef bitmapCtx;

@end
