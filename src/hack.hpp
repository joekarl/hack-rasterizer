#ifndef HACK_H_
#define HACK_H_

#include "hack_types.hpp"

#include <algorithm>
#include <alloca.h>
#include <limits.h>
#include <math.h>

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

template <typename ATTR_TYPE, typename VARY_TYPE, typename UNIF_TYPE>
inline void __HACK_rasterize_triangle(const HACK_Context<VARY_TYPE> &ctx,
                                      const int triangleId,
                                      const ATTR_TYPE *polygonAttributes,
                                      const UNIF_TYPE &uniforms);

/**
 * Rasterize a set of triangles
 * polygonAttributes - <ATTR_TYPE>[] - this array holds all of the per vertex information for all of our triangles
 * uniforms - <UNIF_TYPE> - this object holds things that are uniform to all vertices in all of our triangles
 * vertexCount - int - the number of vertices, should be the length of polygonAttributes
 */
template <typename ATTR_TYPE, typename VARY_TYPE, typename UNIF_TYPE>
inline void HACK_rasterize_triangles(const HACK_Context<VARY_TYPE> &ctx,
                                     const ATTR_TYPE *polygonAttributes,
                                     const UNIF_TYPE &uniforms,
                                     int vertexCount)
{
    // every three vertexes is a triangle we should rasterize
    for (int v = 0; v < vertexCount;) {
        __HACK_rasterize_triangle<ATTR_TYPE, VARY_TYPE, UNIF_TYPE>(ctx, v, polygonAttributes, uniforms);
        v += 3;
    }
}

/**
 * INTERNAL - Rasterize a single triangle
 * triangleId - int - the ID of the triangle we're rendering, this is used to calc which polygon attributes we will use for our vertexes
 * polygonAttributes - <ATTR_TYPE>[] -
 * uniforms - <UNIF_TYPE> -
 * scanlines - <HACK_Scanline<VARY_TYPE>> - scanlines that we will use to scan convert our triangle into
 */
template <typename ATTR_TYPE, typename VARY_TYPE, typename UNIF_TYPE>
inline void __HACK_rasterize_triangle(const HACK_Context<VARY_TYPE> &ctx,
                                    const int triangleId,
                                    const ATTR_TYPE *polygonAttributes,
                                    const UNIF_TYPE &uniforms)
{
    // allocate 3 outputs, one for each vertex
    HACK_vertex<VARY_TYPE> vertexShaderOutput[3];
    shadeVertex(polygonAttributes[triangleId], uniforms, vertexShaderOutput[0]);
    shadeVertex(polygonAttributes[triangleId + 1], uniforms, vertexShaderOutput[1]);
    shadeVertex(polygonAttributes[triangleId + 2], uniforms, vertexShaderOutput[2]);
    
    HACK_Scanline<VARY_TYPE> *scanlines = ctx.scanlines;
    
    int halfHeight = ctx.height / 2;
    int halfWidth = ctx.width / 2;
    
    // calc polygon normal and short circuit if needed
    if (ctx.enableBackfaceCulling == true) {
        
    }
    
    // calculate number of scanlines needed for triangle
    // TODO(karl): clip to ctx y coords
    int bottomScanY = ceil(std::min(std::min(vertexShaderOutput[0].position.y * halfHeight, vertexShaderOutput[1].position.y * halfHeight), vertexShaderOutput[2].position.y * halfHeight));
    int topScanY = ceil(std::max(std::max(vertexShaderOutput[0].position.y * halfHeight, vertexShaderOutput[1].position.y * halfHeight), vertexShaderOutput[2].position.y * halfHeight));
    int scanlineNum = topScanY - bottomScanY;
    
    for (int i = 0; i < scanlineNum; ++i) {
        scanlines[i].leftX = INT_MAX;
        scanlines[i].rightX = INT_MIN;
    }
    
    // populate scanlines with values
    // this is where actual scanline conversion is done
    for (int i = 0; i < 3; ++i) {
        // this is dumb, I shouldn't be making copies here...
        HACK_vertex<VARY_TYPE> v1 = vertexShaderOutput[i];
        HACK_vertex<VARY_TYPE> v2 = (i == 2) ? vertexShaderOutput[0] : vertexShaderOutput[i + 1];
        
        bool vertexPlaneIsLeftVertical = false;
        
        if (v1.position.y > v2.position.y) {
            // if our y is decreasing instead of increasing we need to flip ordering
            std::swap(v1, v2);
            vertexPlaneIsLeftVertical = true;
        }
        
        const HACK_Vec3 &v1Position = v1.position;
        const HACK_Vec3 &v2Position = v2.position;
        
        float dy = (v2Position.y - v1Position.y) * halfHeight;
        float dx = (v2Position.x - v1Position.x) * halfWidth;
        int bottomY = ceil(v1Position.y * halfHeight);
        int topY = ceil(v2Position.y * halfHeight);
        
        if (dy == 0) {
            // we skip horizontal lines because they'll be filled by diagonals later
            // also horizontal lines will fill things with NaNs because of division by zero...
            continue;
        }
        
        if (dx == 0) {
            // have to do special case for vertical line
            int x = ceil(v2Position.x) * halfWidth;
            for (int y = bottomY; y < topY + 1; ++y) {
                HACK_Scanline<VARY_TYPE> &scanline = scanlines[y - bottomScanY];
                scanline.leftX = std::min(scanline.leftX, x);
                scanline.rightX = std::max(scanline.rightX, x);
                float lerpVal = (y - v1Position.y * halfHeight) / (v2Position.y * halfHeight - v1Position.y * halfHeight);
                if (vertexPlaneIsLeftVertical) {
                    lerp(v1.varying, v2.varying, lerpVal, scanline.leftVarying);
                    lerp(v1Position.z, v2Position.z, lerpVal, scanline.rightZ);
                } else {
                    lerp(v1.varying, v2.varying, lerpVal, scanline.rightVarying);
                    lerp(v1Position.z, v2Position.z, lerpVal, scanline.leftZ);
                }
            }
        } else {
        
            // this is slow, should be optimized
            float gradient = dx / dy;
            
            for (int y = bottomY; y < topY + 1; ++y) {
                // line equation
                int x = ceil(v1Position.x * halfWidth + (y - v1Position.y * halfHeight) * gradient);
                
                HACK_Scanline<VARY_TYPE> &scanline = scanlines[y - bottomScanY];
                scanline.leftX = std::min(scanline.leftX, x);
                scanline.rightX = std::max(scanline.rightX, x);
                float lerpVal = (y - v1Position.y * halfHeight) / (v2Position.y * halfHeight - v1Position.y * halfHeight);
                if (x == scanline.leftX) {
                    lerp(v1.varying, v2.varying, lerpVal, scanline.leftVarying);
                    lerp(v1Position.z, v2Position.z, lerpVal, scanline.leftZ);
                }
                if (x == scanline.  rightX) {
                    lerp(v1.varying, v2.varying, lerpVal, scanline.rightVarying);
                    lerp(v1Position.z, v2Position.z, lerpVal, scanline.rightZ);
                }
            }
        }
    }
    

    // we have all of our scanlines setup, now just loop through shading each pixel in the scanline
    VARY_TYPE lerpedVarying;
    HACK_pixel pixelOutput;
    for (int i = 0; i < scanlineNum; ++i) {
        const HACK_Scanline<VARY_TYPE> &scanline = scanlines[i];
        
        // clip scanline to ctx space
        // TODO(karl): check against ctx x coords
        
        //*
        for (int j = scanline.leftX; j < scanline.rightX + 1; ++j) {
            // lerp the left and right of the scanline into
            float dx = scanline.rightX - scanline.leftX;
            float lerpVal = (dx != 0) ? static_cast<float>(j - scanline.leftX) / static_cast<float>(scanline.rightX - scanline.leftX) : 0;
            lerp<VARY_TYPE>(scanline.leftVarying, scanline.rightVarying, lerpVal, lerpedVarying);
            float pixelZ = -1;
            lerp(scanline.leftZ, scanline.rightZ, lerpVal, pixelZ);
            int pixelX = j + halfWidth;
            int pixelY = i + bottomScanY + halfHeight;
            if (pixelX == ctx.width) {
                pixelX -= 1;
            }
            
            //NSLog(@"scanline left: %d right %d", scanline.leftX, scanline.rightX);
            //NSLog(@"lerp val %f", lerpVal);
            //NSLog(@"shading pixel {%d, %d, %f}", pixelX, pixelY, pixelZ);
            shadeFragment(lerpedVarying, uniforms, pixelOutput);
            //NSLog(@"color is {%f, %f, %f, %f}", pixelOutput.color.r, pixelOutput.color.g, pixelOutput.color.b, pixelOutput.color.a);
            
            // update depth and color buffers with our rendering context
            // this is ARGB
            int pixelBase = (pixelX + (ctx.height - 1 - pixelY) * (ctx.width)) * 4;
            //NSLog(@"pixel base %d j %d scanlineL %d scanlineR %d", pixelBase, j, scanline.leftX, scanline.rightX);
            ctx.colorBuffer[pixelBase] = floor(pixelOutput.color.a * 255);
            ctx.colorBuffer[pixelBase + 1] = floor(pixelOutput.color.r * 255);
            ctx.colorBuffer[pixelBase + 2] = floor(pixelOutput.color.g * 255);
            ctx.colorBuffer[pixelBase + 3] = floor(pixelOutput.color.b * 255);
        }
        
        //*/
    }
}


#endif
