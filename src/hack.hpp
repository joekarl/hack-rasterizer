#ifndef HACK_H_
#define HACK_H_

#include "hack_types.hpp"

#include <algorithm>
#include <alloca.h>
#include <cstdlib>
#include <limits.h>

inline int _HACK_fast_ceil(float f) {
    return static_cast<int>(f + 0.5);
}

inline int _HACK_fast_floor(float f) {
    return static_cast<int>(f - 0.5);
}

template <typename VARY_TYPE>
inline void HACK_clear_color_buffer(const HACK_Context<VARY_TYPE> &ctx)
{
    memset(ctx.colorBuffer, 0, ctx.width * ctx.height * 4);
}

template <typename ATTR_TYPE, typename VARY_TYPE, typename UNIF_TYPE>
inline void __HACK_rasterize_filled_polygon(const HACK_Context<VARY_TYPE> &ctx,
                                            const HACK_vertex<VARY_TYPE> *vertices,
                                            const int vertexCount,
                                            const UNIF_TYPE &uniforms);

template <typename ATTR_TYPE, typename VARY_TYPE, typename UNIF_TYPE>
inline void __HACK_rasterize_wireframe_polygon(const HACK_Context<VARY_TYPE> &ctx,
                                               const HACK_vertex<VARY_TYPE> *vertices,
                                               const int vertexCount,
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
                                     int vertexCount,
                                     bool fillTriangles)
{
    // every three vertexes is a triangle we should rasterize
    for (int v = 0; v < vertexCount;) {
        
        // take 3 vertices, vertex shade them, clip against screen and zNear/zFar
        // allocate 3 outputs, one for each vertex
        static HACK_vertex<VARY_TYPE> vertexShaderOutput[7];
        shadeVertex(polygonAttributes[v], uniforms, vertexShaderOutput[0]);
        shadeVertex(polygonAttributes[v + 1], uniforms, vertexShaderOutput[1]);
        shadeVertex(polygonAttributes[v + 2], uniforms, vertexShaderOutput[2]);
        
        // do clipping
        
        // take resulting polygon and scan convert + pixel shade
        
        if (fillTriangles) {
            __HACK_rasterize_filled_polygon<ATTR_TYPE, VARY_TYPE, UNIF_TYPE>(ctx, vertexShaderOutput, 3, uniforms);
        } else {
            __HACK_rasterize_wireframe_polygon<ATTR_TYPE, VARY_TYPE, UNIF_TYPE>(ctx, vertexShaderOutput, 3, uniforms);
        }
        v += 3;
    }
}

template<typename VARY_TYPE>
inline void debugPrint(const VARY_TYPE &vary);

/**
 * INTERNAL - Rasterize a single filled triangle
 * triangleId - int - the ID of the triangle we're rendering, this is used to calc which polygon attributes we will use for our vertexes
 * polygonAttributes - <ATTR_TYPE>[] -
 * uniforms - <UNIF_TYPE> -
 * scanlines - <HACK_Scanline<VARY_TYPE>> - scanlines that we will use to scan convert our triangle into
 */
template <typename ATTR_TYPE, typename VARY_TYPE, typename UNIF_TYPE>
inline void __HACK_rasterize_filled_polygon(const HACK_Context<VARY_TYPE> &ctx,
                                            const HACK_vertex<VARY_TYPE> *vertices,
                                            const int vertexCount,
                                            const UNIF_TYPE &uniforms)
{
    HACK_Scanline<VARY_TYPE> *scanlines = ctx.scanlines;
    
    int halfHeight = ctx.height / 2;
    int halfWidth = ctx.width / 2;
    
    HACK_Vec3 edge1 = {
        vertices[1].position.x - vertices[0].position.x,
        vertices[1].position.y - vertices[0].position.y,
        vertices[1].position.z - vertices[0].position.z
    };
    HACK_Vec3 edge2 = {
        vertices[2].position.x - vertices[1].position.x,
        vertices[2].position.y - vertices[1].position.y,
        vertices[2].position.z - vertices[1].position.z
    };
    HACK_Vec3 surfaceNormal = _HACK_Cross_Product(edge1, edge2);
    HACK_Vec3 cameraDirection = {0, 0, 1};
    
    float angleToTriangleNormal = _HACK_Dot_Product(surfaceNormal, cameraDirection);
    
    bool isBackFacing = angleToTriangleNormal < 0;
    
    // calc polygon normal and short circuit if needed
    if (ctx.enableBackfaceCulling && isBackFacing) {
        return;
    }
    
    // calculate number of scanlines needed for triangle
    int bottomScanY = _HACK_fast_ceil(std::max(0.0f,
                                               std::min(std::min(vertices[0].position.y * halfHeight + halfHeight,
                                                                 vertices[1].position.y * halfHeight + halfHeight),
                                                        vertices[2].position.y * halfHeight + halfHeight)));
    int topScanY = _HACK_fast_ceil(std::min(static_cast<float>(ctx.height),
                                            std::max(std::max(vertices[0].position.y * halfHeight + halfHeight,
                                                              vertices[1].position.y * halfHeight + halfHeight),
                                                     vertices[2].position.y * halfHeight + halfHeight)));
    
    for (int i = bottomScanY; i != topScanY + 1; ++i) {
        scanlines[i].leftX = INT_MAX;
        scanlines[i].rightX = INT_MIN;
    }
    
    // populate scanlines with values
    // this is where actual scanline conversion is done
    for (int i = 0; i < vertexCount; ++i) {
        const HACK_vertex<VARY_TYPE> &v1 = vertices[i];
        const HACK_vertex<VARY_TYPE> &v2 = (i == vertexCount - 1) ? vertices[0] : vertices[i + 1];
        
        int v1x = _HACK_fast_ceil(v1.position.x * halfWidth);
        int v2x = _HACK_fast_ceil(v2.position.x * halfWidth);
        int v1y = _HACK_fast_ceil(v1.position.y * halfHeight);
        int v2y = _HACK_fast_ceil(v2.position.y * halfHeight);
        
        int dx = std::abs(v2x - v1x);
        int dy = std::abs(v2y - v1y);
        
        if (dy == 0) {
            // horizontal special case
            continue;
        } else if (dx == 0) {
            // vertical special case
            int sy = v1y < v2y ? 1 : -1;
            for (int currY = v1y; currY != v2y + sy; currY += sy) {
                HACK_Scanline<VARY_TYPE> &scanline = scanlines[currY + halfHeight];
                scanline.leftX = std::min(scanline.leftX, v2x);
                scanline.rightX = std::max(scanline.rightX, v2x);
                float lerpVal = static_cast<float>(currY - v1y) / static_cast<float>(v2y - v1y);
                
                if ((sy > 0 && !isBackFacing) || (sy < 0 && isBackFacing)) {
                    lerp(v1.varying, v2.varying, lerpVal, scanline.rightVarying);
                    lerp(v1.position.z, v2.position.z, lerpVal, scanline.rightZ);
                } else {
                    lerp(v1.varying, v2.varying, lerpVal, scanline.leftVarying);
                    lerp(v1.position.z, v2.position.z, lerpVal, scanline.leftZ);
                }
            }
        } else {
            // general case
            
            int currX = v1x;
            int currY = v1y;
            
            int sx = v1x < v2x ? 1 : -1;
            int sy = v1y < v2y ? 1 : -1;
            
            int err = (dx > dy ? dx : -dy) / 2;
            int err2;
            
            for (;;) {
                HACK_Scanline<VARY_TYPE> &scanline = scanlines[currY + halfHeight];
                scanline.leftX = std::min(scanline.leftX, currX);
                scanline.rightX = std::max(scanline.rightX, currX);
                float lerpVal = (dx > dy) ? static_cast<float>(currX - v1x) / static_cast<float>(v2x - v1x)
                                          : static_cast<float>(currY - v1y) / static_cast<float>(v2y - v1y);
                
                if (currX == scanline.leftX) {
                    lerp(v1.varying, v2.varying, lerpVal, scanline.leftVarying);
                    lerp(v1.position.z, v2.position.z, lerpVal, scanline.leftZ);
                }
                
                if (currX == scanline.rightX) {
                    lerp(v1.varying, v2.varying, lerpVal, scanline.rightVarying);
                    lerp(v1.position.z, v2.position.z, lerpVal, scanline.rightZ);
                }
                
                if (currX == v2x && currY == v2y) break;
                
                err2 = err;
                
                if (err2 > -dx) {
                    err -= dy;
                    currX += sx;
                }
                if (err2 < dy) {
                    err += dx;
                    currY += sy;
                }
            }
        }
    }

    // we have all of our scanlines setup, now just loop through shading each pixel in the scanline
    //*
    VARY_TYPE lerpedVarying;
    HACK_pixel pixelOutput;
    for (int i = bottomScanY; i != topScanY + 1; ++i) {
        const HACK_Scanline<VARY_TYPE> &scanline = scanlines[i];
        int pixelY = i;
        
        if (pixelY >= ctx.height || pixelY < 0) {
            continue;
        }
        
        // clip scanline to ctx space
        // TODO(karl): check against ctx x coords
        
        for (int j = scanline.leftX; j < scanline.rightX + 1; ++j) {
            int pixelX = j + halfWidth;
            if (pixelX >= ctx.width || pixelX < 0) {
                continue;
            }
            
            // lerp the left and right of the scanline into
            float dx = scanline.rightX - scanline.leftX;
            float lerpVal = (dx != 0) ? static_cast<float>(j - scanline.leftX) / static_cast<float>(scanline.rightX - scanline.leftX) : 0;
            lerp<VARY_TYPE>(scanline.leftVarying, scanline.rightVarying, lerpVal, lerpedVarying);
            float pixelZ = -1;
            lerp(scanline.leftZ, scanline.rightZ, lerpVal, pixelZ);
            
            
            shadeFragment(lerpedVarying, uniforms, pixelOutput);
            
            // update depth and color buffers with our rendering context
            // this is ARGB
            int pixelBase = (pixelX + (ctx.height - 1 - pixelY) * (ctx.width)) * 4;
            
            int r = _HACK_fast_floor(pixelOutput.color.r * 255);
            int g = _HACK_fast_floor(pixelOutput.color.g * 255);
            int b = _HACK_fast_floor(pixelOutput.color.b * 255);
            
            ctx.colorBuffer[pixelBase] = _HACK_fast_floor(pixelOutput.color.a * 255);
            ctx.colorBuffer[pixelBase + 1] = r;
            ctx.colorBuffer[pixelBase + 2] = g;
            ctx.colorBuffer[pixelBase + 3] = b;
        }
    }
    //*/
}

template <typename ATTR_TYPE, typename VARY_TYPE, typename UNIF_TYPE>
inline void __HACK_rasterize_wireframe_polygon(const HACK_Context<VARY_TYPE> &ctx,
                                               const HACK_vertex<VARY_TYPE> *vertices,
                                               const int vertexCount,
                                               const UNIF_TYPE &uniforms)
{
    
    int halfHeight = ctx.height / 2;
    int halfWidth = ctx.width / 2;
    
    VARY_TYPE lerpedVarying;
    HACK_pixel pixelOutput;
    // calculate pixel edges and shade inline
    for (int i = 0; i < vertexCount; ++i) {
        const HACK_vertex<VARY_TYPE> &v1 = vertices[i];
        const HACK_vertex<VARY_TYPE> &v2 = (i == vertexCount - 1) ? vertices[0] : vertices[i + 1];
        
        int v1x = _HACK_fast_ceil(v1.position.x * halfWidth);
        int v2x = _HACK_fast_ceil(v2.position.x * halfWidth);
        int v1y = _HACK_fast_ceil(v1.position.y * halfHeight);
        int v2y = _HACK_fast_ceil(v2.position.y * halfHeight);
        
        int dx = std::abs(v2x - v1x);
        int dy = std::abs(v2y - v1y);
        
        
        if (dy == 0) {
            // horizontal special case
            
            int sx = v1x < v2x ? 1 : -1;
            
            int pixelY = v1y + halfHeight;
            for (int currX = v1x; currX != v2x + sx; currX += sx) {
                int pixelX = currX + halfWidth;
                
                if (pixelX > -1 && pixelX < ctx.width && pixelY > -1 && pixelY < ctx.height) {
                    float lerpVal = static_cast<float>(currX - v1x) / static_cast<float>(v2x - v1x);
                    lerp(v1.varying, v2.varying, lerpVal, lerpedVarying);
                    
                    // shade and set pixel
                    shadeFragment(lerpedVarying, uniforms, pixelOutput);
                    
                    // update depth and color buffers with our rendering context
                    // this is ARGB
                    int pixelBase = (pixelX + (ctx.height - 1 - pixelY) * (ctx.width)) * 4;
                    ctx.colorBuffer[pixelBase] = _HACK_fast_floor(pixelOutput.color.a * 255);
                    ctx.colorBuffer[pixelBase + 1] = _HACK_fast_floor(pixelOutput.color.r * 255);
                    ctx.colorBuffer[pixelBase + 2] = _HACK_fast_floor(pixelOutput.color.g * 255);
                    ctx.colorBuffer[pixelBase + 3] = _HACK_fast_floor(pixelOutput.color.b * 255);
                }
            }
        } else if (dx == 0) {
            // vertical special case
            
            int sy = v1y < v2y ? 1 : -1;
            
            int pixelX = v1x + halfWidth;
            for (int currY = v1y; currY != v2y + sy; currY += sy) {
                int pixelY = currY + halfHeight;
                
                if (pixelX > -1 && pixelX < ctx.width && pixelY > -1 && pixelY < ctx.height) {
                    float lerpVal = static_cast<float>(currY - v1y) / static_cast<float>(v2y - v1y);
                    lerp(v1.varying, v2.varying, lerpVal, lerpedVarying);
                    
                    // shade and set pixel
                    shadeFragment(lerpedVarying, uniforms, pixelOutput);
                    
                    // update depth and color buffers with our rendering context
                    // this is ARGB
                    int pixelBase = (pixelX + (ctx.height - 1 - pixelY) * (ctx.width)) * 4;
                    ctx.colorBuffer[pixelBase] = _HACK_fast_floor(pixelOutput.color.a * 255);
                    ctx.colorBuffer[pixelBase + 1] = _HACK_fast_floor(pixelOutput.color.r * 255);
                    ctx.colorBuffer[pixelBase + 2] = _HACK_fast_floor(pixelOutput.color.g * 255);
                    ctx.colorBuffer[pixelBase + 3] = _HACK_fast_floor(pixelOutput.color.b * 255);
                }
            }
        } else {
            // general case
            
            int currX = v1x;
            int currY = v1y;
            
            int sx = v1x < v2x ? 1 : -1;
            int sy = v1y < v2y ? 1 : -1;
            
            int err = (dx > dy ? dx : -dy) / 2;
            int err2;
            
            for (;;) {
                int pixelY = currY + halfHeight;
                int pixelX = currX + halfWidth;
                
                if (pixelX > -1 && pixelX < ctx.width && pixelY > -1 && pixelY < ctx.height) {
                    float lerpVal = (dx > dy) ? static_cast<float>(currX - v1x) / static_cast<float>(v2x - v1x)
                                              : static_cast<float>(currY - v1y) / static_cast<float>(v2y - v1y);
                    lerp(v1.varying, v2.varying, lerpVal, lerpedVarying);
                    
                    // shade and set pixel
                    shadeFragment(lerpedVarying, uniforms, pixelOutput);
                    
                    // update depth and color buffers with our rendering context
                    // this is ARGB
                    int pixelBase = (pixelX + (ctx.height - 1 - pixelY) * (ctx.width)) * 4;
                    ctx.colorBuffer[pixelBase] = _HACK_fast_floor(pixelOutput.color.a * 255);
                    ctx.colorBuffer[pixelBase + 1] = _HACK_fast_floor(pixelOutput.color.r * 255);
                    ctx.colorBuffer[pixelBase + 2] = _HACK_fast_floor(pixelOutput.color.g * 255);
                    ctx.colorBuffer[pixelBase + 3] = _HACK_fast_floor(pixelOutput.color.b * 255);
                }
                
                if (currX == v2x && currY == v2y) break;
                
                err2 = err;
                
                if (err2 > -dx) {
                    err -= dy;
                    currX += sx;
                }
                if (err2 < dy) {
                    err += dx;
                    currY += sy;
                }
            }
        }
    }
}


#endif
