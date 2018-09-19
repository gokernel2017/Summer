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

typedef struct {
    void  (*onmousemove)  (EVENT *event);
    void  (*onmousedown)  (EVENT *event);
    void  (*onmouseup)    (EVENT *event);
}DATA;

static EVENT  *_event_ = NULL;
static DATA   *_data_ = NULL;

void basic() {}
void (*idle)(void) = basic;
int   width, height;

#ifdef USE_SDL
//
//-------------------------------------------------------------------
//
#include <SDL/SDL.h>

#define ASBITMAP      SDL_Surface
#define CHAR_SPACE    32
#define COLOR_ORANGE  64515

SDL_Surface *screen;

void base_text (ASBITMAP *bmp, char *text, short x, short y, int color);

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

void gaBeginScene (void) {
    SDL_Rect r = { 0,0, width, height };
    SDL_FillRect (screen, &r, 0);
}
void gaEndScene (void) {
    SDL_UpdateRect (screen,0,0,width, height);
}
void gaText (char *text, int x, int y, int color) {
    base_text (screen, text, x, y, color);
}

void putpixel (ASBITMAP *bmp, short x, short y, int color) {
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


void hline (ASBITMAP *bmp, int x1, int y, int x2, int color) {
    int i; // counter
    Uint8 *p; // pixel

    if ( y < bmp->clip_rect.y || y > bmp->clip_rect.y + bmp->clip_rect.h-1)
        return;

    // Set cliping
    if (x1 < bmp->clip_rect.x) x1 = bmp->clip_rect.x;
    if (x2 > bmp->clip_rect.x + bmp->clip_rect.w-1) x2 = bmp->clip_rect.x + bmp->clip_rect.w-1;

    int bpp = bmp->format->BytesPerPixel;

    //Here p is the address to the pixel we want to set
    p = (Uint8 *)bmp->pixels + y * bmp->pitch + x1 * bpp;

    switch (bpp) {
    case 2:
        for (i = x1; i <= x2; i++) {
            *(Uint16 *)p = color; // Set color
            p += bpp;             // Increment
        }
        break;
    case 4:
        for (i = x1; i <= x2; i++) {
            *(Uint32 *)p = color; // Set color
            p += bpp;             // Increment
        }
        break;
    }
}// hline ();

void vline (ASBITMAP *bmp, int x, int y1, int y2, int color) {
    const int bpp = bmp->format->BytesPerPixel;
    int i;
    Uint8 *p; // pixel

    if ( x < bmp->clip_rect.x || x > bmp->clip_rect.x + bmp->clip_rect.w-1 )
        return;

    // Set cliping
    if ( y1 < bmp->clip_rect.y ) y1 = bmp->clip_rect.y;
    if ( y2 > bmp->clip_rect.y + bmp->clip_rect.h-1 ) y2 = bmp->clip_rect.y + bmp->clip_rect.h-1;

    //Here p is the address to the pixel we want to set
    p = (Uint8 *)bmp->pixels + y1 * bmp->pitch + x * bpp;

    switch (bpp) {
    case 2:
        for (i = y1; i <= y2; i++) {
            *(Uint16 *)p = color; // Set color
            p += bmp->pitch;      // Increment
        }
        break;


    case 4:
        for (i = y1; i <= y2; i++) {
            *(Uint32 *)p = color; // Set color
            p += bmp->pitch;      // Increment
        }
        break;
    }
}// vline ();


void AS_base_draw_char_8x13_16 (ASBITMAP *bmp, unsigned char ch, short x, short y, int color) {

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
        if(*text > 32)
            AS_base_draw_char_8x13_16 ( bmp, (unsigned char)(*text), x, y, color );
        *text++;
        x += 8;

        if (*text == '\n') {
            x = orig_x;
            y += 15;
        }
    }
}
void drawRect (ASBITMAP *bmp, int x1, int y1, int x2, int y2, int color) {
    hline (bmp, x1, y1, x2, color); // top
    vline (bmp, x2, y1, y2, color); // right >
    hline (bmp, x1, y2, x2, color); // button
    vline (bmp, x1, y1, y2, color); // left <
}


void gaButton (int x, int y, int w, int h, char *txt) {
    drawRect (screen, x, y, x+w-1, y+h-1, COLOR_ORANGE); // left <
    base_text (screen, txt, x+5, y+7, COLOR_ORANGE);
}

//
//-------------------------------------------------------------------
//
#endif // ! USE_SDL

#ifdef USE_DIRECTX
//
//-------------------------------------------------------------------
//
#include <windowsx.h>
#define COLOR_ORANGE  -32226

static const char ClassName[] = "Graphic_Application_Class";
static HWND       win;
static int        WindowCount;

LRESULT CALLBACK WindowProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

    switch (msg) {
    case WM_CREATE:
        WindowCount++;
        break;

    case WM_DESTROY:
        WindowCount--;
        if (!WindowCount) {
            PostQuitMessage (0); // exit of program if not windows ...
        }
        break;

    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
				SetCapture(hwnd);
        if (_data_ && _data_->onmousedown) {
            if (msg==WM_LBUTTONDOWN)
                _event_->which = 1;
            else
                _event_->which = 3;
            _data_->onmousedown(_event_);
        }
        break;

		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
				ReleaseCapture();
        if (_data_ && _data_->onmouseup) {
            if (msg==WM_LBUTTONUP)
                _event_->which = 1;
            else
                _event_->which = 3;
            _data_->onmouseup(_event_);
        }
        break;

    case WM_MOUSEMOVE:
        if (_data_ && _data_->onmousemove) {
            _event_->offsetX = GET_X_LPARAM(lParam); //LOWORD(lParam);
            _event_->offsetY = GET_Y_LPARAM(lParam); //HIWORD(lParam);
            _data_->onmousemove(_event_);
        }
        break;

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

//
//-------------------------------------------------------------------
//
#endif // ! USE_DIRECTX

//-------------------------------------------------------------------
//------------------------  ALL PLATFORMS  --------------------------
//-------------------------------------------------------------------
//
char buf1[16] = { 'F', 'P', 'S', ':', ' ', '6', '0', 0, 0 };
int gaFPS (void) {
    static int fps=0, t1=0, t2=0;
    gaText (buf1, 10, 10,COLOR_ORANGE);
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
    gaText (buf2, 320, 100, COLOR_ORANGE);
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
    _event_->offsetX = 0;
    _event_->offsetY = 0;
    _event_->which = 0;

    _data_ = (DATA*) malloc (sizeof(DATA));
    if (_data_) {
        _data_->onmousemove = NULL;
        _data_->onmousedown = NULL;
        _data_->onmouseup   = NULL;
    }

    #ifdef USE_DIRECTX

    WNDCLASSEX wc;

    ZeroMemory (&wc, sizeof(wc));

    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = ClassName;
    wc.lpfnWndProc = WindowProc;
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.cbSize = sizeof(WNDCLASSEX);

    // Use default icon and mouse-pointer
    wc.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszMenuName = NULL;                 // No menu
    wc.cbClsExtra = 0;                      // No extra bytes after the window class
    wc.cbWndExtra = 0;                      // structure or the window instance
    wc.hbrBackground = (HBRUSH) COLOR_BACKGROUND+1;

    // Register the window class, and if it fails quit the program
    if (!RegisterClassEx(&wc)) {
        MessageBox (0, "Class Register Nor Found", "Sorry ERRO:", MB_ICONINFORMATION);
        return 0;
    }

    int x = (GetSystemMetrics(SM_CXSCREEN) - w) / 2;
    int y = ((GetSystemMetrics(SM_CYSCREEN) - h) / 2) - (GetSystemMetrics(SM_CYCAPTION)/2);

    win = CreateWindowEx (
        0,                        // Extended possibilites for variation
        ClassName,                // Classname
        "Graphic Application API: DirectX 8",
        WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX,
        x, y, w, h,
        HWND_DESKTOP,             // The window is a child-window to desktop
        NULL,                     // No menu
        GetModuleHandle (NULL),   // Program Instance handler
        NULL                      // No Window Creation data
        );

    if (DirectX_CreateDevice(win, 1)) {
        printf ("OK ... DitectX 8 CreateDevice FOUND\n");
    } else {
        printf ("DitectX 8 CreateDevice FAILED\n");
        return 0;
    }

    ShowWindow (win, 1);

    timeBeginPeriod (1);

    #endif // ! USE_DIRECTX

    #ifdef USE_SDL

    SDL_Init (SDL_INIT_VIDEO);
    #ifdef _WIN32
    SDL_putenv ("SDL_VIDEO_CENTERED=center");
    #endif
    SDL_WM_SetCaption ("Graphic Application API: SDL | To Exit Press F12 !", NULL);
    screen = SDL_SetVideoMode (w, h, 16, 0); // color 16

    #endif // ! USE_SDL

    width = w; height = h;
    idle = call;

    return 1;

}// gaInit ();

void gaRun (void) {

    #ifdef USE_DIRECTX

    MSG msg;

    if (idle) {
        for (;;) {
            if (PeekMessage (&msg, NULL, 0, 0, PM_REMOVE)) {
                if(msg.message == WM_QUIT)
                    break;
                TranslateMessage (&msg);
                DispatchMessage (&msg);
            }
            idle ();
            Sleep(1);
        }
    } else {
        while (GetMessage (&msg, NULL, 0, 0)) {
            TranslateMessage (&msg);
            DispatchMessage  (&msg);
        }
    }

    #endif // ! USE_DIRECTX

    #ifdef USE_SDL

    SDL_Event e;
    int quit = 0;

    while (!quit) {

        while (SDL_PollEvent(&e)) {
            switch (e.type) {
            case SDL_KEYDOWN:
                if (e.key.keysym.sym == SDLK_F12)
                    quit = 1;
                break;

            case SDL_MOUSEBUTTONDOWN:
                if (_data_ && _data_->onmousedown) {
                    _event_->which = e.button.button;
                    _data_->onmousedown(_event_);
                }
                break;

            case SDL_MOUSEBUTTONUP:
                if (_data_ && _data_->onmouseup) {
                    _event_->which = e.button.button;
                    _data_->onmouseup(_event_);
                }
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

        idle();
        SDL_Delay(1);

    }// while (!quit)

    SDL_Quit();

    #endif // ! USE_SDL

}// gaRun ();

#endif // ! USE_GA
