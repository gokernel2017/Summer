//-------------------------------------------------------------------
//
// Graphic API:
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
//-------------------------------------------------------------------
#include <GL/gl.h>

static const char ClassName[] = "Graphic_Application_Class";
static int    WindowCount;
static int    running, count;
static HWND   win;
static HDC    hDC;
static HGLRC  hRC;

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

void EnableOpenGL (HWND hWnd, HDC *hDC, HGLRC *hRC) {
    PIXELFORMATDESCRIPTOR pfd;
    int iFormat;

    /* get the device context (DC) */
    *hDC = GetDC (hWnd);

    /* set the pixel format for the DC */
    ZeroMemory (&pfd, sizeof (pfd));
    pfd.nSize = sizeof (pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | 
      PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;
    iFormat = ChoosePixelFormat (*hDC, &pfd);
    SetPixelFormat (*hDC, iFormat, &pfd);

    /* create and enable the render context (RC) */
    *hRC = wglCreateContext( *hDC );
    wglMakeCurrent(*hDC, *hRC);
}

//-------------------------------------------------------------------
#endif // ! WIN32

//-------------------------------------------------------------------
//------------------------  ALL PLATFORMS  --------------------------
//-------------------------------------------------------------------
//
int gaFPS (void) {
    static int fps=0, t1=0, t2=0;
    fps++;
    t1 = time(NULL);
    if (t1 != t2) {
        t2 = t1;
//        printf ("FPS: %d TIME: %d\n", fps, t1);
        printf ("FPS: %d\n", fps);
        fps=0;
        return 1;
    }
    return 0;
}

void gaBeginScene (void) {
    #ifdef WIN32
    glClearColor (0.0, 0.0, 0.0, 0.0);
    glClear (GL_COLOR_BUFFER_BIT);
    //glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // GL_COLOR_BUFFER_BIT);
    #endif
}
void gaEndScene (void) {
    #ifdef WIN32
    SwapBuffers (hDC);
    #endif
}

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

    EnableOpenGL (win, &hDC, &hRC);

    ShowWindow (win, 1);

    timeBeginPeriod (1);

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
