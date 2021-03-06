//
//  hack_types.hpp
//  Hack Triangle Demo
//
//  Created by Karl Kirch on 8/4/15.
//  Copyright © 2015 Karl Kirch. All rights reserved.
//

#ifndef hack_types_h
#define hack_types_h

template <typename VARY_TYPE>
struct HACK_Scanline;

/**
 * Our rendering context
 */
template <typename VARY_TYPE>
struct HACK_Context
{
    int width, height;
    HACK_Scanline<VARY_TYPE> *scanlines;
    bool enableBackfaceCulling;
    unsigned char *colorBuffer;
    float zClipNear, zClipFar;
};

/**
 * Scanline representation
 * Holds left/right positions and associated varying objects
 */
template <typename VARY_TYPE>
struct HACK_Scanline {
    int leftX, rightX;
    float leftZ, rightZ;
    VARY_TYPE leftVarying, rightVarying;
};

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

inline HACK_Vec3 _HACK_Cross_Product(const HACK_Vec3 &v1, const HACK_Vec3 &v2) {
    HACK_Vec3 result;
    result.x = v1.y * v2.z - v1.z * v2.y;
    result.y = v1.z * v2.x - v1.x * v2.z;
    result.z = v1.x * v2.y - v1.y * v2.x;
    return result;
}

inline float _HACK_Dot_Product(const HACK_Vec3 &v1, const HACK_Vec3 &v2) {
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

/**
 * Output of a fragment shader,
 * contains the color and z depth of our pixel
 */
struct HACK_pixel {
    HACK_Vec4 color;
};

/**
 * Output of a vertex shader
 * contains the vertex position and an associated varying object with our vertex
 */
template <typename VARY_TYPE>
struct HACK_vertex {
    HACK_Vec3 position;
    VARY_TYPE varying;
};

/**
 * vertex shader template function
 */
template <typename ATTR_TYPE, typename UNIF_TYPE, typename VARY_TYPE>
void shadeVertex(const ATTR_TYPE &attribute, const UNIF_TYPE &uniform, HACK_vertex<VARY_TYPE> &output);

/**
 * fragment shader template function
 */
template <typename VARY_TYPE, typename UNIF_TYPE>
void shadeFragment(const VARY_TYPE &varying, const UNIF_TYPE &uniform, HACK_pixel &output);

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
