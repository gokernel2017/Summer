//-------------------------------------------------------------------
//
// Link: -ld3d8 -ld3dx8d -lgdi32 -lwinmm
//
//-------------------------------------------------------------------
//
#include "summer.h"

#ifdef USE_GA
#ifdef USE_DIRECTX

//#ifdef DX8
    #include <DX8/d3dx8.h>
    typedef LPDIRECT3D8         LPDIRECT3D;
    typedef LPDIRECT3DDEVICE8   LPDIRECT3DDEVICE;
    #define Direct3DCreate      Direct3DCreate8
//#endif

static LPDIRECT3D D3D = NULL;
static LPDIRECT3DDEVICE  device = NULL;
//
static HFONT hFont;
static LPD3DXFONT pFont = NULL;
static HRESULT r=0;
int color = D3DCOLOR_XRGB(255,130,30);
static int count;

int DirectX_CreateDevice (HWND hwnd, int FullScreen) {
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

    // Create a D3D font using D3DX
    hFont =  (HFONT)GetStockObject(SYSTEM_FIXED_FONT); //SYSTEM_FONT);// DEFAULT_GUI_FONT

    // Create the D3DX Font
    r = D3DXCreateFont(device, hFont, &pFont);
    if (r != S_OK) {
        printf ("ERRO: DirectX Font Not Found\n");
  return 0;
    }

    return 1;
}

void gaBeginScene (void) {
//    device->Clear (0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,0,0), 1.0f, 0);
//    device->Clear (0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0f, 0);

    device->Clear (0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(254,238,204), 1.0f, 0);
    device->BeginScene ();
}
void gaEndScene (void) {
    device->EndScene ();
    device->Present (NULL, NULL, NULL, NULL);
}
void gaText (char *text, int x, int y, int color) {

    RECT TextRect={x,y,0,0};

    // Inform font it is about to be used
    pFont->Begin();

    // Calculate the rectangle the text will occupy
    //pFont->DrawText(text, -1, &TextRect, DT_CALCRECT, 0 );

    // Output the text, left aligned
    pFont->DrawText(text, -1, &TextRect, DT_LEFT, color);

    // Finish up drawing
    pFont->End();

    // Release the font
//    pFont->Release();
}

void drawRect (int x, int y, int w, int h) {
    struct Vertex {
        float x, y, z, rhw;
        DWORD color;
    } p[] = {
        { x, y,   0, 1,   color }, { x+w, y,    0, 1,  color },  // --
        { x+w, y, 0, 1,   color }, { x+w, y+h,  0, 1,  color },  // |
        { x+w, y+h, 0, 1, color }, { x, y+h,  0, 1,    color },  // --
        { x, y+h, 0, 1,   color }, { x, y,  0, 1,      color }  // |
    };

    #define SVertexType D3DFVF_XYZRHW | D3DFVF_DIFFUSE

//    #ifdef DX8
    device->SetVertexShader (SVertexType);
//    #endif
//    #ifdef DX9
//    device->SetFVF (SVertexType);
//    #endif
    device->DrawPrimitiveUP (D3DPT_LINELIST, 4, p, sizeof(Vertex));
}

void gaButton (int x, int y, int w, int h, char *txt) {
    drawRect (x,y,w,h);
    gaText (txt, x+5, y+5, color);
}

#endif // ! USE_DIRECTX
#endif // ! USE_GA
