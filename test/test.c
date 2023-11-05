#include "../src/sort.h"
#include "../src/filter.h"
#include "pngz.h"

pixel filter_foo(pixel p) {
  p.g = 0;
  p.b = 0;
  return p;
}
pixel filter_image_foo(pngz* z, unsigned row, unsigned col) {
  return (pixel){
    .r = z->pixels[row][col].r,
    .g = 0,
    .b = 0,
    .a = 0xFF
  };
}

void filter_test() {
  pngz z;
  pngz_load_from(&z, "png/Catgun.png");
  pxl_filter(&z, filter_image_foo);
  pngz_save_as(z, "png/FILTERTEST.png");
  pngz_free(&z);
}
void sort_test() {
  pngz z;
  pngz_load_from(&z, "png/Catgun.png");
  sort_info si = {
    .img_ref = &z,
    .vertical_sort = 1,
    .start_threshold = 0.0001,
    .stop_threshold = 0.05,
    .min_length = 100,
    .max_length = 100,
    .random_start = 100,
    .random_stop = 100,
    .comparison_function = compare_brightness,
    .threshold_function = threshold_brightness
  };
  orthogonal_sort(si);
  pngz_save_as(z, "png/SORTTEST.png");
  pngz_free(&z);
}

int main() {
  filter_test();
  sort_test();
}
