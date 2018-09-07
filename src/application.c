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
#include "summer.h"

//
// Only WIN32 Implementation ... Please Wait for LInux ...
//

#ifdef USE_APPLICATION

static TEvent *_event_;

static int count;

static const char ClassName[] = "Summer_Object_Class";
static int  WindowCount;
static int running;

typedef struct {
    int   type;

    void  (*call) (TEvent *evevt);

    void  *info; // any information to object

}DATA_OBJECT;


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

            if ((mx < r.right-r.left && my < r.bottom-r.top) && data && data->call) {
                _event_->type = 0;//EV_ZERO;
                _event_->id = GetDlgCtrlID(hwnd);
                data->call (_event_);
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
        data->call = NULL;
        // Set the DATA
        SetWindowLongPtr (o, GWLP_USERDATA, (LONG_PTR)data);
    }

#endif
#ifdef __linux__
    printf ("AppNewWindow no implemented in Linux\n");
#endif

    return o;
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
        data->call = NULL;
        // Set the PROC end DATA
        origButtonProc = SetWindowLongPtr (o, GWLP_WNDPROC, (LONG_PTR)ButtonProc);
        SetWindowLongPtr (o, GWLP_USERDATA, (LONG_PTR)data);
    }
#endif // WIN32

    return o;
}

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

void AppSetCall (OBJECT o, void(*call)(TEvent *evevt)) {
    if (o && call) {
        DATA_OBJECT *data = (DATA_OBJECT*)GetWindowLongPtr (o, GWLP_USERDATA);
        if (data)
            data->call = call;
    }
}

int AppInit (int argc, char **argv) {
    static int init = 0;

    if (init) return 1;
    init = 1;

printf ("\nApplication, WIN32 Inplementation: AppInit();\n");

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

    return 1;
   
}// AppInit()

void AppRun (void) {

printf ("\nApplication, WIN32 Inplementation: AppRun();\n");

    #ifdef WIN32

    MSG msg;

    while (GetMessage (&msg, NULL, 0, 0)) {
        TranslateMessage (&msg);
        DispatchMessage  (&msg);
    }

    #endif // WIN32

}// AppRun ()

#endif // ! USE_APPLICATION
