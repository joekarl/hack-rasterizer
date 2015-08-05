//
//  shaders.hpp
//  Hack Triangle Demo
//
//  Created by Karl Kirch on 8/4/15.
//  Copyright © 2015 Karl Kirch. All rights reserved.
//

#ifndef shaders_h
#define shaders_h

#include "hack_types.hpp"

template<>
void shadeVertex(const VertexAttribute &vert, const Uniform &uniforms, HACK_vertex<VertexVarying> &output)
{
    output.position = vert.position;
    output.varying.color = vert.color;
}

template<>
void shadeFragment(const VertexVarying &varying, const Uniform &uniforms, HACK_pixel &output)
{
    output.color.xyz = varying.color;
    output.color._a = 1.0;
}

#endif /* shaders_h */
