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

extern void openglMakeFont8x13 (void);

void (*idle)(void) = NULL;
int   width, height;

#ifdef __linux__
// gcc -o quad quad.c -lX11 -lGL -lGLU
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xresource.h>
//#include<X11/X.h>
#include<GL/gl.h>
#include<GL/glx.h>
//#include<GL/glu.h>

static Display  *display;
static XContext context;
Window          root;
Window          win;
XEvent          event;
static int      screen;
int             black;
int             white;

GLint                   att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
XVisualInfo             *vi;
Colormap                cmap;
XSetWindowAttributes    swa;
GLXContext              glc;
XWindowAttributes       gwa;

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

void set2D (int w, int h);

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
char buf1[16] = { 'F', 'P', 'S', ':', ' ', '6', '0', 0, 0 };
int gaFPS (void) {
    static int fps=0, t1=0, t2=0;
//    set2D(width, height);
    gaText (buf1, 10, 37);
    fps++;
    t1 = time(NULL);
    if (t1 != t2) {
        t2 = t1;
        sprintf (buf1, "FPS: %d", fps);
//        printf ("FPS: %d\n", fps);
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
    #ifdef WIN32
    glClearColor (0.0, 0.0, 0.0, 0.0);
//    glClear (GL_COLOR_BUFFER_BIT);
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // GL_COLOR_BUFFER_BIT);
    #endif
}

void gaEndScene (void) {
    #ifdef WIN32
    SwapBuffers (hDC);
    #endif
    #ifdef __linux__
    glXSwapBuffers(display, win);
    #endif
}

void set2D (int w, int h) {
/*
  	glDisable (GL_DEPTH_TEST);
    glDisable (GL_CULL_FACE);

    glViewport(0, 0, w, h);

    // set ortho projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glOrtho(0, (float)w, (float)h, 0, 1.0f, -1.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
*/
//  Note, there may be other things you need to change,
//	   depending on how you have your OpenGL state set up.
//
	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_TEXTURE_2D);

	// This allows alpha blending of 2D textures with the scene
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glViewport(0, 0, w, h);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	glOrtho(0.0, (GLdouble)w, (GLdouble)h, 0.0, 0.0, 1.0);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
}

void Leave2DMode () {
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glPopAttrib();
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

    int x = (GetSystemMetrics(SM_CXSCREEN) - w) / 2;
    int y = ((GetSystemMetrics(SM_CYSCREEN) - h) / 2) - (GetSystemMetrics(SM_CYCAPTION)/2);

    win = CreateWindowEx (
        0,                        // Extended possibilites for variation
        ClassName,                // Classname
        "Graphic Application API: BETA",
        WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX,
        x, y, w, h,
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
/*
    screen = DefaultScreen (display);

    black = BlackPixel (display, screen);
    white = WhitePixel (display, screen);

    context = XUniqueContext();

    //
    // Create the Window
    //
    win = XCreateSimpleWindow (
      display, DefaultRootWindow(display),
      100,100, w, h,
      1, black, white
      );
    XStoreName (display, win, "Graphic Application API: BETA");

    XSelectInput (display, win, StructureNotifyMask | ExposureMask | KeyPressMask
      			| LeaveWindowMask | EnterWindowMask);
    XMapWindow (display, win);
//
*/
    root = DefaultRootWindow(display);

    vi = glXChooseVisual(display, 0, att);

    if(vi == NULL) {
        printf("\n\tno appropriate visual found\n\n");
        exit(0);
    } else {
        printf("\n\tvisual %p selected\n", (void *)vi->visualid); /* %p creates hexadecimal output like in glxinfo */
    }

    cmap = XCreateColormap(display, root, vi->visual, AllocNone);

    swa.colormap = cmap;
    swa.event_mask = ExposureMask | KeyPressMask | PointerMotionMask;
 
    win = XCreateWindow(display, root, 0, 0, w, h, 0, vi->depth, InputOutput, vi->visual, CWColormap | CWEventMask, &swa);

    XMapWindow(display, win);
    XStoreName(display, win, "Graphic Application API: BETA");
 
    glc = glXCreateContext(display, vi, NULL, GL_TRUE);
    glXMakeCurrent(display, win, glc);
 
//    glEnable(GL_DEPTH_TEST); 


    #endif // ! __linux__

    openglMakeFont8x13 ();
    set2D (w, h);
    width = w; height = h;
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
                if(msg.message == WM_QUIT)
                    break;
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
            else
            if (event.type == MotionNotify) {
                if (_data_ && _data_->onmousemove) {
                    _event_->offsetX = event.xmotion.x;
                    _event_->offsetY = event.xmotion.y;
                    _data_->onmousemove(_event_);
                }
            }

        }// while (XPending(display))

        if (idle) idle ();

    }// while (!quit)

    XCloseDisplay (display);

    #endif // __linux__

}// gaRun ();

void testDrawTriangle (void) {
    int i;
glColor3f (0.0, 1.0, 0.0);
    glBegin(GL_POINTS);
    for (i = 0; i < 10; i++) {
        glVertex2i(10+5*i,110);
    }
/*
    Leave2DMode();

    glBegin(GL_LINE_LOOP);// GL_TRIANGLES);

    glVertex2f (0.0, 1.0);
    glVertex2f (0.87, -0.5);
    glVertex2f (-0.87, -0.5);

    glEnd();
*/
}
  
#endif // ! USE_GA
