/* filter.c
 * filter module for pxl library
 * samantha jane
 *--------------------------------------------------------------------------80*/

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include "pngz.h"
#include "filter.h"



/**
 * casts f to void and passes in the addition filter type.
 * this allows pxl_filter_single to accept both function types.
 * acts as intermediary between pxl_filter macro and the
 * pxl_filter_single function.
 */
int pxl_filter_single_pixel(pngz* z, pixel_filter_func f) {
  return pxl_filter_single(z, *(void**)&f, PIXEL);
}

/**
 * casts f to void and passes in the addition filter type.
 * this allows pxl_filter_single to accept both function types.
 * acts as intermediary between pxl_filter macro and the
 * pxl_filter_single function.
 */
int pxl_filter_single_image(pngz* z, image_filter_func f) {
  return pxl_filter_single(z, *(void**)&f, IMAGE);
}

/**
 * pixel filter
 * single threaded.
 * maps a function onto every pixel, this function may freely accept
 * filter types, supported by the pxl_filter generic macro.
 * errors if pngz isn't loaded
 *
 * @param z pngz* simple png to filter
 * @param filter void(*)(unsigned char*), a filter function that take pixel ptr
 * @return exit code
 */
int pxl_filter_single(pngz* z, void* filter, filter_type ft) {
    /* make an intermediary and copy to new pixel buffer */
  pixel** output = pngz_alloc_pixels(z->rows, z->cols);
  /* filter; */
  for (unsigned row = 0; row < z->rows; row++) {
    for (unsigned col = 0; col < z->cols; col++) {
      if (ft == PIXEL)
        output[row][col] = (*(pixel_filter_func*)&filter)(z->pixels[row][col]);
      else
        output[row][col] = (*(image_filter_func*)&filter)(z, row, col);
    }
  }
  pngz_free_pixels(z->pixels, z->rows);
  z->pixels = output;
  return 0;
}


/* multithreaded filtering *--------------------------------------------------*/
pthread_barrier_t thread_barrier;

/**
 * thread functions that accesses rows of pixels.
 *
 * @param params thread parameter struct
 * @return void*, works via ptr reference
 */
void* pxl_filter_threadfn(void* params) {
  /* read in params */
  pxl_thread_parameters* tp = (pxl_thread_parameters*) params;
  /* apply the filter here for subsection of rows */
  for (unsigned i = tp->row_start; i < tp->row_stop; i++) {
    for (unsigned j = 0; j < tp->i_image->width; j++) {
      if (tp->f_type == PIXEL)
        tp->o_buf[i][j] = (*(pixel_filter_func*)&(tp->filter_function))(tp->i_image->pixels[i][j]);
      else
        tp->o_buf[i][j] = (*(image_filter_func*)&(tp->filter_function))(tp->i_image, i, j);
    }
  }
  /* halt threads */
  pthread_barrier_wait(&thread_barrier);
  return NULL;
}


void pxl_filter_threaded_pixel(pngz* z, pixel_filter_func f, unsigned tc) {
  pxl_filter_thread_dispatcher(z, *(void**)&f, tc, PIXEL);
}

void pxl_filter_threaded_image(pngz* z, image_filter_func f, unsigned tc) {
  pxl_filter_thread_dispatcher(z, *(void**)&f, tc, IMAGE);
}

/**
 * dispatches multithreaded filter.
 *
 * @param s pngz reference to filter
 * @param filter some filter function taking and returning a pixel
 * @return exit code, setting errno on failure
 */
int pxl_filter_thread_dispatcher(pngz* z, void* filter, unsigned thread_count, filter_type ft) {

  /* make an empty pixel buffer */
  pixel** output = pngz_alloc_pixels(z->rows, z->cols);

  /* thread containers and barrier */
  pthread_t* threads = malloc(sizeof(pthread_t) * thread_count);
  pxl_thread_parameters* tps = malloc(sizeof(pxl_thread_parameters) * thread_count);

  /* set barrier */
  pthread_barrier_init(&thread_barrier, NULL, thread_count);

  /* calc how much to do */
  unsigned divisions = (z->height / thread_count) + 1;

  /* launch threads */
  for (unsigned i = 0; i < thread_count; i++) {
    /* initialize thread params here */
    tps[i] = (pxl_thread_parameters) {
      .i_image = z,
      .o_buf = output,
      .f_type = ft,
      .filter_function = filter,
      .row_start = i * divisions,
      .row_stop = (i + 1) * divisions > z->height ? z->height : (i+1) * divisions,
      .thread_id = i
    };
    if (pthread_create(&threads[i], NULL, pxl_filter_threadfn, (void*)&tps[i])) {
      fprintf(stderr, "[ERROR] unable to create thread %d\n", i);
    }
  }

  /* clean up threads */
  for (unsigned i = 0; i < thread_count; i++) {
    pthread_join(threads[i], NULL);
  }
  pthread_barrier_destroy(&thread_barrier);
  free(threads);
  free(tps);

  /* free input and swap */
  pngz_free_pixels(z->pixels, z->rows);
  z->pixels = output;
  return 0;
}

/* resize an image */
//int pxl_resize(pngz* z, unsigned rows, unsigned cols, resize_function rf) {
//   return 0;
// }

