#ifndef HACK_H_
#define HACK_H_

#include <alloca.h>

/**
 * Our rendering context
 */
struct HACK_Context
{};

/**
 * Output of a fragment shader, 
 * contains the color and z depth of our pixel
 */
struct HACK_pixel {
    float r, g, b, a, z;
};

/**
 * Output of a vertex shader
 * contains the vertex position and an associated varying object with our vertex
 */
template <typename VARY_TYPE>
struct HACK_vertex {
    float x, y, z;
    VARY_TYPE varying;
};

/**
 * Scanline representation
 * Holds left/right positions and associated varying objects
 */
template <typename VARY_TYPE>
struct __HACK_Scanline {
    int leftX, rightX;
    float leftZ, rightZ;
    VARY_TYPE leftVaryings, rightVaryings;
};

/**
 * lerp function template
 * Anything that is used as a VARY_TYPE must have lerp implemented for it
 */
template <typename T>
void lerp(const T &v1, const T &v2, float lerp, T &output);

/**
 * Rasterize a set of triangles
 * polygonAttributes - <ATTR_TYPE>[] - this array holds all of the per vertex information for all of our triangles
 * uniforms - <UNIF_TYPE> - this object holds things that are uniform to all vertices in all of our triangles
 * vertexCount - int - the number of vertices, should be the length of polygonAttributes
 * vertexShader - fn(const ATTR_TYPE & attribute, const UNIF_TYPE & uniform, HACK_vertex<VARY_TYPE> & output)
 *              - function that transforms the triangles' vertices and sets up the varying data for further pipeline steps
 *              - your shader function should populate the output parameter
 * fragmentShader - fn(const VARY_TYPE & varying, const UNIF_TYPE & uniform, HACK_pixel & output)
 *                - function that determines the color of a pixel based on varyings and uniforms
 *                - your shader function should populate the output parameter
 */
template <typename ATTR_TYPE, typename VARY_TYPE, typename UNIF_TYPE>
inline void HACK_rasterize_triangles(const HACK_Context &ctx,
                    const ATTR_TYPE *polygonAttributes,
                    const UNIF_TYPE &uniforms,
                    const int vertexCount,
                    void (*vertexShader) (const ATTR_TYPE &attribute, const UNIF_TYPE &uniform, HACK_vertex<VARY_TYPE> &output),
                    void (*fragmentShader) (const VARY_TYPE &varying, const UNIF_TYPE &uniform, HACK_pixel &output))
{
    // every three vertexes is a triangle we should rasterize
    for (int v = 0; v < vertexCount;) {
        // do this in a new function so we can get fancy auto stack allocation semantics
        // this could be slow :/ not sure at the moment...
        __HACK_rasterize_triangle(ctx, v, polygonAttributes, uniforms, vertexShader, fragmentShader);
        v += 3;
    }
}

/**
 * INTERNAL - Rasterize a single triangle
 * triangleId - int - the ID of the triangle we're rendering, this is used to calc which polygon attributes we will use for our vertexes
 * polygonAttributes - <ATTR_TYPE>[] -
 * uniforms - <UNIF_TYPE> -
 * vertexShader - fn
 * fragmentShader - fn
 */
template <typename ATTR_TYPE, typename VARY_TYPE, typename UNIF_TYPE>
inline void __HACK_rasterize_triangle(const HACK_Context &ctx,
                                    const int triangleId,
                                    const ATTR_TYPE *polygonAttributes,
                                    const UNIF_TYPE &uniforms,
                                    void (*vertexShader) (const ATTR_TYPE &attribute, const UNIF_TYPE &uniform, HACK_vertex<VARY_TYPE> &output),
                                    void (*fragmentShader) (const VARY_TYPE &varying, const UNIF_TYPE &uniform, HACK_pixel &output))
{
    // allocate 3 outputs, one for each vertex
    HACK_vertex<VARY_TYPE> vertexShaderOutput[3];
    vertexShader(polygonAttributes[triangleId], uniforms, vertexShaderOutput[0]);
    vertexShader(polygonAttributes[triangleId + 1], uniforms, vertexShaderOutput[1]);
    vertexShader(polygonAttributes[triangleId + 2], uniforms, vertexShaderOutput[2]);
    
    // calc polygon normal and short circuit if needed
    
    // calculate number of scanlines needed for triangle
    // TODO(karl): calc tri y coords
    // TODO(karl): clip to ctx y coords
    int bottomScanY, topScanY;
    int scanlineNum = 5; // topScanY - bottomScanY;
    
    // alloc the scanline memory from the stack because we don't know how many scanlines we'll need per tri
    __HACK_Scanline<VARY_TYPE> *scanlines = (__HACK_Scanline<VARY_TYPE> *) alloca(sizeof(__HACK_Scanline<VARY_TYPE>) * scanlineNum);
    
    // populate scanlines with values
    // this is where actual scanline conversion is done
    // TODO(karl): actually do it
    
    

    // we have all of our scanlines setup, now just loop through shading each pixel in the scanline
    VARY_TYPE lerpedVarying;
    HACK_pixel pixelOutput;
    for (int i = 0; i < scanlineNum; ++i) {
        __HACK_Scanline<VARY_TYPE> scanline = scanlines[i];
        
        // we wouldn't really do this, this is for testing that lerping is actually happening
        // in real code these would already be set for us during the scanline conversion process
        scanline.leftX = 0;
        scanline.rightX = 5;
        
        // clip scanline to ctx space
        // TODO(karl): check against ctx x coords
        // TODO(karl): maybe check flag to see if we should respect pixel z value changes? if not do depth buffer optimization here
        
        for (int j = scanline.leftX; j <= scanline.rightX; ++j) {
            // lerp the left and right of the scanline into
            float lerpVal = (float)(j - scanline.leftX) / (float)(scanline.rightX - scanline.leftX);
            lerp<VARY_TYPE>(vertexShaderOutput[0].varying, vertexShaderOutput[1].varying, lerpVal, lerpedVarying);
            
            fragmentShader(lerpedVarying, uniforms, pixelOutput);
            
            // update depth and color buffers with our rendering context
            
        }
    }
}


#endif
