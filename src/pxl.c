/**---------------------------------------------------*
 * @file pxl.c                                        |
 * driver program with SDL Main call back definitions |
 *----------------------------------------------------*-----------------------*/
#include <assert.h>
#include "pxl_context.h"
#include "pxl_pngz.h"
#include "pxl_sort.h"
#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL_main.h>

#define INPATH "./png/valtest.png"
#define OUTPATH "./png/valtest.png"
#define UNUSED_VAR(x) \
  do {                \
    (void)(x);        \
  } while (0);

/* init */
SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[]) {
  /* since its a callback, don't alter the function declaration */
  /* todo: parse paths from args */
  UNUSED_VAR(argc)
  UNUSED_VAR(argv)

  /* load pictures and sort info into context */
  static PNGZ_Image z1;
  static PNGZ_Image z2;
  PNGZ_LoadFrom(&z1, INPATH);
  PNGZ_LoadFrom(&z2, INPATH);
  PNGZ_SaveAs(z1, "png/test1.png");
  PNGZ_SaveAs(z2, "png/test2.png");

  /**
   * no malloc, we only need a fixed number
   * (1 context, 1 sort_info, 2 images (original and sorted) */
  static PXL_SortInfo si = {.img = &z1,
                            .img_original = &z2,
                            .vertical_sort = 0,
                            .start_threshold = 0,
                            .stop_threshold = 0,
                            .min_length = 1,
                            .max_length = 100,
                            .random_start = 0,
                            .random_stop = 0,
                            .comparison_function = compare_brightness,
                            .threshold_function = threshold_brightness};
  static PXL_Context context = {
      .path = INPATH,
      .sort_info = &si}; /* everything is contained within the context */

  /* load sdl and render first frame */
  PXL_Init(&context);
  PXL_Update(&context);
  PXL_Render(&context);

  *appstate = &context;
  return SDL_APP_CONTINUE;
}

/* main loop */
SDL_AppResult SDL_AppIterate(void* appstate) {
  PXL_Context* context = (PXL_Context*)appstate;

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
SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {
  return PXL_HandleEvents((PXL_Context*)appstate, event);
}

/* clean up (called on SDL_APP_SUCCESS / SDL_APP_QUIT) */
void SDL_AppQuit(void* appstate, SDL_AppResult result) {
  /* since its a callback, don't alter the function declaration */
  UNUSED_VAR(result)
  PXL_Context* context = (PXL_Context*)appstate;
  PNGZ_SaveAs(*(context->sort_info->img), OUTPATH);
  PXL_Quit(context);  // this handles pngz cleanup as well
  return;
}
