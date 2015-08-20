# hack-rasterizer
Flexible software rasterizer

## Needs implemented
- z-buffer - (medium) - record per pixel z in buffer
- matrix types - (medium) - types and operators
- raster color space - (medium) convert float pixel amounts to integer amounts in color space
- texturing - (hard) - need texture representation and sampling
- vector operators - (medium) - basic dot products, normalizations, etc...

## Optimization points

Big wins:
- optimized lerping - (medium) - calc step and use addition rather than calcing the step every frame
