//
//  hack_types.hpp
//  Hack Triangle Demo
//
//  Created by Karl Kirch on 8/4/15.
//  Copyright © 2015 Karl Kirch. All rights reserved.
//

#ifndef hack_types_h
#define hack_types_h

struct HACK_Vec3 {
    union{
        struct {
            float x, y, z;
        };
        struct {
            float r, g, b;
        };
    };
};

struct HACK_Vec4 {
    union {
        struct {
            float x, y, z, w;
        };
        struct {
            float r, g, b, a;
        };
        struct {
            HACK_Vec3 xyz;
            float _w;
        };
        struct {
            HACK_Vec3 rgb;
            float _a;
        };
    };
};

/**
 * lerp function template
 * Anything that is used as a VARY_TYPE must have lerp implemented for it
 */
template <typename T>
void lerp(const T &v1, const T &v2, float lerp, T &output);

template<>
inline void lerp<float>(const float &f1, const float &f2, float lerp, float &output)
{
    output = (f2 - f1) * lerp + f1;
}

template<>
inline void lerp<HACK_Vec3>(const HACK_Vec3 &v1, const HACK_Vec3 &v2, float lerpVal, HACK_Vec3 &output)
{
    output.x = (v2.x - v1.x) * lerpVal + v1.x;
    output.y = (v2.y - v1.y) * lerpVal + v1.y;
    output.z = (v2.z - v1.z) * lerpVal + v1.z;
}

template<>
inline void lerp<HACK_Vec4>(const HACK_Vec4 &v1, const HACK_Vec4 &v2, float lerpVal, HACK_Vec4 &output)
{
    output.x = (v2.x - v1.x) * lerpVal + v1.x;
    output.y = (v2.y - v1.y) * lerpVal + v1.y;
    output.z = (v2.z - v1.z) * lerpVal + v1.z;
    output.w = (v2.w - v1.w) * lerpVal + v1.w;
}

#endif /* hack_types_h */
