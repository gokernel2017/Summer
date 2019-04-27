//-------------------------------------------------------------------
//
// SG: Summer Graphic ( GL / DirectX / HTML Canvas ):
//
// SG START DATA: 15/04/2019 - 17:40
//
//-------------------------------------------------------------------
//
#ifndef _SG_H_
#define _SG_H_

#ifdef __cplusplus
extern "C" {
#endif

//-----------------------------------------------
//------------------  INCLUDE  ------------------
//-----------------------------------------------
//
#include "config.h"

#ifdef USE_GL
    #include <SDL/SDL.h>
    #ifndef USE_SDL
        #include <SDL/SDL_opengl.h>
    #endif
    #ifdef WIN32
        #undef main
    #endif
#endif
#ifdef USE_DX
    #include <windows.h>
#endif

//-----------------------------------------------
//---------------  DEFINE / ENUM  ---------------
//-----------------------------------------------
//
#define LIBIMPORT     extern
#define BMP           SDL_Surface

typedef struct {
    void    *target;  // OBJECT or Pointer to "object"
    int     offsetX;  // mouse x
    int     offsetY;  // mouse y
    int     which;    // mouse button: 1 | 3
}TEvent;

//-----------------------------------------------
//-----------------  PUBLIC API  ----------------
//-----------------------------------------------
//
LIBIMPORT int   sgInit      (void);
//LIBIMPORT void  sgRun       (void (*call) (void));
LIBIMPORT void  sgRun       (void);
LIBIMPORT void  sgSetEvent  (void (*call) (TEvent *e), char *name);
LIBIMPORT void  sgDrawText  (char *text, int x, int y, int color);
LIBIMPORT void  sgDrawFloat (float f);
LIBIMPORT void  sgRect      (int x, int y, int w, int h, int color);
LIBIMPORT void  sgClear     (void);
LIBIMPORT void  sgBeginScene(void);
LIBIMPORT void  sgEndScene  (void);
LIBIMPORT void  sgSet2D     (void); // set 2D mode
LIBIMPORT void  sgSet3D     (void); // set 3D mode
LIBIMPORT void  opengl_make_font_8x13 (void);
LIBIMPORT void  opengl_draw_text (char *str, int x, int y, int color);

#ifndef USE_SDL
LIBIMPORT void draw_cube (GLenum type);
LIBIMPORT void draw_piso ();
#endif

#ifdef __cplusplus
}
#endif
#endif // ! _SW_H_

