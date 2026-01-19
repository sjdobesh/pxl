#ifndef _PXL_SORT_H_
#define _PXL_SORT_H_
#include "pngz.h"

/* declare sort parameters */

typedef struct PXL_SortInfo {
  PNGZ_Image* img;                       /* image to sort */
  PNGZ_Image* img_original;              /* reference to original image */
  unsigned vertical_sort:1;              /* boolean for vert or horiz sort */
  float angle;                           /* angle to sort along (0-2 Pi) */
  float start_threshold, stop_threshold; /* controls what gets sorted */
  int random_start, random_stop;         /* random amount to dither threshold by */
  int min_length, max_length;            /* minimum and maximum sorted length */
  int (*comparison_function)(const void*, const void*); /* sort function */
  int (*threshold_function)(PNGZ_Pixel, float);         /* determines what to sort */
} PXL_SortInfo;

/* pixel comparison function */
int compare_brightness(const void* a, const void* b);
int threshold_brightness(PNGZ_Pixel p, float t);
/* int threshold_difference(pixel a, pixel b, float t); */

/* sort a pixel bufffer */
void sort(PNGZ_Pixel* p, int length, int (*compare)(const void*, const void*));

/* vert horiz sort */
int PXL_Sort(PXL_SortInfo);

/* debug print */
void print_sort_info(PXL_SortInfo si);

#endif /* _PIX_SORT_H_ */
