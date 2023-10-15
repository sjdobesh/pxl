/* pxl.h
 * samantha jane
 * simple png
 *--------------------------------------------------------------------------80*/

#ifndef _PXL_H_
#define _PXL_H_

#include "pngz.h"

typedef enum {
  CONVOLUTION,
  PIXEL,
} filter_type;

typedef enum {
  NEAREST_NEIGHBOR,
  BILINEAR,
  BICUBIC,
} resize_function;

/**
 * thread parameters for multithreaded image filtering
 */
typedef struct pxl_thread_parameters {
  pngz* i_image; /* reference to the image */
  pixel** o_buf; /* reference to the output */
  filter_type f_type;
  union {
    pixel (*filter_function_pixel) (pixel);
    pixel (*filter_function_image) (pngz, unsigned, unsigned);
  unsigned long row_start, row_stop;
  int thread_id;
} thread_parameters;

/* prototypes */
/* filter functions */
/* maybe we can consolidate thread fns? with a union perhaps */
/* or use generics to accept multiple types of filter functions (single pixel, or pixel with reference */

void* pxl_filter_threadfn(void* params);
int pxl_filter_threaded(pngz* z, pixel(*filter)(pixel), unsigned thread_count);
int pxl_filter(pngz* z, pixel(*filter)(pixel));

void* pxl_convolution_threadfn(void* params);
int pxl_convolution_threaded(pngz* s, pixel(*convolution)(spng s, unsigned row, unsigned col, unsigned thread_count);
int pxl_convolution(pngz* z, pixel(*convolution)(pngz z, unsigned row, unsigned col);

/* resizing functions */
// void* pxl_resize_threadfn(void* params);
// int pxl_resize_threaded(pngz* z, unsigned rows, unsigned cols, resize_function rf, unsigned thread_count);
// int pxl_resize(pngz* z, unsigned rows, unsigned cols, resize_function rf);

#endif
