/* filter.h
 * for pxl library
 * samantha jane
 *--------------------------------------------------------------------------80*/

#ifndef _FILTER_H_
#define _FILTER_H_

#include "pngz.h"

typedef enum {
  IMAGE,
  PIXEL,
} filter_type;

typedef enum {
  NEAREST_NEIGHBOR,
  BILINEAR,
  BICUBIC,
} resize_function;

/* two styles of filter function inputs */
typedef pixel(*pixel_filter_func)(pixel);
typedef pixel(*image_filter_func)(pngz*, unsigned, unsigned);

/**
 * thread parameters for multithreaded image filtering
 */
typedef struct pxl_thread_parameters {
  pngz* i_image; /* reference to the image */
  pixel** o_buf; /* reference to the output */
  filter_type f_type;
  void* filter_function;
  unsigned long row_start, row_stop;
  int thread_id;
} pxl_thread_parameters;

/**
 * pixel filter generic macro that determines which reference style
 * the filter argument uses, either with whole image reference or
 * with a single pixel.
 *
 * image style reference:
 *   pixel(*filter)(pngz* z, unsigned row, unsigned col)
 *
 * pixel style reference:
 *   pixel(*filter)(pngz* z, pixel p)
 *
 * @param z pngz* to input image that will become output
 * @param f filter function, either image or pixel style reference
 */
#define pxl_filter(z, f) _Generic((f), \
  pixel(*)(pngz*, unsigned, unsigned): pxl_filter_single_image, \
  pixel(*)(pixel): pxl_filter_single_pixel \
)(z, f)

#define pxl_filter_threaded(z, f, t) _Generic ((f),\
    pixel(*)(pixel): pxl_filter_threaded_pixel, \
    pixel(*)(pngz*, unsigned, unsigned): pxl_filter_threaded_image\
)(z, f, t)

/* prototypes */
/* filter functions */
/* maybe we can consolidate thread fns? with a union perhaps */
/* or use generics to accept multiple types of filter functions (single pixel, or pixel with reference */


void* pxl_filter_threadfn(void* params);
int pxl_filter_thread_dispatcher(pngz* z, void* filter, unsigned thread_count, filter_type f_type);
int pxl_filter_single_pixel(pngz* z, pixel(*filter)(pixel));
int pxl_filter_single_image(pngz* z, pixel(*filter)(pngz*, unsigned, unsigned));
int pxl_filter_single(pngz* z, void* f, filter_type frt);
void pxl_filter_threaded_pixel(pngz* z, pixel_filter_func f, unsigned tc);
void pxl_filter_threaded_image(pngz* z, image_filter_func f, unsigned tc);

/* resizing functions */
// void* pxl_resize_threadfn(void* params);
// int pxl_resize_threaded(pngz* z, unsigned rows, unsigned cols, resize_function rf, unsigned thread_count);
// int pxl_resize(pngz* z, unsigned rows, unsigned cols, resize_function rf);

#endif
