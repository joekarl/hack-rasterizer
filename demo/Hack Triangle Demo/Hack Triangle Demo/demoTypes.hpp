//
//  demoTypes.h
//  Hack Triangle Demo
//
//  Created by Karl Kirch on 8/3/15.
//  Copyright © 2015 Karl Kirch. All rights reserved.
//

#ifndef demoTypes_h
#define demoTypes_h

#include "hack.hpp"

struct VertexAttribute {
    HACK_Vec3 position;
    HACK_Vec3 color;
};

struct VertexVarying {
    HACK_Vec3 color;
};

struct Uniform {};

template<>
inline void lerp<VertexVarying>(const VertexVarying &v1, const VertexVarying &v2, float lerpVal, VertexVarying &output)
{
    lerp(v1.color, v2.color, lerpVal, output.color);
}


#endif /* demoTypes_h */
