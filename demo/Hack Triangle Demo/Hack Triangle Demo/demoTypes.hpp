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

struct Attribute {
    int x;
};

struct Varying {
    int vX;
};

struct Uniform {};

template<>
void lerp<Varying>(const Varying & v1, const Varying & v2, float lerp, Varying & output)
{
    output.vX = (v2.vX - v1.vX) * lerp + v1.vX;
}

#endif /* demoTypes_h */
