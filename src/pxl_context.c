/**-------------------------------------*
 * @file pxl_context.c                  |
 * SDL app context (PXL_Context)        |
 * and state management                 |
 *--------------------------------------*-----------------------------------80*/

/* includes & globals *-------------------------------------------------------*/

/* pxl_context header already included:
 * - <SDL3/SDL.h>
 * - <SDL3_image/SDL_image.h>
 * - <SDL3_ttf/SDL_ttf.h>
 */
#include "pxl_context.h"

/* all defined as extern in sdl_context.h */
const char* MENU_FORMAT[] = {
  "min length [%d] ",
  "max length [%d] ",
  "random start [%d] ",
  "random stop  [%d] ",
  "start threshold [%0.3f] ",
  "stop threshold  [%0.3f] ",
  "vertical [%d]"
};
const SDL_Color WHITE = {255, 255, 255};
const SDL_Color BLACK = {0, 0, 0, 255};
const SDL_Color RED =   {255, 0, 0};
char MENU[MENU_LENGTH][MAX_STRING_LENGTH] = {0};

/* SDL resources *------------------------------------------------------------*/
/* libraries
 * window
 * screen renderer
 * font renderer
 */


/**
 * initialize all SDL libraries.
 * currently uses:
 * - SDL
 * - TTF
 *
 * @return exit code (0 on success, 1 on error)
 */
int PXL_InitSDLLibs() {
  if ( ! SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
    fprintf(stderr, "ERROR > failed SDL init.\n");
    return 1;
  }
  if ( ! TTF_Init()) {
    fprintf(stderr, "ERROR > failed SDL_ttf init.\n");
    return 1;
  }

  return 0;
}

/**
 * initialize default window and renderer settings.
 *
 * @param[out] context ptr to allocated PXL_Context struct.
 * modifies the following member variables:
 * - /c context->win: initializes window to macros SCREEN_WIDTH X SCREEN_HEIGHT
 * - /c context->renderer: initializes renderer with SDL_BLENDMODE_NONE
 * @return exit code (0 on success, 1 on error)
 */
int PXL_CreateWindowRenderer(PXL_Context* context) {
  /* create window and renderer */
  if ( ! (SDL_CreateWindowAndRenderer(
            "pxl",
            SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_HIGH_PIXEL_DENSITY,
            &(context->win), &(context->renderer)
         )
  )) {
    fprintf(stderr, "ERROR > creating window renderer.\n%s\n", SDL_GetError());
    return 1;
  }
  /* default blending options will be available in SDL 3.4 (apt has 3.2) */
  SDL_SetRenderDrawBlendMode(context->renderer, SDL_BLENDMODE_NONE);
  return 0;

}

/**
 * initialize text processing by opening font creating surface text engine
 *
 * @param[out] context ptr to an allocated context struct.
 * modifies the following:
 * - \c context->default_font: set to BigBlue_TerminalPlus font ptr
 * - \c context->text_engine: set to surface text engine
 * @return exit code (0 on success, 1 on error)
 */
int PXL_CreateFontEngine(PXL_Context* context) {
  if ( ! (context->default_font =
            TTF_OpenFont("fonts/BigBlue_TerminalPlus.TTF", FONT_SIZE)
  )) {
    fprintf(stderr, "ERROR > couldn't find default font.\n");
    return 1;
  }
  if ( ! (context->text_engine = TTF_CreateSurfaceTextEngine())) {
    fprintf(stderr, "ERROR > creating text engine.\n%s\n", SDL_GetError());
    return 1;

  }
  return 0;
}

/**
 *  create rendering dependencies
 *  - init SDL Libraries
 *  - create default window renderer and font renderer (text engine)
 *
 * does not allocate memory for ptr!
 * expects preallocated ptr (static or malloc)
 *
 * @param[out] context ptr to PXL context variables and state to init
 * @return exit code (0 on success, 1 on error)
 */
int PXL_CreateSDLResources(PXL_Context* context) {

  if (PXL_InitSDLLibs()) return 1;

  if (PXL_CreateWindowRenderer(context)) return 1;

  if (PXL_CreateFontEngine(context)) return 1;

  return 0;
}
/* surface & texture functions *----------------------------------------------*/

/**
 * create an 8 bit RGB surface with SDL_BLENDMODE_NONE
 *
 * @param[out] surface
 * @param w width
 * @param h height
 * @return exit code (0 on success, 1 on error)
 */
int PXL_Create8BitSurface(SDL_Surface** surface, int w, int h) {
  if ( ! (*surface = SDL_CreateSurface(w, h, SDL_PIXELFORMAT_XRGB8888))) {
    fprintf(stderr, "ERROR > creating 8bit surface.\n%s\n", SDL_GetError());
    return 1;
  };
  SDL_SetSurfaceBlendMode(*surface, SDL_BLENDMODE_NONE);
  return 0;
}

/**
 * creates surface from image on disk.
 * surface is set to SDL_BLENDMODE_NONE
 *
 * @param surface[out] surface to initialize and fill with img data
 * @param path img file on disk
 * @return exit code (0 on success, 1 on error)
 */
int PXL_CreateSurfaceFromImg(SDL_Surface** surface, const char* path) {
  if ( ! (*surface = IMG_Load(path))) {
    fprintf(
      stderr, "ERROR > loading %s to screen surface .\n%s\n",
      path, SDL_GetError()
    );
    return 1;
  }
  SDL_SetSurfaceBlendMode(*surface, SDL_BLENDMODE_NONE);
  return 0;
}

/**
 * creates texture from surface with streaming access.
 *
 * @param renderer ptr to an initialized context renderer
 * @param surface ptr to an initialized surface with img to laod
 * @param texture[out] ptr to texture created from surface
 * @return exit code (0 on success, 1 on error)
 */
int PXL_CreateTextureFromSurface(
      SDL_Renderer* renderer, SDL_Surface* surface, SDL_Texture** texture
) {
  if ( ! (*texture =
             SDL_CreateTexture(
               renderer,
               surface->format,
               SDL_TEXTUREACCESS_STREAMING,
               surface->w,
               surface->h
             )
  )) {
    fprintf(stderr,"ERROR > creating texture.\n%s\n",SDL_GetError());
    return 1;
  }
  SDL_SetTextureScaleMode(*texture, SDL_SCALEMODE_NEAREST);
  SDL_SetTextureBlendMode(*texture, SDL_BLENDMODE_NONE);
  return 0;
}

/**
 * load a context surface from a path and then
 * load a matching texture surface with streaming access.
 * both are set to SDL_BLENDMODE_NONE and the surface is SDL_SCALEMODE_NEAREST
 *
 * @param[in,out] context ptr to initialized render with path to disk set
 * reads:
 * - /c context->path: path to image on disk
 * modifies:
 * - /c context->screen_surface: initialized with image data
 * - /c context->screen_texture: initialized to match surface format
 * @return exit code (0 on success, 1 on error)
 */
int PXL_CreateScreen(PXL_Context* context) {

  if (PXL_CreateSurfaceFromImg(&(context->screen_surface), context->path))
    return 1;

  if (PXL_CreateTextureFromSurface(
        context->renderer,
        context->screen_surface,
        &(context->screen_texture)
  ))
    return 1;

  return 0;
}

/**
 * create menu text objects for a specific index
 */
int PXL_CreateMenuTextItem(PXL_Context* context, int menu_index) {
  if ( ! (context->text[menu_index] =
            TTF_CreateText(
              context->text_engine,
              context->default_font,
              MENU[menu_index], sizeof(MENU[menu_index])
            )
  )) {
    fprintf(stderr,"ERROR > creating menu text object.\n%s\n",SDL_GetError());
    return 1;
  }
  return 0;
}

/**
 * create all menu items and text surfaces
 *
 * @param[in,out] context
 * reads:
 * - /c context->text_engine: surface type
 * - /c context->default_font: font for text engine
 * modifies:
 * - /c context->text: text object array
 */
int PXL_CreateMenuText(PXL_Context* context) {
  int exit = 0;
  for (int i = 0; i < MENU_LENGTH; i++)
    if (PXL_CreateMenuTextItem(context, i)) exit = 1;
  return exit;
}

/**
 * formats data from sort info to load as text
 * make a surface object from screen to store
 * text seperate but not alpha.
 */
int PXL_CreateMenu(PXL_Context* context) {

  if (PXL_Create8BitSurface(
        &(context->text_surface),
        context->screen_surface->w,
        context->screen_surface->h
      )) return 1;

  if (PXL_CreateTextureFromSurface(
        context->renderer,
        context->text_surface,
        &(context->text_texture)
      )) return 1;

  if (PXL_CreateMenuText(context)) return 1;

  return 0;
}

int PXL_CreateContextResources(PXL_Context* context) {

  if (PXL_CreateScreen(context))
    return 1;
  else {
    context->render = 1;
    context->update |= MENU_UPDATE;
  }

  if (PXL_CreateMenu(context))
    return 1;
  else {
    context->render = 1;
    context->update &= ~MENU_UPDATE;
  }

  return 0;

}

/**
 * load bare minimum for sdl (a window and renderer)
 * then make the rest of the context resources (textures, surfaces, text)
 */
int PXL_Init(PXL_Context* context) {

  if (PXL_CreateSDLResources(context)) return 1;

  if (PXL_CreateContextResources(context)) return 1;

  return 0;

}

/**
 * read menu data from sort info struct into global menu array
 *
 * @param[in] si initialized sorting info for menu
 * - reads in all contents into global MENU array
 * @return exit code (0 on success, 1 on error)
 */
int PXL_UpdateMenuValues(PXL_SortInfo* si) {
  memset(MENU, 0, sizeof(MENU));
  snprintf(MENU[0], sizeof(MENU[0]), MENU_FORMAT[0], si->min_length);
  snprintf(MENU[1], sizeof(MENU[1]), MENU_FORMAT[1], si->max_length);
  snprintf(MENU[2], sizeof(MENU[2]), MENU_FORMAT[2], si->random_start);
  snprintf(MENU[3], sizeof(MENU[3]), MENU_FORMAT[3], si->random_stop);
  snprintf(MENU[4], sizeof(MENU[4]), MENU_FORMAT[4], si->start_threshold);
  snprintf(MENU[5], sizeof(MENU[5]), MENU_FORMAT[5], si->stop_threshold);
  snprintf(MENU[6], sizeof(MENU[6]), MENU_FORMAT[6], si->vertical_sort);
  return 0;
}


int PXL_UpdateTextureFromSurface(SDL_Surface* surface, SDL_Texture** texture) {
  /* update pixel values */
  if (! SDL_UpdateTexture(
          *texture,
          NULL,
          surface->pixels,
          surface->pitch
        )
  ) {
    fprintf(stderr, "ERROR > updating texture.\n%s\n", SDL_GetError());
    return 1;
  }

  return 0;
}

/**
 * update a contexts texture from its surface
 *
 * @param context with texture and surface components
 * @return exit code
 */
int PXL_UpdateScreenTexture(PXL_Context* context) {
  return PXL_UpdateTextureFromSurface(
           context->screen_surface,
           &(context->screen_texture)
         );
}

/**
 * update a contexts text from its surface
 *
 * @param context with text texture and surface components
 * @return exit code
 */
int PXL_UpdateMenuTextItem(PXL_Context* context, int menu_index) {
  /* regenerate text object i*/
  TTF_DestroyText(context->text[menu_index]);
  if ( ! (context->text[menu_index] =
            TTF_CreateText(
              context->text_engine, context->default_font,
              MENU[menu_index], sizeof(MENU[menu_index])
            )
  )) {
    return 1;
  }
  return 0;

}

int PXL_UpdateMenuText(PXL_Context* context) {
  int exit = 0;
  /* update values and rerender */
  PXL_UpdateMenuValues(context->sort_info);
  for (int i = 0; i < MENU_LENGTH; i++)
    if (PXL_UpdateMenuTextItem(context, i))
      exit = 1;
  return exit;
}

int PXL_UpdateMenuSurface(PXL_Context* context) {
  SDL_ClearSurface(context->text_surface, 0, 0, 0, 0xFF);
  PXL_UpdateMenuText(context);
  for (int i = 0; i < MENU_LENGTH; i++) {
    if (i == context->menu_selection) {
      TTF_SetTextColor(context->text[i], 0xFF, 0, 0, 0xFF);
      TTF_DrawSurfaceText(context->text[i], 0, 0, context->text_surface);
      TTF_SetTextColor(context->text[i], 0xFF, 0xFF, 0xFF, 0xFF);
    } else {
      TTF_DrawSurfaceText(context->text[i], 0, 0, context->text_surface);
    }
  }
  return 0;
}

int PXL_UpdateMenuTexture(PXL_Context* context) {
  PXL_UpdateMenuSurface(context);
  PXL_UpdateTextureFromSurface(context->text_surface, &(context->screen_texture));
  return 0;
}

/**
 * update a everything in a context
 * screen and menu textures
 *
 * @param context with texture and surface components
 * @return exit code
 */
int PXL_Update(PXL_Context* context) {

  /* skip if no flags set */
  if ( ! (context->update)) return 0;

  if (context->update & SCREEN_UPDATE) {
    PXL_UpdateScreenTexture(context);
    context->update &= ~SCREEN_UPDATE;
    context->render = 1;
  }

  if (context->update & MENU_UPDATE) {
    PXL_UpdateMenuTexture(context);
    context->update &= ~MENU_UPDATE;
    context->render = 1;
  }

  return 0;
}

/** handles highspeed continuous keyboard input */
int PXL_HandleKeyboardState(PXL_Context* context) {
  if (context->kbstate[SDL_SCANCODE_J]) {
    switch(context->menu_selection) {
      case 0:
        context->sort_info->min_length++;
        break;
      case 1:
        context->sort_info->max_length++;
        break;
    }
    context->update |= SCREEN_UPDATE | MENU_UPDATE;
    context->render = 1;
    return SDL_APP_CONTINUE;
  }
  if (context->kbstate[SDL_SCANCODE_K]) {
    switch(context->menu_selection) {
      case 0:
        context->sort_info->min_length--;
        break;
      case 1:
        context->sort_info->max_length--;
        break;
    }
    context->update |= SCREEN_UPDATE | MENU_UPDATE;
    context->render = 1;
    return SDL_APP_CONTINUE;
  }
  if (context->kbstate[SDL_SCANCODE_Q]) {
    return SDL_APP_SUCCESS;
  }
  return SDL_APP_CONTINUE;
}

/**
 * handles inputs to adjust sort info
 */
int PXL_HandleEvents(PXL_Context* context, SDL_Event* event) {
  if (event->type == SDL_EVENT_QUIT)
    return SDL_APP_SUCCESS;
  context->kbstate = SDL_GetKeyboardState(NULL);
  if (context->menu_selection > 0 && context->kbstate[SDL_SCANCODE_H]) {
    context->menu_selection--;
  }
  if (context->menu_selection < MENU_LENGTH - 1 && context->kbstate[SDL_SCANCODE_L]) {
    context->menu_selection++;
  }
  return SDL_APP_CONTINUE;
}

/**
 * render text portion to screen
 */
int PXL_RenderText(PXL_Context* context) {

  int w = 0, h = 0, offset = 0;

  /* clear text surface and texture*/
  SDL_ClearSurface(context->text_surface, 0, 0, 0, 0xFF);

  for (int i = 0; i < MENU_LENGTH; i++) {
    TTF_DrawSurfaceText(context->text[i], offset, 0, context->text_surface);
    TTF_GetTextSize(context->text[i], &w, &h);
    offset += w;
  }

  SDL_FRect src  = { .w=SCREEN_WIDTH, .h=h };
  SDL_FRect dest = { .w=SCREEN_WIDTH, .h=h*2 };

  PXL_UpdateTextureFromSurface(context->text_surface, &(context->text_texture));
  SDL_RenderTexture(context->renderer, context->text_texture, &src , &dest);

  return 0;
}
/**
 * clear and refresh contexts renderer
 */
int PXL_Render(PXL_Context* context) {

  /* skip if render flag not set */
  if ( ! (context->render)) return 0;

  /* clear and redraw screen */
  SDL_RenderClear(context->renderer);

  SDL_RenderTexture(context->renderer, context->screen_texture, NULL, NULL);

  PXL_RenderText(context);

  SDL_RenderPresent(context->renderer);

  return 0;
}

/**
 * free resources and quit
 */
int PXL_Quit(PXL_Context* context) {

  /* free screen stuff  */
  SDL_DestroySurface(context->screen_surface);
  SDL_DestroyTexture(context->screen_texture);

  /* free text */
  TTF_DestroySurfaceTextEngine(context->text_engine);
  SDL_DestroySurface(context->text_surface);
  SDL_DestroyTexture(context->text_texture);
  for (int i = 0; i < MENU_LENGTH; i++) {
    TTF_DestroyText(context->text[i]);
  }

  /* free pngz stuff */
  PNGZ_Free(context->sort_info->img);
  PNGZ_Free(context->sort_info->img_original);

  /* quit libraries */
  TTF_Quit();
  SDL_Quit();

  return 0;
}
