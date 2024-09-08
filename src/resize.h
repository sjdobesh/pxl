#ifndef _RESIZE_H_
#define _RESIZE_H_
#include "pngz.h"
int pxl_resize(pngz* z, unsigned rows, unsigned cols);
int pxl_crop(pngz* z, unsigned rows, unsigned cols, unsigned row_start, unsigned col_start);
#endif
