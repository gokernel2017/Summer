//-------------------------------------------------------------------
//
// SG: Summer Graphic ( GL / DirectX / HTML Canvas ):
//
// SG START DATA: 15/04/2019 - 17:40
//
//-------------------------------------------------------------------
//
#include "sg.h"

static const unsigned char fixed_font[14][764] = {
  "                                   xx                                                                                                                                                                                                                                                                                                                                                                                                                                                                              xx             xxx                                                                                                                                                                                                                                                      ",
  "                                   xx    xxx                                                                                                                                                                                                                                                                                                                                                                                                                                                                      xxxx             xx                                                                      xx       xx                                                                                                                                                                     ",
  "           xx    xx  xx   xx xx   xxxx  xx xx     xxx      xx       xx    xx                                                 xx    xxxx     xx    xxxx    xxxx    xx     xxxxxx    xxx   xxxxxx   xxxx    xxxx                       xx          xx       xxxx   xxxxxx    xx    xxxxx    xxxx   xxxx    xxxxxx  xxxxxx   xxxx   xx  xx   xxxx       xx  xx  xx  xx      xx   xx xx   xx  xxxx   xxxxx    xxxx   xxxxx    xxxx   xxxxxx  xx  xx  xx  xx  xx   xx xx  xx  xx  xx  xxxxxx   xxxx   xx       xxxx   xx  xx             xx           xx                  xx            xxxx          xx        xx       xx   xx      xxxx                                                             xx                                                        xx     xx     xx     xxx   x   ",
  "          xxxx   xx  xx   xx xx  xx  xx xx xx x  xx xx     xx      xx      xx                                                xx   xx  xx   xxx   xx  xx  xx  xx   xx     xx        xx        xx  xx  xx  xx  xx                     xx            xx     xx  xx xx    xx  xxxx   xx  xx  xx  xx  xx xx   xx      xx      xx  xx  xx  xx    xx        xx  xx  xx  xx      xx   xx xx   xx xx  xx  xx  xx  xx  xx  xx  xx  xx  xx    xx    xx  xx  xx  xx  xx   xx xx  xx  xx  xx      xx   xx     xx         xx                                   xx                  xx           xx             xx                      xx        xx                                                             xx                                                       xx      xx      xx   xx xx xx   ",
  "          xxxx   xx  xx  xxxxxxx xx      xxx xx  xx xx     xx      xx      xx     xx xx    xx                               xx    xx xxx xxxxx   xx  xx  xx  xx   xx xx  xx       xx        xx   xx  xx  xx  xx    xxx     xxx     xx              xx    xx  xx xx    xx xx  xx  xx  xx  xx  xx  xx  xx  xx      xx      xx  xx  xx  xx    xx        xx  xx xx   xx      xxx xxx xxx  xx xx  xx  xx  xx  xx  xx  xx  xx  xx        xx    xx  xx  xx  xx  xx   xx  xx x   xx  xx      xx   xx      xx        xx                            xxxx   xxxxx    xxxx    xxxxx   xxxx    xx      xxxxx  xxxxx   xxxx     xxxx   xx  xx    xx    xxxxxx  xxxxx    xxxx   xxxxx    xxxxx  xx  xx   xxxxx  xxxxxx  xx  xx  xx  xx  xx   xx xx  xx  xx  xx  xxxxxx    xx      xx      xx   x   xxx    ",
  "          xxxx            xx xx   xx        xx    xxx             xx        xx     xxx     xx                               xx    xx xxx    xx       xx      xx   xx xx  xx      xxxxx      xx   xxx xx  xx  xx    xxx     xxx    xx     xxxxxx     xx      xx  xx  xxxx xx  xx  xx  xx  xx      xx  xx  xx      xx      xx      xx  xx    xx        xx  xx xx   xx      xx x xx xxxx xx xx  xx  xx  xx  xx  xx  xx  xx   xx       xx    xx  xx  xx  xx  xx x xx   xx    xx  xx     xx    xx      xx        xx                               xx  xx  xx  xx  xx  xx  xx  xx  xx   xx     xx  xx  xx  xx    xx       xx   xx  xx    xx    xx x xx xx  xx  xx  xx  xx  xx  xx  xx  xx xxx  xx       xx     xx  xx  xx  xx  xx x xx xx  xx  xx  xx      xx    xx      xx      xx              ",
  "           xx             xx xx    xx      xx    xx               xx        xx   xxxxxxx xxxxxx          xxxxxx            xx     xx  xx    xx      xx     xxx    xx xx  xxxxx   xx  xx    xx     xxxx   xx  xx                  xx                  xx    xx   xx xx xx xx  xx  xxxxx   xx      xx  xx  xxxxx   xxxxx   xx      xxxxxx    xx        xx  xxxx    xx      xx x xx xx xxxx xx  xx  xxxxx   xx  xx  xxxxx     xx      xx    xx  xx  xx  xx  xx x xx   xx     xxxx     xx     xx       xx       xx                               xx  xx  xx  xx      xx  xx  xx  xx  xxxxxx  xx  xx  xx  xx    xx       xx   xx xx     xx    xx x xx xx  xx  xx  xx  xx  xx  xx  xx  xxx     xx       xx     xx  xx  xx  xx  xx x xx  xxxx   xx  xx     xx    xx       xx       xx             ",
  "           xx             xx xx     xx    xx     xx xxxx          xx        xx     xxx     xx                              xx     xxx xx    xx     xx        xx  xx  xx      xx  xx  xx    xx    xx xxx   xxxxx                   xx     xxxxxx     xx     xx   xx xx xx xxxxxx  xx  xx  xx      xx  xx  xx      xx      xx xxx  xx  xx    xx        xx  xx xx   xx      xx x xx xx  xxx xx  xx  xx      xx  xx  xx xx      xx     xx    xx  xx  xx  xx  xx x xx  x xx     xx     xx      xx       xx       xx                            xxxxx  xx  xx  xx      xx  xx  xxxxxx   xx     xx  xx  xx  xx    xx       xx   xxxx      xx    xx x xx xx  xx  xx  xx  xx  xx  xx  xx  xx       xxxx    xx     xx  xx  xx  xx  xx x xx   xx    xx  xx    xx    xx        xx        xx            ",
  "                         xxxxxxx     xx  xx xxx  xx  xx           xx        xx    xx xx    xx                             xx      xxx xx    xx    xx     xx  xx  xxxxxxx     xx  xx  xx   xx     xx  xx     xx                     xx              xx           xx  xxxx xx  xx  xx  xx  xx  xx  xx  xx  xx      xx      xx  xx  xx  xx    xx    xx  xx  xx xx   xx      xx   xx xx   xx xx  xx  xx      xx  xx  xx  xx      xx    xx    xx  xx  xx  xx   xx xx  xx  xx    xx    xx       xx        xx      xx                           xx  xx  xx  xx  xx      xx  xx  xx       xx     xx  xx  xx  xx    xx       xx   xx xx     xx    xx x xx xx  xx  xx  xx  xx  xx  xx  xx  xx          xx   xx     xx  xx  xx  xx  xx x xx  xxxx   xx  xx   xx      xx       xx       xx             ",
  "           xx             xx xx  xx  xx  x xx xx xx  xx           xx        xx                     xxx             xxx    xx      xx  xx    xx   xx      xx  xx      xx     xx   xx  xx   xx     xx  xx    xx      xxx     xxx      xx            xx       xx   xx       xx  xx  xx  xx  xx  xx  xx xx   xx      xx      xx  xx  xx  xx    xx    xx  xx  xx  xx  xx      xx   xx xx   xx xx  xx  xx      xx  xx  xx  xx  xx  xx    xx    xx  xx   xxxx    xx xx  xx  xx    xx    xx       xx        xx      xx                           xx  xx  xx  xx  xx  xx  xx  xx  xx       xx     xx  xx  xx  xx    xx       xx   xx  xx    xx    xx x xx xx  xx  xx  xx  xx  xx  xx  xx  xx          xx   xx     xx  xx   xxxx    xx xx  xx  xx  xx  xx  xx        xx      xx      xx              ",
  "           xx             xx xx   xxxx     xx xx  xxx xx           xx      xx                      xxx             xxx   xx        xxxx     xx   xxxxxx   xxxx       xx  xxxx     xxxx    xx      xxxx    xxx      xxx     xxx       xx          xx        xx    xxxxxxx xx  xx  xxxxx    xxxx   xxxx    xxxxxx  xx       xxxxx  xx  xx   xxxx    xxxx   xx  xx  xxxxxx  xx   xx xx   xx  xxxx   xx       xxxx   xx  xx   xxxx     xx     xxxx     xx     xx xx  xx  xx    xx    xxxxxx   xx         xx     xx                            xxxxx  xxxxx    xxxx    xxxxx   xxxx    xx      xxxxx  xx  xx  xxxxxx     xx   xx  xx  xxxxxx  xx   xx xx  xx   xxxx   xxxxx    xxxxx  xx      xxxxx     xxxx   xxxxx    xx     xx xx  xx  xx   xxxx   xxxxxx    xx      xx      xx              ",
  "                                   xx       xxx                    xx      xx                       xx                   xx                                                                                                 xx                                                                                                                                                                              xx                                                                            xx         xx     xx                                                                               xx                     xx                                           xx          xx                                                             xx             xx      xx      xx              ",
  "                                   xx                               xx    xx                       xx                                                                                                                      xx                                                                                                                                                                                xx                                                                           xx                xx                                                                               xx                     xx                                           xx          xx                                                            xx               xx     xx     xx               ",
  "                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          xxxx            xxxx          xxxxxxxx                                                         xxxxx                   xxxx                                            xx          xx                                                         xxxx                       xx                      ",
};

typedef struct {
    void  (*onmouseup)    (TEvent *event);
    void  (*onmousedown)  (TEvent *event);
    void  (*onmousemove)  (TEvent *event);
}EVENT;

SDL_Surface *screen;
static EVENT func_event;
static TEvent _ev_;

static int
    quit,
    running
    ;

void _call_ (void) {
    SDL_Delay (10);
}
void (*CallBack) (void) = _call_;

void DrawPixel (BMP *bmp, int x, int y, int color) {
    int bpp;
    Uint8 *p;

    // Get clip
    if(
        x < bmp->clip_rect.x ||
        x > bmp->clip_rect.x + bmp->clip_rect.w ||
        y < bmp->clip_rect.y ||
        y >= bmp->clip_rect.y + bmp->clip_rect.h
    )
  return;

    bpp = bmp->format->BytesPerPixel;

    // Here p is the address to the pixel we want to set
    p = (Uint8 *)bmp->pixels + y * bmp->pitch + x * bpp;

    if (bpp==2) { *(Uint16 *)p = color; return; }
    if (bpp==4) *(Uint32 *)p = color;

}// DrawPixel()


// 8 x 13
void DrawChar (BMP *bmp, char ch, int x, int y, int color) {

  if (ch > 32) {
    register unsigned char count;
    register int xx;

      xx = (ch - SDLK_SPACE) * 8;

      // insert color
      for (count=0; count < 8; count++) {
          if ( fixed_font[ 0][xx] == 'x' ) { DrawPixel(bmp, x, y+0,  color); }
          if ( fixed_font[ 1][xx] == 'x' ) { DrawPixel(bmp, x, y+1,  color); }
          if ( fixed_font[ 2][xx] == 'x' ) { DrawPixel(bmp, x, y+2,  color); }
          if ( fixed_font[ 3][xx] == 'x' ) { DrawPixel(bmp, x, y+3,  color); }
          if ( fixed_font[ 4][xx] == 'x' ) { DrawPixel(bmp, x, y+4,  color); }
          if ( fixed_font[ 5][xx] == 'x' ) { DrawPixel(bmp, x, y+5,  color); }
          if ( fixed_font[ 6][xx] == 'x' ) { DrawPixel(bmp, x, y+6,  color); }
          if ( fixed_font[ 7][xx] == 'x' ) { DrawPixel(bmp, x, y+7,  color); }
          if ( fixed_font[ 8][xx] == 'x' ) { DrawPixel(bmp, x, y+8,  color); }
          if ( fixed_font[ 9][xx] == 'x' ) { DrawPixel(bmp, x, y+9,  color); }
          if ( fixed_font[10][xx] == 'x' ) { DrawPixel(bmp, x, y+10, color); }
          if ( fixed_font[11][xx] == 'x' ) { DrawPixel(bmp, x, y+11, color); }
          if ( fixed_font[12][xx] == 'x' ) { DrawPixel(bmp, x, y+12, color); }
          if ( fixed_font[13][xx] == 'x' ) { DrawPixel(bmp, x, y+13, color); }
          xx++; x++;
      }
  }
}

void sgDrawText (char *text, int x, int y, int color) {
#ifdef USE_GL
  #ifdef USE_SDL
  BMP *bmp = screen;
  while (*text) {
    if(*text > 32)
      DrawChar (bmp, *text, x, y, color);
    text++;
    x += 8;
  }
  #else
  opengl_draw_text (text, x, y, color);
  #endif
#endif
}

void sgDrawFloat (float f) {
    char buf[50];
    sprintf(buf, "FLOAT: %f", f);
    sgDrawText (buf, 10, 10, 64512);//COLOR_ORAMGE);
}

void sgRect (int x, int y, int w, int h, int color) {
    SDL_FillRect (screen, &(struct SDL_Rect){ x, y, w, h }, color);
}

int sgInit (int argc, char **argv) {
    int w = 800, h = 600, flag = 0;
    static int init = 0;

    if (init) return 1;
    init = 1;

#ifdef USE_GL
    SDL_Init (SDL_INIT_VIDEO);
    #ifdef WIN32
    SDL_putenv ("SDL_VIDEO_CENTERED=center");
    #endif
    SDL_WM_SetCaption ("SG: Summer Graphic (GL/DirectX/HTML Canvas) | To Exit Press ESC !", NULL);

    // SDL / OPENGL
    #ifndef USE_SDL
    flag = SDL_OPENGL;
    #endif

    screen = SDL_SetVideoMode (w, h, 16, flag);
    #ifndef USE_SDL
    opengl_make_font_8x13 ();
    #endif

    atexit (SDL_Quit);
#endif // ! USE_GL

    return 1;

}// sgInit ();

void sgEvent (void) {
    SDL_Event ev;

    while (SDL_PollEvent(&ev)) {
    switch (ev.type) {
    case SDL_MOUSEMOTION:
        _ev_.offsetX = ev.motion.x; // mouse_x
        _ev_.offsetY = ev.motion.y; // mouse y
        if (func_event.onmousemove)
            func_event.onmousemove(&_ev_);
        break;

    case SDL_MOUSEBUTTONDOWN:
        if (func_event.onmousedown)
            func_event.onmousedown(&_ev_);
        break;

    case SDL_MOUSEBUTTONUP:
        if (func_event.onmouseup)
            func_event.onmouseup(&_ev_);
        break;

    case SDL_KEYDOWN:
        if (ev.key.keysym.sym == SDLK_ESCAPE) {
            quit = 1;
        }
        break;
    }// switch (ev.type)
    }// while (SDL_PollEvent(&ev))
}

void sgRun (void (*call) (void)) {

    if (call)
        CallBack = call;

    quit = 0;
    while (!quit) {
        sgEvent();
        CallBack();
    }
}

void sgSetEvent (void (*call) (TEvent *e), char *name) {
    if (name) {
        if (!strcmp(name, "onmouseup")) func_event.onmouseup = call;
        if (!strcmp(name, "onmousedown")) func_event.onmousedown = call;
        if (!strcmp(name, "onmousemove")) func_event.onmousemove = call;
    }
}

void sgBeginScene (void) {
#ifdef USE_GL
  #ifdef USE_SDL
  SDL_FillRect (screen, &(struct SDL_Rect){ 0,0, screen->w, screen->h }, 0);
  #else
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // GL_COLOR_BUFFER_BIT);
  #endif
#endif
}

void sgEndScene (void) {
#ifdef USE_GL
  #ifdef USE_SDL
  SDL_UpdateRect (screen, 0, 0, screen->w, screen->h);
  //SDL_Flip(screen);
  #else
  SDL_GL_SwapBuffers();
  #endif
#endif
}

void sgClear (void) {
#ifdef USE_GL
  #ifdef USE_SDL
  SDL_FillRect (screen, &(struct SDL_Rect){ 0,0, screen->w, screen->h }, 0);
  #else
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // GL_COLOR_BUFFER_BIT);
  #endif
#endif
}

// This set the projection mode 3D.
void sgSet3D (void) {
#ifdef USE_GL
  #ifndef USE_SDL
    glEnable (GL_DEPTH_TEST);
//    glEnable ( GL_CULL_FACE );

    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    gluPerspective (60.0, ((GLdouble)800)/((GLdouble)600), 1.0, 1024.0);
  #endif
#endif
}

// This set the projection mode 2D.
void sgSet2D (void) {
#ifdef USE_GL
  #ifndef USE_SDL
  glDisable (GL_DEPTH_TEST);
//    glDisable ( GL_CULL_FACE );

//  glMatrixMode (GL_PROJECTION);
  glLoadIdentity ();
  glOrtho (0.0, 800, 600, 0.0, 0.0, 1.0);
  #endif
#endif
}

// This set the projection mode 2D.
void set_2D (void) {
#ifdef USE_GL
  #ifndef USE_SDL
    glDisable (GL_DEPTH_TEST);
//    glDisable ( GL_CULL_FACE );

//    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    glOrtho (0.0, 800, 600, 0.0, 0.0, 1.0);
  #endif
#endif
}

// Silicon Graphics, Inc.
//
// glut implementation:
// FILE:
//   glut_shapes.c
//   glutWireCube(GLdouble size);
//
#ifdef USE_GL
#ifndef USE_SDL
void draw_cube (GLenum type) {
  static GLfloat n[6][3] = {
    {-1.0, 0.0, 0.0},
    {0.0, 1.0, 0.0},
    {1.0, 0.0, 0.0},
    {0.0, -1.0, 0.0},
    {0.0, 0.0, 1.0},
    {0.0, 0.0, -1.0}
  };
  static GLint faces[6][4] = {
    {0, 1, 2, 3},
    {3, 2, 6, 7},
    {7, 6, 5, 4},
    {4, 5, 1, 0},
    {5, 6, 2, 1},
    {7, 4, 0, 3}
  };
  GLfloat v[8][3];
  GLint i;

  v[0][0] = v[1][0] = v[2][0] = v[3][0] = -1.0 / 2;
  v[4][0] = v[5][0] = v[6][0] = v[7][0] = 1.0 / 2;
  v[0][1] = v[1][1] = v[4][1] = v[5][1] = -1.0 / 2;
  v[2][1] = v[3][1] = v[6][1] = v[7][1] = 1.0 / 2;
  v[0][2] = v[3][2] = v[4][2] = v[7][2] = -1.0 / 2;
  v[1][2] = v[2][2] = v[5][2] = v[6][2] = 1.0 / 2;

  for (i = 5; i >= 0; i--) {
    glBegin(type);
    glNormal3fv(&n[i][0]);
    glVertex3fv(&v[faces[i][0]][0]);
    glVertex3fv(&v[faces[i][1]][0]);
    glVertex3fv(&v[faces[i][2]][0]);
    glVertex3fv(&v[faces[i][3]][0]);
    glEnd();
  }
}
#endif
#endif

void draw_piso () {
  int i;
    glColor3ub (70, 70, 70);
    glBegin (GL_LINES);
    for (i=-5; i < 6; i++) {
        glVertex3f (i, 0.0, -5.0); glVertex3f (i, 0.0, 5.0);
        glVertex3f (-5.0, 0.0, i); glVertex3f (5.0, 0.0, i);
    }
    glEnd ();
}

