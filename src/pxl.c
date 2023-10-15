/* pxl.c
 * samantha jane
 *--------------------------------------------------------------------------80*/

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include "pngz.h"
#include "pxl.h"


/**
 * pixel filter.
 * maps a function onto every pixel.
 * errors if pngz isn't loaded
 *
 * @param z pngz* simple png to filter
 * @param filter void(*)(unsigned char*), a filter function that take pixel ptr
 * @return exit code
 */
int pxl_filter(pngz* z, pixel(*filter)(pixel)) {
  if (!z->pixels) {
    fprintf(stderr, "ERROR > filtering failed due to no loaded image.\n");
    errno = EPERM;
    return 1;
  }
  /* make an intermediary and copy to new pixel buffer */
  pixel** output = pngz_alloc_pixels(z->rows, z->cols);
  for (unsigned row = 0; row < z->rows; row++) {
    for (unsigned col = 0; col < z->cols; col++) {
      output[row][col] = filter(z->pixels[row][col]);
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
void* pngz_filter_threadfn(void* params) {
  /* read in params */
  struct thread_parameters* tp = (thread_parameters*) params;
  /* apply the filter here for subsection of rows */
  for (unsigned i = tp->row_start; i < tp->row_stop; i++) {
    for (unsigned j = 0; j < tp->i_image->width; j++) {
      tp->o_buf[i][j] = tp->filter_function(tp->i_image->pixels[i][j]);
    }
  }
  /* halt threads */
  pthread_barrier_wait(&thread_barrier);
  return NULL;
}

/**
 * dispatches multithreaded filter.
 *
 * @param s pngz reference to filter
 * @param filter some filter function taking and returning a pixel
 * @return exit code, setting errno on failure
 */
int pxl_filter_threaded(pngz* z, pixel (*filter)(pixel), unsigned thread_count) {

  /* make an empty pixel buffer */
  pixel** output = pngz_alloc_pixels(z->rows, z->cols);

  /* thread containers and barrier */
  pthread_t* threads = malloc(sizeof(pthread_t) * thread_count);
  thread_parameters* tps = malloc(sizeof(thread_parameters) * thread_count);

  /* set barrier */
  pthread_barrier_init(&thread_barrier, NULL, thread_count);

  /* calc how much to do */
  unsigned divisions = z->height / thread_count;

  /* launch threads */
  for (unsigned i = 0; i < thread_count; i++) {
    /* initialize thread params here */
    tps[i] = (thread_parameters) {
      .i_image = z,
      .o_buf = output,
      .filter_function = filter,
      .row_start = i * divisions,
      .row_stop = (i + 1) * divisions > z->height ? z->height : (i+1) * divisions,
      .thread_id = i
    };
    if (pthread_create(&threads[i], NULL, pngz_filter_threadfn, (void*)&tps[i])) {
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
int pngz_resize(pngz* z, unsigned rows, unsigned cols, resize_function rf) {
  /* determine if scaling up or down */
  /* determine algorithm to use */
  /* can we multi thread this process? (yes) */
  return 0;
}

