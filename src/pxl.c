/**---------------------------------------------------*
 * @file pxl.c                                        |
 * driver program with SDL Main call back definitions |
 *----------------------------------------------------*-----------------------*/
#include <pngz.h>
#define SDL_MAIN_USE_CALLBACKS 1
#include "pxl_context.h"
#include "pxl_pngz.h"
#include "pxl_sort.h"
#include <SDL3/SDL_main.h>
#include <stdio.h>
/*#include <stdlib.h>*/

#define DEBUG_PRINT 0
#define DEBUG_LEVEL 0
#define INPATH "./png/val.png"
#define OUTPATH "./png/valtest.png"

/* init */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {

  /**
   * we dont have to malloc since we only need a fixed number
   * (1 context, 1 sort_info, 2 images (original and sorted) */
  static PXL_Context context = {.path = INPATH};
  static PXL_SortInfo si = {.vertical_sort = 0,
                            .start_threshold = 0,
                            .stop_threshold = 0,
                            .min_length = 1,
                            .max_length = 100,
                            .random_start = 0,
                            .random_stop = 0,
                            .comparison_function = compare_brightness,
                            .threshold_function = threshold_brightness};
  static PNGZ_Image z1;
  static PNGZ_Image z2;

  /* local variables to summarize function results for debugging */
  int z1_load_success, z2_load_success, pxl_init_success, pxl_update_success,
      pxl_render_success;

  /* load pictures and sort info into context */
  z1_load_success = PNGZ_LoadFrom(&z1, INPATH);
  z2_load_success = PNGZ_LoadFrom(&z2, INPATH);
  PNGZ_SaveAs(z1, "png/test1.png");
  PNGZ_SaveAs(z2, "png/test2.png");
  si.img = &z1;
  si.img_original = &z2;
  context.sort_info = &si; /* everything is contained within the context */

  /* load sdl and render */
  pxl_init_success = PXL_Init(&context);
  pxl_update_success = PXL_Update(&context);
  pxl_render_success = PXL_Render(&context);

  if (pxl_init_success || pxl_update_success || pxl_render_success) {
    fprintf(stderr,
            "SDL_AppInit summary\n"
            " - load image buffers: [z1 %d] [z2 %d]\n"
            " - initial pxl calls: [init %d] [update %d] [render %d]\n",
            z1_load_success, z2_load_success, pxl_init_success,
            pxl_update_success, pxl_render_success);
  }

  *appstate = &context;
  return SDL_APP_CONTINUE;
}

/* main loop */
SDL_AppResult SDL_AppIterate(void *appstate) {

  int copy_exit, sort_exit, pngz2surface_exit, pxl_update_exit, pxl_render_exit;

  PXL_Context *context = (PXL_Context *)appstate;

  /* refresh and resort */
  copy_exit =
      PNGZ_Copy(*(context->sort_info->img_original), context->sort_info->img);
  sort_exit = PXL_Sort(*(context->sort_info));

  /* render */
  pngz2surface_exit =
      PXL_PngzToSurface(*(context->sort_info->img), context->screen_surface);
  pxl_update_exit = PXL_Update(context);
  pxl_render_exit = PXL_Render(context);

  if (copy_exit || sort_exit || pngz2surface_exit || pxl_update_exit ||
      pxl_render_exit) {
    fprintf(stderr, "PXL_ERROR > something went wrong in SDL_AppIterate\n");
    fprintf(stderr, "PNGZ_Copy : %s\n", (copy_exit ? "FAIL" : "SUCCESS"));
    fprintf(stderr, "PXL_Sort : %s\n", (sort_exit ? "FAIL" : "SUCCESS"));
    fprintf(stderr, "PXL_PngzToSurface : %s\n",
            (pngz2surface_exit ? "FAIL" : "SUCCESS"));
    fprintf(stderr, "PXL_Update : %s\n",
            (pxl_update_exit ? "FAIL" : "SUCCESS"));
    fprintf(stderr, "PXL_Render : %s\n",
            (pxl_render_exit ? "FAIL" : "SUCCESS"));
    return SDL_APP_FAILURE;
  }

  /* handle user input or quit. Keyboard state is faster than app events */
  return PXL_HandleKeyboardState(context);
}

/* event handler */
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
  return PXL_HandleEvents((PXL_Context *)appstate, event);
}

/* clean up (called on SDL_APP_SUCCESS / SDL_APP_QUIT) */
void SDL_AppQuit(void *appstate, SDL_AppResult result) {
  PXL_Context *context = (PXL_Context *)appstate;
  PNGZ_SaveAs(*(context->sort_info->img), OUTPATH);
  PXL_Quit(context); // this handles pngz cleanup as well
  return;
}
