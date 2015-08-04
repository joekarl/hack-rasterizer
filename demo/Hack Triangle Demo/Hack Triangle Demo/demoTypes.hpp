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

struct Vec3 {
    union{
        struct {
            float x, y, z;
        };
        struct {
            float r, g, b;
        };
    };
};

struct Vec4 {
    union {
        struct {
            float x, y, z, w;
        };
        struct {
            float r, g, b, a;
        };
        struct {
            Vec3 xyz;
            float _w;
        };
        struct {
            Vec3 rgb;
            float _a;
        };
    };
};


struct VertexAttribute {
    Vec3 position;
    Vec4 color;
};

struct VertexVarying {
    Vec4 color;
};

struct Uniform {};

template<>
void lerp<Vec4>(const Vec4 &v1, const Vec4 &v2, float lerpVal, Vec4 &output)
{
    output.x = (v2.x - v1.x) * lerpVal + v1.x;
    output.y = (v2.y - v1.y) * lerpVal + v1.y;
    output.z = (v2.z - v1.z) * lerpVal + v1.z;
    output.w = (v2.w - v1.w) * lerpVal + v1.w;
}

template<>
void lerp<VertexVarying>(const VertexVarying &v1, const VertexVarying &v2, float lerpVal, VertexVarying &output)
{
    lerp(v1.color, v2.color, lerpVal, output.color);
}


#endif /* demoTypes_h */
