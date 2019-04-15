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
    #include <SDL/SDL_opengl.h>
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
LIBIMPORT int   sgInit      (int argc, char **argv);
LIBIMPORT void  sgRun       (void (*call) (void));
LIBIMPORT void  sgSetEvent  (void (*call) (TEvent *e), char *name);
LIBIMPORT void  sgDrawText  (char *text, int x, int y, int color);
LIBIMPORT void  sgRect      (int x, int y, int w, int h, int color);
LIBIMPORT void  sgClear     (void);
LIBIMPORT void  sgRender    (void);
#ifdef __cplusplus
}
#endif
#endif // ! _SW_H_

