//-------------------------------------------------------------------
//
// Graphic Application API:
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

// Implementation: Windows & Linux

#ifdef USE_GA

void (*idle)(void) = NULL;

#ifdef __linux__
// gcc -o quad quad.c -lX11 -lGL -lGLU
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xresource.h>
//#include<X11/X.h>
//#include<GL/gl.h>
//#include<GL/glx.h>
//#include<GL/glu.h>

static Display  *display;
static XContext context;
Window          root;
Window          win;
XEvent          event;
static int      screen;
int             black;
int             white;

/*
//Display                 *dpy;
//Window                  root;
//GLint                   att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
XVisualInfo             *vi;
Colormap                cmap;
XSetWindowAttributes    swa;
//Window                  win;
//GLXContext              glc;
XWindowAttributes       gwa;
XEvent                  xev;
*/

#endif

#ifdef WIN32
static const char ClassName[] = "Graphic_Application_Class";
static int    WindowCount;
static int    running, count;
static HWND   win;

LRESULT CALLBACK WindowProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

    switch (msg) {
    case WM_CREATE:
        WindowCount++;
        break;

    case WM_DESTROY:
        WindowCount--;
        if (!WindowCount) {
            running = 0;
            PostQuitMessage (0); // exit of program if not windows ...
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

#endif

int gaInit (int w, int h, void(*call)(void)) {
    static int init = 0;

    if (init) return 0;
    init = 1;

    #ifdef WIN32
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

    win = CreateWindowEx (
        0,                        // Extended possibilites for variation
        ClassName,                // Classname
        "Graphic Application API: BETA",
        WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX,
        100, 100, w, h,
        HWND_DESKTOP,             // The window is a child-window to desktop
        NULL,                     // No menu
        GetModuleHandle (NULL),   // Program Instance handler
        NULL                      // No Window Creation data
        );

    ShowWindow (win, 1);

    #endif // ! WIN32

    #ifdef __linux__

    if ((display = XOpenDisplay (NULL)) == NULL) {
        printf ("X Display not found\n");
        return 0;
    }
    screen = DefaultScreen (display);

    black = BlackPixel (display, screen);
    white = WhitePixel (display, screen);

    context = XUniqueContext();

    //
    // Create the Window
    //
    win = XCreateSimpleWindow (
      display, DefaultRootWindow(display),
      100,100,800,600,
      1, black, white
      );
    XStoreName (display, win, "Graphic Application API: BETA");

    XSelectInput (display, win, StructureNotifyMask | ExposureMask | KeyPressMask
      			| LeaveWindowMask | EnterWindowMask);
    XMapWindow (display, win);

    #endif // ! __linux__

    idle = call;

    return 1;

}// gaInit ();

void gaRun (void) {

    #ifdef WIN32
    MSG msg;

    if (running) return;

    running = 1;

    if (idle) {
        for (;;) {
            if (PeekMessage (&msg, NULL, 0, 0, PM_REMOVE)) {
                TranslateMessage (&msg);
                DispatchMessage (&msg);
            }
            if (running==0) return;
            idle ();
        }
    } else {
        while (GetMessage (&msg, NULL, 0, 0)) {
            TranslateMessage (&msg);
            DispatchMessage  (&msg);
        }
    }
    #endif // WIN32

    #ifdef __linux__

    int quit = 0;

    while (!quit) {

        while (XPending(display)) {

            XNextEvent (display, &event);

            if (event.type == KeyPress) { // key F12: exit of main loop
                char buf[128] = {0};
                KeySym keysym;
                int len = XLookupString (&event.xkey, buf, sizeof(buf), &keysym, NULL);
                if (keysym == XK_F12)
                    quit = 1;
            }
        }// while (XPending(display))

        if (idle) idle ();

    }// while (!quit)

    XCloseDisplay (display);

    #endif // __linux__

}// gaRun ();
  
#endif // ! USE_GA
