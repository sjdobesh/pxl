#include "resize.h"

/**
 * unsigned map function
 *
 * @param imin input minimum
 * @param imax input maximum
 * @param omin output minimum
 * @param omax output maximum
 * @param input value to map between ranges
 * @return the mapped value
 */
unsigned umap(float imin, float imax, float omin, float omax, float input) {
  return (unsigned)((((input - imin)*(omax - omin))/(imax - imin)) + omin);\
}

/**
 * scale an image up or down
 *
 * @param z pngz* to scale up or down
 * @param rows new row size to scale up or down to
 * @param cols new col size to scale up or down to
 */
int pxl_resize(pngz* z, unsigned rows, unsigned cols){
  pixel** output = pngz_alloc_pixels(rows, cols);

  for (unsigned row = 0; row < rows; row++)
    for (unsigned col = 0; col < cols; col++)
      output[row][col] = z->pixels[umap(0, z->rows, 0, rows, row)][umap(0, z->cols, 0, cols, col)];
      // [0, x] - [0, y] = (y / x) * t

  pngz_free_pixels(z->pixels, z->rows);
  z->pixels = output;
  z->rows = rows;
  z->cols = cols;
  return 0;
}

/**
 * crops an image to a certain size from a given top left corner position
 *
 * @param z pngz* containing image reference to crop
 * @param rows new generated crop height
 * @param cols new generated crop width
 * @param row_start top left corner of crop
 * @param col_start top left corner of crop
 * @return error code
 */
int pxl_crop(pngz* z, unsigned rows, unsigned cols, unsigned row_start, unsigned col_start){

  pixel** output = pngz_alloc_pixels(rows, cols);

  for (unsigned row = row_start; row < rows; row++)
    for (unsigned col = col_start; col < cols; col++)
      if (row < z->rows && col < z->cols) /* guard rail */
        output[row][col] = z->pixels[row][col];

  pngz_free_pixels(z->pixels, z->rows);
  z->pixels = output;
  z->rows = rows;
  z->cols = cols;
  return 0;
}
