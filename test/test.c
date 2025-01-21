#include "../src/sort.h"
#include "../src/filter.h"
#include "../src/resize.h"
#include "pngz.h"

/* single pixel filter type */
pixel filter_foo(pixel p) {
  p.g = 0;
  p.b = 0;
  return p;
}
/* full image reference filter type */
pixel filter_image_foo(pngz* z, unsigned row, unsigned col) {
  return (pixel){
    .r = z->pixels[row][col].r,
    .g = 0,
    .b = 0,
    .a = 0xFF
  };
}

void art_script() {
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
  fuck_sort(si);
  pngz_save_as(z, "png/sortfuck.png");
  pngz_free(&z);
}

void filter_test() {
  pngz z;
  printf("\nfilter test---\n");
  pngz_load_from(&z, "png/Catgun.png");
  printf("single threaded filter...\n");
  pxl_filter(&z, filter_image_foo);
  pngz_save_as(z, "png/SINGLEFILTERTEST.png");
  printf("multi threaded filter...\n");
  pxl_filter_threaded(&z, filter_image_foo, 4);
  pngz_save_as(z, "png/MULTIFILTERTEST.png");
  pngz_free(&z);
}
void sort_test() {
  pngz z;
  printf("\nsort test---\n");
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

void resize_test() {
  pngz z;
  printf("\nresize test---\n");
  pngz_load_from(&z, "png/Catgun.png");
  pxl_resize(&z, 100, 100);
  pngz_save_as(z, "png/RESIZETEST.png");
  pngz_free(&z);
}

void crop_test() {
  pngz z;
  printf("\ncrop test---\n");
  pngz_load_from(&z, "png/Catgun.png");
  pxl_crop(&z, 100, 100, 50, 50);
  pngz_save_as(z, "png/CROPTEST.png");
  pngz_free(&z);
}

int main() {
  /*
  filter_test();
  sort_test();
  resize_test();
  crop_test();
  */
  art_script();
  return 0;
}
