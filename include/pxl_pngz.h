#ifndef _PXL_PNGZ_H_
#define _PXL_PNGZ_H_

#include <SDL3/SDL.h>
#include <pngz.h>

/* get */
Uint32 PXL_GetPixel(SDL_Surface *surface, int x, int y);
int PXL_GetPixelRGBA(SDL_Surface *surface, int x, int y, Uint8 *r, Uint8 *g, Uint8 *b, Uint8 *a);
PNGZ_Pixel PXL_GetPngzPixel(SDL_Surface *surface, int x, int y);

/* set */
int PXL_SetPixel(SDL_Surface *surface, int x, int y, Uint32 pixel_data);
int PXL_SetPixelRGBA(SDL_Surface *surface, int x, int y, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
int PXL_SetPngzPixel(SDL_Surface *surface, int x, int y, PNGZ_Pixel p);

/* interop */
int PXL_SurfaceToPngz(SDL_Surface *surface, PNGZ_Image* z);
int PXL_PngzToSurface(PNGZ_Image z, SDL_Surface *surface);

#endif /* !_PXL_PNGZ_H_ */
