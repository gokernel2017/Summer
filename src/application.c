//-------------------------------------------------------------------
//
// TANKS TO:
// ----------------------------------------------
//
//   01: God the creator of the heavens and the earth in the name of Jesus Christ.
//
//   02 - Fabrice Bellard: www.bellard.org
//
// ----------------------------------------------
//
// THIS FILE IS PART OF SUMMER LANGUAGE:
//
// Summer Application API:
//
// FILE:
//   application.c
//
// SUMMER LANGUAGE START DATE ( 27/08/2017 - 08:35 ):
//   rewrite: 20/07/2018 - 11:10
//
// BY:  Francisco - gokernel@hotmail.com
//
//-------------------------------------------------------------------
//
// Windows Max Mem: 22.140
//
#include "summer.h"
#include <GL/gl.h>

//
// Only WIN32 Implementation ... Please Wait for LInux ...
//

#ifdef USE_APPLICATION

//HINSTANCE WindowInstance = nullptr;
//void *nullptr;

static TEvent *_event_;
static int count;
static const char ClassName[] = "Summer_Object_Class";
static int  WindowCount;
static int running;
static HDC hDC;
static HGLRC hRC;        

OBJECT win;

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

typedef struct {
    int   type;

//    void  (*call)         (TEvent *evevt);
    void  (*onclick)      (TEvent *event);
    void  (*onmousemove)  (TEvent *event);
    void  (*onmousedown)  (TEvent *event);
    void  (*onmouseup)    (TEvent *event);

    void  *info; // any information to object

}DATA_OBJECT;

#ifdef WIN32
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


    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
        {
        DATA_OBJECT *data = (DATA_OBJECT*)GetWindowLongPtr (hwnd, GWLP_USERDATA);
        if (data) {
            POINT p;
            if (msg==WM_LBUTTONDOWN) SetCapture(hwnd);
            if (msg==WM_LBUTTONUP) ReleaseCapture();

//					int x=lParam&0xffff;
//					int y=(lParam&0xffff0000)>>16;

            GetCursorPos(&p);
//            _event_->x = lParam&0xffff;//LOWORD(lParam);
//            _event_->y = (lParam&0xffff0000)>>16;//HIWORD(lParam);

            _event_->x = p.x;
            _event_->y = p.y;

            if (data->onmousedown && (msg==WM_LBUTTONDOWN || msg==WM_RBUTTONDOWN)) {
                if (msg==WM_LBUTTONDOWN)
                    Gvar [VAR_MB].value.i = 1;
                else
                    Gvar [VAR_MB].value.i = 2;
                data->onmousedown(_event_);
            }
            else
            if (data->onmouseup && (msg==WM_LBUTTONUP || msg==WM_RBUTTONUP)) {
                Gvar [VAR_MB].value.i = 0;
                data->onmouseup(_event_);
            }
        }
        } break;


    case WM_MOUSEMOVE:
        {
        DATA_OBJECT *data = (DATA_OBJECT*)GetWindowLongPtr (hwnd, GWLP_USERDATA);

        if (data && data->onmousemove) {
            Gvar [VAR_OBJECT].value.p = hwnd;
            Gvar [VAR_MX].value.i     = LOWORD(lParam);
            Gvar [VAR_MY].value.i     = HIWORD(lParam);

            _event_->type = MOUSEMOVE; // WM_MOUSEMOVE;
            _event_->x = LOWORD(lParam);
            _event_->y = HIWORD(lParam);
            data->onmousemove(_event_);
        }
        } break;

    case WM_COMMAND:
        // not a menu
        if (lParam != 0) break;
        if (HIWORD(wParam) != 0) break;
        {
printf ("WM_COMMAND: %d\n", count++);
/*
        DATA_OBJECT *data = (DATA_OBJECT*)GetWindowLongPtr (hwnd, GWLP_USERDATA);

        if (data && data->call) {
            _event_->type = EV_MENU;
            _event_->x = LOWORD(wParam); // menu index id
            data->call(_event_);
        }
*/
        } break;

    default:
        return DefWindowProc (hwnd, msg, wParam, lParam);
    }

    return 0;

}// WindowProc()
#endif // ! WIN32


#ifdef WIN32
static LONG_PTR origButtonProc;
static LRESULT CALLBACK ButtonProc (OBJECT hwnd, int msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_LBUTTONUP) {
        int mx = (short)LOWORD(lParam);
        int my = (short)HIWORD(lParam);

        if (mx > 0 && my > 0) {
            RECT r;
            GetWindowRect (hwnd, &r);

            DATA_OBJECT *data = (DATA_OBJECT*)GetWindowLongPtr (hwnd, GWLP_USERDATA);

            if ((mx < r.right-r.left && my < r.bottom-r.top) && data && data->onclick) {
                _event_->type = 0;//EV_ZERO;
                _event_->id = GetDlgCtrlID(hwnd);
                data->onclick (_event_);
            }
        }
    }
    return CallWindowProc ((WNDPROC)origButtonProc, hwnd, msg, wParam, lParam);
}
#endif // WIN32

OBJECT AppNewWindow (OBJECT parent, int x, int y, int w, int h, char *text) {
    OBJECT o;

#ifdef WIN32

    int flags = WS_VISIBLE | WS_OVERLAPPEDWINDOW;

    o = CreateWindowEx (
      0,
      ClassName, // classname
      text,             // Title text
      flags,
      //WS_OVERLAPPEDWINDOW | WS_THICKFRAME | WS_CLIPCHILDREN, //flags,            // WS_OVERLAPPEDWINDOW
      x, y, w, h,       // x, y, w, h
      HWND_DESKTOP,     // parent
      NULL,             // id
      GetModuleHandle(NULL),    // Program Instance handler
      NULL              // No Window Creation data
      );

    DATA_OBJECT *data = (DATA_OBJECT*) malloc (sizeof(DATA_OBJECT));
    if (data) {
        data->onclick     = NULL;
        data->onmousemove = NULL;
        data->onmousedown = NULL;
        data->onmouseup   = NULL;
        // Set the DATA
        SetWindowLongPtr (o, GWLP_USERDATA, (LONG_PTR)data);
    }

#endif
#ifdef __linux__
    printf ("AppNewWindow no implemented in Linux\n");
#endif

    return o;
}
OBJECT AppNewRenderGL (OBJECT parent, int x, int y, int w, int h, char *text) {
//    OBJECT o;

#ifdef WIN32

    int style = WS_POPUP|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX|WS_VISIBLE; //WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX;
    int styleFS = WS_POPUP|WS_VISIBLE; //WS_POPUP
    int flags = WS_VISIBLE | WS_OVERLAPPEDWINDOW;

    win = CreateWindowEx (
      0,
      ClassName, // classname
      text,             // Title text
//      WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE,
      style,
      x,y, w, h,
      HWND_DESKTOP,     // parent
      NULL,             // id
      GetModuleHandle(NULL),    // Program Instance handler
      NULL              // No Window Creation data
      );

    EnableOpenGL (win, &hDC, &hRC);

    DATA_OBJECT *data = (DATA_OBJECT*) malloc (sizeof(DATA_OBJECT));
    if (data) {
        data->onclick     = NULL;
        data->onmousemove = NULL;
        data->onmousedown = NULL;
        data->onmouseup   = NULL;
        // Set the DATA
        SetWindowLongPtr (win, GWLP_USERDATA, (LONG_PTR)data);
    }

#endif
#ifdef __linux__
    printf ("AppNewWindow no implemented in Linux\n");
#endif

    return win;
}


OBJECT AppNewButton (OBJECT parent, int x, int y, int w, int h, char *text) {
    OBJECT o;

#ifdef WIN32
    int flags = WS_VISIBLE | WS_CHILD;

    o = CreateWindowEx (
      0,
      "button",        // classname
      text,            // Title text
      flags,
      x, y, w, h,      // x, y, w, h
      parent,          // parent
      (HMENU)100,      // id
      GetModuleHandle(NULL),    // Program Instance handler
      NULL             // No Window Creation data
			);

//    SendMessage (o, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), (LPARAM)TRUE);

    DATA_OBJECT *data = (DATA_OBJECT*) malloc (sizeof(DATA_OBJECT));
    if (data) {
        data->onclick     = NULL;
        data->onmousemove = NULL;
        data->onmousedown = NULL;
        data->onmouseup   = NULL;
        // Set the PROC end DATA
        origButtonProc = SetWindowLongPtr (o, GWLP_WNDPROC, (LONG_PTR)ButtonProc);
        SetWindowLongPtr (o, GWLP_USERDATA, (LONG_PTR)data);
    }
#endif // WIN32

    return o;
}


void AppSetCall (OBJECT o, void(*call)(TEvent *evevt), char *type) {
    if (o && call && type) {
        DATA_OBJECT *data = (DATA_OBJECT*)GetWindowLongPtr (o, GWLP_USERDATA);
        if (data) {
            if (!strcmp(type, "onclick"))     data->onclick = call;
            if (!strcmp(type, "onmousemove")) data->onmousemove = call;
            if (!strcmp(type, "onmousedown")) data->onmousedown = call;
            if (!strcmp(type, "onmouseup")) data->onmouseup = call;
        }
    }
}

int AppInit (int argc, char **argv) {
    static int init = 0;

    if (init) return 1;
    init = 1;

printf ("\nApplication, WIN32 Inplementation: AppInit(): %d\n", GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if ((_event_ = (TEvent*)malloc(sizeof(TEvent))) == NULL)
        return 0;

    _event_->type =
    _event_->value =
    _event_->id =
    _event_->x =
    _event_->y = 0;

    #ifdef WIN32

    WNDCLASSEX wc;

//    InitCommonControls();

    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = ClassName;
    wc.lpfnWndProc = WindowProc;
    wc.style = CS_DBLCLKS;
    //wc.style = CS_HREDRAW | CS_VREDRAW;
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

    #endif // WIN32

    timeBeginPeriod (1);

    return 1;
   
}// AppInit()

void AppRender (void) {
    SwapBuffers (hDC);
    glClearColor (0.0, 0.0, 0.0, 0.0);
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // GL_COLOR_BUFFER_BIT);
/*
    glClearColor (0.0f, 0.0f, 0.0f, 0.0f);
    glClear (GL_COLOR_BUFFER_BIT);
*/
}

void DisableOpenGL (HWND hWnd, HDC hDC, HGLRC hRC) {
    wglMakeCurrent (NULL, NULL);
    wglDeleteContext (hRC);
    ReleaseDC (hWnd, hDC);
}

void AppRun (void(*idle)(void)) {

printf ("\nApplication, WIN32 Inplementation: AppRun();\n");

    if (running) return;

    running = 1;

    #ifdef WIN32

    MSG msg;

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

    DisableOpenGL (win, hDC, hRC);

    #endif // WIN32

}// AppRun ()



#endif // ! USE_APPLICATION
