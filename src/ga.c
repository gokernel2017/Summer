//-------------------------------------------------------------------
//
// GA: Graphic Application API:
//
//   A portable graphic API ( C & WEB ) ... OpenGL, DirectX, HTML_Canvas.
//
// FILE:
//   ga.c
//
// START DATE: 12/09//2017 - 07:25
//
//-------------------------------------------------------------------
//
#include "summer.h"
#include <time.h>

// Implementation: Windows & Linux

#ifdef USE_GA
#include <SDL/SDL.h>

#define ASBITMAP      SDL_Surface
#define CHAR_SPACE    32
#define COLOR_ORANGE  64515

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
    void  (*onmousemove)  (EVENT *event);
    void  (*onmousedown)  (EVENT *event);
    void  (*onmouseup)    (EVENT *event);
}DATA;

SDL_Surface *screen;
static EVENT  *_event_ = NULL;
static DATA   *_data_ = NULL;

void basic() {}
void (*idle)(void) = basic;
int   width, height;


void putpixel (
    ASBITMAP *bmp,
    short x,
    short y,
    int color
) {
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

    switch(bpp){
    case 2:
        *(Uint16 *)p = color;
        break;

    case 4:
        *(Uint32 *)p = color;
        break;
    }

}//END: AS_base_putpixel()


void AS_base_draw_char_8x13_16 (
    ASBITMAP *bmp,
    unsigned char ch,
    short x,
    short y,
    int color
) {

  // Only 16 depth
  if (bmp->format->BytesPerPixel == 2 && ch > 32) {
    register unsigned char count;
    register int xx;

      xx = (ch - CHAR_SPACE) * 8;

      // insert color
      for (count=0; count < 8; count++) {
//        if ( fixed_font[ 0][xx+count] == 'x' ) { AS_putpixel(AS_buf, x+count, y+0,  color); }
          if ( fixed_font[ 0][xx] == 'x' ) { putpixel(bmp, x, y+0,  color); }
          if ( fixed_font[ 1][xx] == 'x' ) { putpixel(bmp, x, y+1,  color); }
          if ( fixed_font[ 2][xx] == 'x' ) { putpixel(bmp, x, y+2,  color); }
          if ( fixed_font[ 3][xx] == 'x' ) { putpixel(bmp, x, y+3,  color); }
          if ( fixed_font[ 4][xx] == 'x' ) { putpixel(bmp, x, y+4,  color); }
          if ( fixed_font[ 5][xx] == 'x' ) { putpixel(bmp, x, y+5,  color); }
          if ( fixed_font[ 6][xx] == 'x' ) { putpixel(bmp, x, y+6,  color); }
          if ( fixed_font[ 7][xx] == 'x' ) { putpixel(bmp, x, y+7,  color); }
          if ( fixed_font[ 8][xx] == 'x' ) { putpixel(bmp, x, y+8,  color); }
          if ( fixed_font[ 9][xx] == 'x' ) { putpixel(bmp, x, y+9,  color); }
          if ( fixed_font[10][xx] == 'x' ) { putpixel(bmp, x, y+10, color); }
          if ( fixed_font[11][xx] == 'x' ) { putpixel(bmp, x, y+11, color); }
          if ( fixed_font[12][xx] == 'x' ) { putpixel(bmp, x, y+12, color); }
          if ( fixed_font[13][xx] == 'x' ) { putpixel(bmp, x, y+13, color); }
          xx++; x++;
      }
  }
}

void base_text (ASBITMAP *bmp, char *text, short x, short y, int color) {
    short orig_x = x;

    while (*text) {
//      if (*text != CHAR_SPACE && *text != 9 && *text != 10 && *text != 13)
        if( *text > 32 )
            AS_base_draw_char_8x13_16 ( bmp, (unsigned char)(*text), x, y, color );
        *text++;
        x+=8;

        if (*text == '\n') {
            x = orig_x;
            y += 15;
        }
    }
}


#ifdef WIN32
//-------------------------------------------------------------------

LRESULT CALLBACK WindowProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

    switch (msg) {

    case WM_MOUSEMOVE:
        {
        if (_data_ && _data_->onmousemove) {
            _event_->offsetX = LOWORD(lParam);
            _event_->offsetY = HIWORD(lParam);
            _data_->onmousemove(_event_);
        }
        } break;

    case WM_COMMAND:
        // not a menu
        if (lParam != 0) break;
        if (HIWORD(wParam) != 0) break;
        {

        } break;

    default:
        return DefWindowProc (hwnd, msg, wParam, lParam);
    }

    return 0;

}// WindowProc()

//-------------------------------------------------------------------
#endif // ! WIN32

//-------------------------------------------------------------------
//------------------------  ALL PLATFORMS  --------------------------
//-------------------------------------------------------------------
//
char buf1[16] = { 'F', 'P', 'S', ':', ' ', '6', '0', 0, 0 };
int gaFPS (void) {
    static int fps=0, t1=0, t2=0;
    gaText (buf1, 10, 10);
    fps++;
    t1 = time(NULL);
    if (t1 != t2) {
        t2 = t1;
        sprintf (buf1, "FPS: %d", fps);
        fps=0;
        return 1;
    }
    return 0;
}

char buf2[16] = { 'F', 'P', 'S', ':', ' ', '6', '0', 0, 0 };
void gaDisplayMouse (int x, int y) {
    sprintf (buf2, "X: %d Y: %d", x, y);
    gaText (buf2, 100, 100);
}

void gaBeginScene (void) {
    SDL_Rect r = { 0,0, width, height };
    SDL_FillRect (screen, &r, 0);
}

void gaEndScene (void) {
    SDL_UpdateRect (screen,0,0,width, height);
}

void gaText (char *text, int x, int y) {
    base_text (screen, text, x, y, COLOR_ORANGE);
}


void gaSetCall (void(*call)(EVENT *evevt), char *type) {
    if (call && _data_ && type) {
        if (!strcmp(type, "onmousemove")) _data_->onmousemove = call;
        if (!strcmp(type, "onmousedown")) _data_->onmousedown = call;
        if (!strcmp(type, "onmouseup")) _data_->onmouseup = call;
    }
}

int gaInit (int w, int h, void(*call)(void)) {
    static int init = 0;

    if (init) return 0;
    init = 1;

    //
    // TYPE JAVA SCRIPT EVENT:
    //
    if ((_event_ = (EVENT*)malloc(sizeof(EVENT))) == NULL)
        return 0;

    _event_->target = NULL;
    _event_->offsetX =  _event_->offsetY = 0;

    _data_ = (DATA*) malloc (sizeof(DATA));
    if (_data_) {
        _data_->onmousemove = NULL;
        _data_->onmousedown = NULL;
        _data_->onmouseup   = NULL;
    }

    SDL_Init (SDL_INIT_VIDEO);
    #ifdef _WIN32
    SDL_putenv ("SDL_VIDEO_CENTERED=center");
    #endif
    SDL_WM_SetCaption ("HELLO: To Exit Press F12 !", NULL);
    screen = SDL_SetVideoMode (w, h, 16, 0); // color 16

    width = w; height = h;
    idle = call;

    return 1;

}// gaInit ();

void gaRun (void) {
    SDL_Event e;
    int quit = 0;

    while (!quit) {

        while (SDL_PollEvent(&e)) {
            switch (e.type) {
            case SDL_KEYDOWN:
                if (e.key.keysym.sym == SDLK_F12)
                    quit = 1;
                break;

            case SDL_MOUSEMOTION:
                if (_data_ && _data_->onmousemove) {
                    _event_->offsetX = e.motion.x;
                    _event_->offsetY = e.motion.y;
                    _data_->onmousemove(_event_);
                }
                break;

            }// switch (e.type)

        }// while (SDL_PollEvent(&e))

//        SDL_Delay(10);
        idle();

    }// while (!quit)

    SDL_Quit();

}// gaRun ();

#endif // ! USE_GA
