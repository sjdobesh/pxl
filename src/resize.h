#ifndef _RESIZE_H_
#define _RESIZE_H_
#include "pngz.h"
int resize(pngz* z, unsigned rows, unsigned cols);
int crop(pngz* z, unsigned rows, unsigned cols);
#endif
