/**-------------------------------------*
 * @file pxl_context.h                  |
 * SDL app context and state management |
 *--------------------------------------*-----------------------------------80*/

#ifndef _PXL_CONTEXT_H_
#define _PXL_CONTEXT_H_

/* libraries, macros, & globals *---------------------------------------------*/

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "pxl_sort.h"

#define SCREEN_WIDTH  900
#define SCREEN_HEIGHT 900
#define MENU_LENGTH 7
#define MAX_STRING_LENGTH 200
#define FONT_SIZE 12

extern const char* MENU_FORMAT[MENU_LENGTH];
extern char MENU[MENU_LENGTH][MAX_STRING_LENGTH];
extern const SDL_Color WHITE;
extern const SDL_Color BLACK;
extern const SDL_Color RED;

/* typedefs *-----------------------------------------------------------------*/

/* update as few things as possible each frame for performance */
typedef enum {
  NONE          = 0,
  SCREEN_UPDATE = 1,
  MENU_UPDATE   = 1 << 1,
} PXL_UPDATE_FLAGS;

/**
 * context contains references to all necessary
 * SDL components and sort algorithm parameters
 */
typedef struct PXL_Context {

  /* core */
  SDL_Window*   win;            /** open window reference */
  SDL_Renderer* renderer;       /** renderer for window */
  SDL_Surface*  screen_surface; /** cpu pixel data */
  SDL_Texture*  screen_texture; /** gpu pixel data */

  /* sort info */
  PXL_SortInfo* sort_info; /** ptr to sorting data */
  const char* path;        /** file path */

  /* menu */
  SDL_Surface*    text_surface;      /** surface for text rendering (we just need one) */
  SDL_Texture*    text_texture;      /** text textures */
  TTF_TextEngine* text_engine;
  TTF_Font*       default_font;      /** default font is BigBlueTerm */
  TTF_Text*       text[MENU_LENGTH]; /* text objects for each menu item */
  const bool*     kbstate;           /** keyboard */
  int             menu_selection;    /** menu index selected */

  /* flags */
  PXL_UPDATE_FLAGS update; /** enum update type*/
  int              render; /** set once context is modified and off when rendered */


} PXL_Context;

/* init and allocate *--------------------------------------------------------*/

/* alloc & free context resources */
int PXL_Init(PXL_Context* context);
int PXL_Quit(PXL_Context* context);

/* initialization for SDL components */
int PXL_CreateSDLResources(PXL_Context* context);
  int PXL_InitSDLLibs();
  int PXL_CreateWindowRenderer(PXL_Context* context);
  int PXL_CreateFontEngine(PXL_Context* context);

/* load surfaces and textures for screen and menu */
int PXL_CreateContextResources(PXL_Context* context);
  /* surfaces */
  int PXL_CreateSurfaceFromImg(SDL_Surface** surface, const char* path);
  int PXL_Create8BitSurface(SDL_Surface** surface, int w, int h);
  /* textures */
  int PXL_CreateTextureFromSurface(
        SDL_Renderer* renderer,
        SDL_Surface* surface,
        SDL_Texture** texture
      );
  int PXL_CreateScreen(PXL_Context* context);
  /* menu resources */
  int PXL_CreateMenu(PXL_Context* context);
    int PXL_CreateMenuText(PXL_Context* context);
    int PXL_CreateMenuTextItem(PXL_Context* context, int menu_index);

/* update functions *---------------------------------------------------------*/

/* update sort info data to screen */
int PXL_Update(PXL_Context* context);
  int PXL_UpdateTextureFromSurface(SDL_Surface* surface, SDL_Texture** texture);
  int PXL_UpdateScreenTexture(PXL_Context* context);
  int PXL_UpdateMenuTexture(PXL_Context* context);
  int PXL_UpdateMenuSurface(PXL_Context* context);
  int PXL_UpdateMenuText(PXL_Context* context);
  int PXL_UpdateMenuTextItem(PXL_Context* context, int menu_index);
  int PXL_UpdateMenuValues(PXL_SortInfo* si);

/* input */
int PXL_HandleEvents(PXL_Context* context, SDL_Event* event);
int PXL_HandleKeyboardState(PXL_Context* context);

/* render */
int PXL_Render(PXL_Context* context);
  int PXL_RenderText(PXL_Context* context);


#endif /* !_PXL_CONTEXT_H_ */
