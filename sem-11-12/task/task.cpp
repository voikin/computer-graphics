#include "framework.h"
#include "task.h"

#include <gl/gl.h> // библиотека OpenGL
#include <gl/glu.h> // библиотека OpenGL

#define STB_IMAGE_IMPLEMENTATION // для текстур
#include "stb_image.h"

#pragma comment(lib,"opengl32.lib") // библиотека OpenGL
#pragma comment(lib,"glu32.lib") // библиотека OpenGL

#define MAX_LOADSTRING 100

HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];

GLfloat theta = 0;

int width, height, nrChannels;

stbi_uc* data = stbi_load("texture2.jpg", &width,
    &height, &nrChannels, NULL);

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

void EnableOpenGL(HWND hWnd, HDC* hDC, HGLRC* hRC);
void DisableOpenGL(HWND hWnd, HDC hDC, HGLRC hRC);
void resize(int width, int height);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_TASK, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TASK));

    MSG msg;

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
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDC_TASK));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_TASK);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

// OpenGL Context
HGLRC hRC;
HDC hdc;
// Установка материалов
GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat mat_shininess[] = { 50.0 };
GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    if (!data)
    {
        printf("Image loading error...\n");
    }

    // enable OpenGL for the window
    EnableOpenGL(hWnd, &hdc, &hRC);
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_SIZE:              // Resize The OpenGL Window
        resize(LOWORD(lParam), HIWORD(lParam));  // LoWord=Width, HiWord=Height
        break;
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // Разобрать выбор в меню:
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

        // OpenGL animation code goes here
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();

        glTranslatef(0.0f, 0.0f, -3.0f);
        glRotatef(theta, 0.8f, 0.5f, 0.2f);

        glBegin(GL_TRIANGLES); // задаем форму граней

        // Front Face
        glColor3f(1, 1, 1);
        glTexCoord2f(1.0f, 1.0f); glNormal3f(0.0, -0.816, 0.289);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(0.0f, 0.0f, 0.0f); //a
        glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, 0.0f, 0.0f);//b
        glTexCoord2f(0.0f, 0.0f); glVertex3f(0.5f, 0.289f, 0.816f);//s

        // Back Face
        glColor3f(1, 0, 0);
        glTexCoord2f(1.0f, 1.0f); glNormal3f(0.0, 0.0, 0.5);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(0.0f, 0.0f, 0.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(0.5f, 0.866f, 0.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(1.0f, 0.0f, 0.0f);

        // Left Face
        glColor3f(0, 0, 1);
        glTexCoord2f(1.0f, 1.0f); glNormal3f(-0.447, 0.0, 0.447);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(0.0f, 0.0f, 0.0f);//a
        glTexCoord2f(0.0f, 0.0f); glVertex3f(0.5f, 0.289f, 0.816f);//s
        glTexCoord2f(1.0f, 0.0f);  glVertex3f(0.5f, 0.866f, 0.0f);//c

        // Right Face
        glColor3f(0, 1, 0);
        glTexCoord2f(1.0f, 1.0f); glNormal3f(0.0, 0.447, -0.447);
        glTexCoord2f(0.0f, 1.0f);  glVertex3f(1.0f, 0.0f, 0.0f);
        glTexCoord2f(1.0f, 0.0f);   glVertex3f(0.5f, 0.866f, 0.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(0.5f, 0.289f, 0.816f);

        // Top и Bottom - убрала

        glEnd();

        SwapBuffers(hdc);
        theta += 0.1f;
        break;

    case WM_DESTROY:
        DisableOpenGL(hWnd, hdc, hRC);
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


// Enable OpenGL
void EnableOpenGL(HWND hWnd, HDC* hDC, HGLRC* hRC)
{
    PIXELFORMATDESCRIPTOR pfd;
    int format;

    // get the device context (DC)
    *hDC = GetDC(hWnd);
    RECT rect;

    // set the pixel format for the DC
    ZeroMemory(&pfd, sizeof(pfd));
    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;
    format = ChoosePixelFormat(*hDC, &pfd);
    SetPixelFormat(*hDC, format, &pfd);

    // create and enable the render context (RC)
    *hRC = wglCreateContext(*hDC);
    wglMakeCurrent(*hDC, *hRC);

    // изменения
    int width, height, nrChannels;
    stbi_uc* data = stbi_load("texture2.jpg", &width, &height, &nrChannels, NULL);

    if (data)
    {
        // invert Y coord
        for (int j = 0; j * 2 < height; ++j)
        {
            int index1 = j * width * nrChannels;
            int index2 = (height - 1 - j) * width * nrChannels;
            for (int i = width * nrChannels; i > 0; --i)
            {
                unsigned char temp = data[index1];
                data[index1] = data[index2];
                data[index2] = temp;
                ++index1;
                ++index2;
            }
        }

        // После загрузки текстуры требуется её подготовка
        GLuint texture;// Storage For One Texture
        glEnable(GL_TEXTURE_2D);
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        if (nrChannels == 3)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        else if (nrChannels == 4)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);

    }
    else
    {
        MessageBox(hWnd, L"Failed to load texture", L"Error", MB_OK);
    }
    glFrontFace(GL_CW);
    glEnable(GL_NORMALIZE);
    glShadeModel(GL_SMOOTH);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);//NICEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    {
        GLfloat ambient[4] = { 0.3f, 0.3f, 0.3f, 1.0f };
        GLfloat light0[4] = { -0.5f, 0.2f, 1.0f, 0.01f };
        glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
        glLightfv(GL_LIGHT0, GL_POSITION, light0);
        glLightfv(GL_LIGHT0, GL_SPECULAR, ambient);
    }
    glEnable(GL_COLOR_MATERIAL);

    GetClientRect(hWnd, &rect);
    resize(rect.right, rect.bottom);
}

// Disable OpenGL - без изменений
void DisableOpenGL(HWND hWnd, HDC hDC, HGLRC hRC)
{
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hRC);
    ReleaseDC(hWnd, hDC);
}

// для возможности масштабирования изображения при изменении размеров окна
void resize(int width, int height)
{
    glViewport(0, 0, width, height);// Reset The Current Viewport
    glMatrixMode(GL_PROJECTION);// Select The Projection Matrix
    glLoadIdentity();// Reset The Projection Matrix
    // Calculate The Aspect Ratio Of The Window
    gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);

    glMatrixMode(GL_MODELVIEW);// Select The Modelview Matrix
    glLoadIdentity();// Reset The Modelview Matrix
}