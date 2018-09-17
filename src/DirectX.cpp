#include "summer.h"

#ifdef WIN32

//#ifdef DX8
    #include <DX8/d3dx8.h>
    typedef LPDIRECT3D8         LPDIRECT3D;
    typedef LPDIRECT3DDEVICE8   LPDIRECT3DDEVICE;
    #define Direct3DCreate      Direct3DCreate8
//#endif

static LPDIRECT3D D3D = NULL;
static LPDIRECT3DDEVICE  device = NULL;

int gxCreateDevice (HWND hwnd, int FullScreen) {
    D3DPRESENT_PARAMETERS d3dpp;
    D3DDISPLAYMODE d3ddm;

    if ((D3D = Direct3DCreate (D3D_SDK_VERSION)) == NULL)
  return 0;

    if (FAILED(D3D->GetAdapterDisplayMode (D3DADAPTER_DEFAULT, &d3ddm)))
  return 0;

    ZeroMemory(&d3dpp, sizeof(d3dpp));
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = d3ddm.Format;
    d3dpp.EnableAutoDepthStencil = TRUE;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    d3dpp.Windowed   = TRUE;

    // Create the D3DDevice
    if (FAILED(D3D->CreateDevice (
        D3DADAPTER_DEFAULT,
        D3DDEVTYPE_HAL,
        hwnd,
        D3DCREATE_SOFTWARE_VERTEXPROCESSING, //D3DCREATE_HARDWARE_VERTEXPROCESSING
        &d3dpp,
        &device
        )))
        return 0;

    return 1;
}

#endif // ! WIN32
