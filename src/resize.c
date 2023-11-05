#include "resize.h"

int resize(pngz* z, unsigned rows, unsigned cols){
  z->rows = rows;
  z->cols = cols;
  return 0;
}

int crop(pngz* z, unsigned rows, unsigned cols){
  z->rows = rows;
  z->cols = cols;
  return 0;
}
