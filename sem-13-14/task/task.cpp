#include "framework.h"
#include "task.h"
#include <d3d9.h>
#include <d3dx9.h>  // Include D3DX header
#include <math.h>

#define MAX_LOADSTRING 100

#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")  // Link D3DX library

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Разместите код здесь.

    // Инициализация глобальных строк
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_TASK, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Выполнить инициализацию приложения:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TASK));

    MSG msg;

    // Цикл основного сообщения:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TASK));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_TASK);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

LPDIRECT3D9 direct3d = NULL;
LPDIRECT3DDEVICE9 device = NULL;
D3DDISPLAYMODE display;
D3DPRESENT_PARAMETERS params;

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    direct3d = Direct3DCreate9(D3D_SDK_VERSION);
    direct3d->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &display);
    ZeroMemory(&params, sizeof(params));
    params.Windowed = TRUE;
    params.SwapEffect = D3DSWAPEFFECT_DISCARD;
    params.BackBufferFormat = display.Format;
    direct3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &params, &device);

    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory(&d3dpp, sizeof(d3dpp));
    d3dpp.BackBufferWidth = 1000;
    d3dpp.BackBufferHeight = 1000;
    d3dpp.BackBufferCount = 1;
    d3dpp.hDeviceWindow = hWnd;
    d3dpp.Flags = 0;
    d3dpp.Windowed = TRUE;
    d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
    d3dpp.EnableAutoDepthStencil = TRUE;
    d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
    d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;

    direct3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &device);

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

struct CUSTOMVERTEX
{
    FLOAT X, Y, Z;
    FLOAT NX, NY, NZ;
    DWORD color;
};

IDirect3DVertexBuffer9* vb;
IDirect3DIndexBuffer9* ib;

CUSTOMVERTEX vertices[] =
{
    {-0.5f, -0.5f, -0.5f, -1.0f, -1.0f, -1.0f, 0x00ff0000},
    {-0.5f,  0.5f, -0.5f, -1.0f,  1.0f, -1.0f, 0x00ff0000},
    { 0.5f, -0.5f, -0.5f,  1.0f, -1.0f, -1.0f, 0x00ff0000},
    { 0.5f,  0.5f, -0.5f,  1.0f,  1.0f, -1.0f, 0x00ff0000},

    { 0.5f, -0.5f, -0.5f,  1.0f, -1.0f, -1.0f, 0x0000ff00},
    { 0.5f,  0.5f, -0.5f,  1.0f,  1.0f, -1.0f, 0x0000ff00},
    { 0.5f, -0.5f,  0.5f,  1.0f, -1.0f,  1.0f, 0x0000ff00},
    { 0.5f,  0.5f,  0.5f,  1.0f,  1.0f,  1.0f, 0x0000ff00},

    { 0.5f, -0.5f,  0.5f,  1.0f, -1.0f,  1.0f, 0x000000ff},
    { 0.5f,  0.5f,  0.5f,  1.0f,  1.0f,  1.0f, 0x000000ff},
    {-0.5f, -0.5f,  0.5f, -1.0f, -1.0f,  1.0f, 0x000000ff},
    {-0.5f,  0.5f,  0.5f, -1.0f,  1.0f,  1.0f, 0x000000ff},

    {-0.5f, -0.5f,  0.5f, -1.0f, -1.0f,  1.0f, 0x00ffff00},
    {-0.5f,  0.5f,  0.5f, -1.0f,  1.0f,  1.0f, 0x00ffff00},
    {-0.5f, -0.5f, -0.5f, -1.0f, -1.0f, -1.0f, 0x00ffff00},
    {-0.5f,  0.5f, -0.5f, -1.0f,  1.0f, -1.0f, 0x00ffff00},

    {-0.5f,  0.5f, -0.5f, -1.0f,  1.0f, -1.0f, 0x0000ffff},
    {-0.5f,  0.5f,  0.5f, -1.0f,  1.0f,  1.0f, 0x0000ffff},
    { 0.5f,  0.5f, -0.5f,  1.0f,  1.0f, -1.0f, 0x0000ffff},
    { 0.5f,  0.5f,  0.5f,  1.0f,  1.0f,  1.0f, 0x0000ffff},

    { 0.5f, -0.5f, -0.5f,  1.0f, -1.0f, -1.0f, 0x00f0f0f0},
    { 0.5f, -0.5f,  0.5f,  1.0f, -1.0f,  1.0f, 0x00f0f0f0},
    {-0.5f, -0.5f, -0.5f, -1.0f, -1.0f, -1.0f, 0x00f0f0f0},
    {-0.5f, -0.5f,  0.5f, -1.0f, -1.0f,  1.0f, 0x00f0f0f0},
};

const unsigned short Idxes[] = {
    0, 2, 1, 2, 3, 1,
    4, 6, 5, 6, 7, 5,
    8, 10, 9, 10, 11, 9,
    12, 14, 13, 14, 15, 13,
    16, 18, 17, 18, 19, 17,
    20, 22, 21, 22, 23, 21,
};

float ViewAngle = 0;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_PAINT:
    {
        device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, D3DCOLOR_XRGB(50, 50, 50), 1.0f, 0);

        device->CreateVertexBuffer(sizeof(vertices), D3DUSAGE_WRITEONLY, D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE, D3DPOOL_DEFAULT, &vb, NULL);
        device->CreateIndexBuffer(sizeof(short) * 36, 0, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &ib, 0);

        device->BeginScene();
        {
            ViewAngle += 0.02f;
            float cv = (float)cos(ViewAngle);  // Исправлено преобразование double в float
            float sv = (float)sin(ViewAngle);  // Исправлено преобразование double в float

            D3DMATRIX World = {
                cv * cv, cv * sv, sv, 0,
                -sv, cv, 0, 0,
                -sv * cv, -sv * sv, cv, 0,
                0, 0, 0, 1,
            };

            device->SetTransform(D3DTS_WORLD, &World);

            void* vb_vertices;

            vb->Lock(0, sizeof(vertices), &vb_vertices, 0);
            memcpy(vb_vertices, vertices, sizeof(vertices));
            vb->Unlock();

            ib->Lock(0, sizeof(short) * 36, &vb_vertices, 0);
            memcpy(vb_vertices, Idxes, sizeof(short) * 36);
            ib->Unlock();
        }

        device->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE);
        device->SetStreamSource(0, vb, 0, sizeof(CUSTOMVERTEX));
        device->SetIndices(ib);
        device->SetRenderState(D3DRS_LIGHTING, TRUE);
        device->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
        device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

        D3DLIGHT9 light;
        ZeroMemory(&light, sizeof(light));
        light.Type = D3DLIGHT_DIRECTIONAL;
        light.Diffuse.r = 1.0f;
        light.Diffuse.g = 1.0f;
        light.Diffuse.b = 1.0f;
        D3DXVECTOR3 lightDir(0.0f, 0.0f, 1.0f);
        D3DXVec3Normalize((D3DXVECTOR3*)&light.Direction, &lightDir);  // Нормализация направления света

        device->SetLight(0, &light);
        device->LightEnable(0, TRUE);

        D3DMATERIAL9 material;
        ZeroMemory(&material, sizeof(material));
        material.Diffuse.r = 0.1f;
        material.Diffuse.g = 0.1f;
        material.Diffuse.b = 0.1f;
        material.Diffuse.a = 0.1f;
        material.Ambient.r = 0.9f;
        material.Ambient.g = 0.9f;
        material.Ambient.b = 0.9f;
        material.Ambient.a = 0.9f;
        device->SetMaterial(&material);

        device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);

        device->EndScene();
        device->Present(NULL, NULL, NULL, NULL);
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
