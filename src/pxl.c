/**---------------------------------------------------*
 * @file pxl.c                                        |
 * driver program with SDL Main call back definitions |
 *----------------------------*-----------------------------------------------*/
#define SDL_MAIN_USE_CALLBACKS 1

#include <SDL3/SDL_main.h>
#include "pxl_context.h"
#include "pxl_pngz.h"
#include "pxl_sort.h"

#define INPATH  "png/val.png"
#define OUTPATH "png/valtest.png"

/* init */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {

  /* static variables */
  /* we dont have to malloc since we only need a fixed number
   * (1 context, 1 sort_info, 2 images) */
  static PXL_Context context = {
    .path = INPATH
  };
  static PXL_SortInfo si = {
    .vertical_sort = 0,
    .start_threshold = 0,
    .stop_threshold = 0,
    .min_length = 1,
    .max_length = 100,
    .random_start = 0,
    .random_stop = 0,
    .comparison_function = compare_brightness,
    .threshold_function = threshold_brightness
  };
  static PNGZ_Image z1, z2;

  /* load pictures and sort info into context */
  PNGZ_LoadFrom(&z1, INPATH);
  PNGZ_LoadFrom(&z2, INPATH);
  si.img = &z1;
  si.img_original = &z2;
  context.sort_info = &si; /* everything is contained within the context */

  /* load sdl and render */
  PXL_Init(&context);
  PXL_Update(&context);
  PXL_Render(&context);

  *appstate = &context;
  return SDL_APP_CONTINUE;
}

/* main loop */
SDL_AppResult SDL_AppIterate(void *appstate) {

  PXL_Context* context = (PXL_Context*) appstate;

  /* refresh and resort */
  PNGZ_Copy(*(context->sort_info->img_original), context->sort_info->img);
  PXL_Sort(*(context->sort_info));

  /* render */
  PXL_PngzToSurface(*(context->sort_info->img), context->screen_surface);
  PXL_Update(context);
  PXL_Render(context);

  /* handle user input or quit. Keyboard state is faster than app events */
  return PXL_HandleKeyboardState(context);

}

/* event handler */
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
  return PXL_HandleEvents((PXL_Context*)appstate, event);
}

/* clean up (called on SDL_APP_SUCCESS / SDL_APP_QUIT) */
void SDL_AppQuit(void *appstate, SDL_AppResult result) {
  PXL_Context* context = (PXL_Context*) appstate;
  PNGZ_SaveAs(*(context->sort_info->img), OUTPATH);
  PNGZ_Free(context->sort_info->img);
  PNGZ_Free(context->sort_info->img_original);
  PXL_Quit(context);
  return;
}
