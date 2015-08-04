# hack-rasterizer
Flexible software rasterizer

## Needs implemented
- backface culling - (easy) - calc normal of polygon, if negative discard
- z-buffer - (easy) - record per pixel z in buffer
- matrix types - (medium) - types and operators
- actually update raster :D - (easy) - record pixel in raster if z-buffer matches
- raster color space - (medium) convert float pixel amounts to integer amounts in color space
- polygon clipping for out of frame polygons - (easy) - just check as we scan convert

## Optimization points

Big wins:
- scan converter math (remove divisions and integer math) - (medium) - big thing is getting rid of divisions
- fragment shader invocation (maybe multithread this?) - (hard) - this is going to be the hardest part to optimize but will have the most win as is called per pixel per triangle, we should be able to multithread via batching inside of a single triangle because each pixel calc is independent

Smaller wins
- optimized lerping - (medium) - simd + assembly
