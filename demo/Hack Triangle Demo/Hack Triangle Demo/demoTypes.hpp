//
//  demoTypes.h
//  Hack Triangle Demo
//
//  Created by Karl Kirch on 8/3/15.
//  Copyright © 2015 Karl Kirch. All rights reserved.
//

#ifndef demoTypes_h
#define demoTypes_h

#include "hack_types.hpp"

struct Demo_Vec3 {
    float x, y, z;
    
    Demo_Vec3 operator+(const Demo_Vec3 &demoVec) const {
        Demo_Vec3 v;
        v.x = x + demoVec.x;
        v.y = y + demoVec.y;
        v.z = z + demoVec.z;
        return v;
    };
};

struct Demo_Vec4 {
    float x, y, z, w;
};

inline HACK_Vec3 toHackVec(const Demo_Vec3 &demoVec) {
    HACK_Vec3 hv;
    hv.x = demoVec.x;
    hv.y = demoVec.y;
    hv.z = demoVec.z;
    return hv;
}

inline HACK_Vec4 toHackVec(const Demo_Vec4 &demoVec) {
    HACK_Vec4 hv;
    hv.x = demoVec.x;
    hv.y = demoVec.y;
    hv.z = demoVec.z;
    hv.w = demoVec.w;
    return hv;
}

struct VertexAttribute {
    Demo_Vec3 position;
    Demo_Vec3 color;
};

struct VertexVarying {
    Demo_Vec3 color;
};

struct Uniform {
    Demo_Vec3 translateVec;
    float zAxisRotation;
};

template<>
inline void lerp<Demo_Vec3>(const Demo_Vec3 &v1, const Demo_Vec3 &v2, float lerpVal, Demo_Vec3 &output)
{
    output.x = (v2.x - v1.x) * lerpVal + v1.x;
    output.y = (v2.y - v1.y) * lerpVal + v1.y;
    output.z = (v2.z - v1.z) * lerpVal + v1.z;
}

template<>
inline void lerp<VertexVarying>(const VertexVarying &v1, const VertexVarying &v2, float lerpVal, VertexVarying &output)
{
    lerp(v1.color, v2.color, lerpVal, output.color);
}


#endif /* demoTypes_h */
