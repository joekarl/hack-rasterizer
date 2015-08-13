//
//  shaders.hpp
//  Hack Triangle Demo
//
//  Created by Karl Kirch on 8/4/15.
//  Copyright © 2015 Karl Kirch. All rights reserved.
//

#ifndef shaders_h
#define shaders_h

#include "demoTypes.hpp"
#include "hack_types.hpp"

template<>
inline void shadeVertex(const VertexAttribute &vert, const Uniform &uniforms, HACK_vertex<VertexVarying> &output)
{
    // rotate around z axis
    Demo_Vec3 rotatedPositionVec;
    rotatedPositionVec.x = (vert.position.x * cos(uniforms.zAxisRotation)) - (vert.position.y * sin(uniforms.zAxisRotation));
    rotatedPositionVec.y = (vert.position.x * sin(uniforms.zAxisRotation)) + (vert.position.y * cos(uniforms.zAxisRotation));
    rotatedPositionVec.z = vert.position.z;
    
    // rotate around y axis
    Demo_Vec3 rotatedPositionVec2;
    rotatedPositionVec2.z = rotatedPositionVec.y * sin(uniforms.zAxisRotation) + rotatedPositionVec.z * cos(uniforms.zAxisRotation);
    rotatedPositionVec2.x = rotatedPositionVec.x * cos(uniforms.zAxisRotation) - rotatedPositionVec.z * sin(uniforms.zAxisRotation);
    rotatedPositionVec2.y = rotatedPositionVec.y;
    
    output.position = toHackVec(rotatedPositionVec2 + uniforms.translateVec);
    output.varying.color = vert.color;
}

template<>
inline void shadeFragment(const VertexVarying &varying, const Uniform &uniforms, HACK_pixel &output)
{
    output.color.xyz = toHackVec(varying.color);
    //NSLog(@"var color r: %f g: %f b: %f", varying.color.r, varying.color.g, varying.color.b);
    output.color._a = 1.0;
}

#endif /* shaders_h */
