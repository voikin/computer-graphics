#include "framework.h"
#include "task-cpp.h"
#include <cmath>
#include <algorithm>

#define MAX_LOADSTRING 100
#define points_count  8

// Шаблон куба с координатами точек
double cube_template[points_count][3] = {
    {-1,-1,-1}, {1,-1,-1}, {1,1,-1}, {-1,1,-1},
    {-1,-1,1}, {1,-1,1}, {1,1,1}, {-1,1,1}
};

#define faces  6
#define fvertex 4

// Определение граней куба
int grani[faces][fvertex] = {
    {0, 4, 5, 1}, {0, 1, 2, 3}, {0, 3, 7, 4},
    {5, 4, 7, 6}, {1, 5, 6, 2}, {2, 6, 7, 3}
};

// Определение цветов для каждой грани
int colors[faces][3] = {
    {165, 116, 138}, {111, 124, 156}, {150, 185, 215},
    {191, 217, 234}, {218, 236, 240}, {243, 247, 237}
};

double amp = 0.9;
double ambient = 0.4;
double light[3] = { -0.5, -0.5, -0.5 };

// Функция вращения куба вокруг оси X
void RotateCubeX(double angle) {
    double cosAngle = cos(angle);
    double sinAngle = sin(angle);
    for (int i = 0; i < points_count; ++i) {
        double y = cube_template[i][1];
        double z = cube_template[i][2];
        cube_template[i][1] = y * cosAngle - z * sinAngle;
        cube_template[i][2] = y * sinAngle + z * cosAngle;
    }
}

// Функция вращения куба вокруг оси Y
void RotateCubeY(double angle) {
    double cosAngle = cos(angle);
    double sinAngle = sin(angle);
    for (int i = 0; i < points_count; ++i) {
        double x = cube_template[i][0];
        double z = cube_template[i][2];
        cube_template[i][0] = x * cosAngle + z * sinAngle;
        cube_template[i][2] = -x * sinAngle + z * cosAngle;
    }
}

POINT points[points_count];
static int centerX = 0;
static int centerY = 0;

// Функция проекции куба на 2D плоскость
void ProjectCube() {
    double focalLength = 500.0;
    for (int i = 0; i < points_count; ++i) {
        double z = cube_template[i][2] + 5;
        if (z == 0) z = 0.01;
        points[i].x = static_cast<LONG>((cube_template[i][0] * focalLength) / z) + centerX;
        points[i].y = static_cast<LONG>((cube_template[i][1] * focalLength) / z) + centerY;
    }
}

// Нормализация вектора
void normalize(double* v) {
    double len = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
    if (len != 0) {
        v[0] /= len;
        v[1] /= len;
        v[2] /= len;
    }
}

// Вычисление нормали для грани куба
void calculateNormal(int faceIndex, double* normal) {
    int* vertexIndices = grani[faceIndex];
    double v1[3], v2[3], v3[3];
    for (int i = 0; i < 3; i++) {
        v1[i] = cube_template[vertexIndices[0]][i];
        v2[i] = cube_template[vertexIndices[1]][i];
        v3[i] = cube_template[vertexIndices[2]][i];
    }
    double u[3] = { v2[0] - v1[0], v2[1] - v1[1], v2[2] - v1[2] };
    double w[3] = { v3[0] - v1[0], v3[1] - v1[1], v3[2] - v1[2] };
    normal[0] = u[1] * w[2] - u[2] * w[1];
    normal[1] = u[2] * w[0] - u[0] * w[2];
    normal[2] = u[0] * w[1] - u[1] * w[0];
    normalize(normal);
}

double K = 0.01;

// Вычисление интенсивности света для вершины
double calculateLightIntensity(double* vertex, double* normal) {
    double lightVector[3] = { light[0] - vertex[0], light[1] - vertex[1], light[2] - vertex[2] };
    double distance = sqrt(lightVector[0] * lightVector[0] + lightVector[1] * lightVector[1] + lightVector[2] * lightVector[2]);
    normalize(lightVector);
    double dotProduct = normal[0] * lightVector[0] + normal[1] * lightVector[1] + normal[2] * lightVector[2];
    double intensity = ambient + (amp * max(dotProduct, 0.0)) / (distance + K);
    return intensity;
}

// Вычисление нормалей для всех вершин куба
void calculateVertexNormals(double vertexNormals[points_count][3]) {
    for (int i = 0; i < points_count; ++i) {
        double normal[3] = { 0, 0, 0 };
        int count = 0;
        for (int j = 0; j < faces; ++j) {
            bool isShared = false;
            for (int k = 0; k < fvertex; ++k) {
                if (grani[j][k] == i) {
                    isShared = true;
                    break;
                }
            }
            if (isShared) {
                double faceNormal[3];
                calculateNormal(j, faceNormal);
                normal[0] += faceNormal[0];
                normal[1] += faceNormal[1];
                normal[2] += faceNormal[2];
                ++count;
            }
        }
        if (count > 0) {
            normal[0] /= count;
            normal[1] /= count;
            normal[2] /= count;
        }
        normalize(normal);
        vertexNormals[i][0] = normal[0];
        vertexNormals[i][1] = normal[1];
        vertexNormals[i][2] = normal[2];
    }
}

HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];

ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_TASKCPP, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TASKCPP));

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

// Проверка, находится ли точка внутри полигона
bool PointInPolygon(int x, int y, POINT poly[], int vertices) {
    bool inside = false;
    for (int i = 0, j = vertices - 1; i < vertices; j = i++) {
        if ((poly[i].y > y) != (poly[j].y > y) &&
            (x < (poly[j].x - poly[i].x) * (y - poly[i].y) / (poly[j].y - poly[i].y) + poly[i].x)) {
            inside = !inside;
        }
    }
    return inside;
}

// Интерполяция интенсивности вдоль ребра
double interpolateEdgeIntensity(double pos, double pos1, double pos2, double I1, double I2) {
    if (pos1 == pos2) return I1;  // Избежание деления на ноль
    double t = (pos - pos1) / (pos2 - pos1);
    return (1 - t) * I1 + t * I2;
}

// Билинейная интерполяция интенсивности внутри полигона
double bilinearInterpolateIntensity(double x, double y, POINT* poly, double* intensities) {
    double Ix1 = interpolateEdgeIntensity(x, poly[0].x, poly[1].x, intensities[0], intensities[1]);
    double Ix2 = interpolateEdgeIntensity(x, poly[3].x, poly[2].x, intensities[3], intensities[2]);
    return interpolateEdgeIntensity(y, poly[0].y, poly[3].y, Ix1, Ix2);
}

ATOM MyRegisterClass(HINSTANCE hInstance) {
    WNDCLASSEXW wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TASKCPP));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_TASKCPP);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow) {
    hInst = hInstance;
    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
    if (!hWnd) {
        return FALSE;
    }
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_SIZE: {
        int width = LOWORD(lParam);
        int height = HIWORD(lParam);
        centerX = width / 2;
        centerY = height / 2;
        InvalidateRect(hWnd, nullptr, TRUE);
    }
                break;
    case WM_COMMAND: {
        int wmId = LOWORD(wParam);
        switch (wmId) {
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
    case WM_KEYDOWN: {
        const double rotationStep = 0.1;
        switch (wParam) {
        case VK_UP:
            RotateCubeX(-rotationStep);
            InvalidateRect(hWnd, nullptr, TRUE);
            break;
        case VK_DOWN:
            RotateCubeX(rotationStep);
            InvalidateRect(hWnd, nullptr, TRUE);
            break;
        case VK_LEFT:
            RotateCubeY(rotationStep);
            InvalidateRect(hWnd, nullptr, TRUE);
            break;
        case VK_RIGHT:
            RotateCubeY(-rotationStep);
            InvalidateRect(hWnd, nullptr, TRUE);
            break;
        default:
            break;
        }
        break;
    }
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        RECT rect;
        GetClientRect(hWnd, &rect);
        HDC hdcBuf = CreateCompatibleDC(hdc);
        HBITMAP hbmBuf = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
        HBITMAP hOldBmp = (HBITMAP)SelectObject(hdcBuf, hbmBuf);
        FillRect(hdcBuf, &rect, (HBRUSH)(COLOR_WINDOW + 1));

        HBITMAP bmp = (HBITMAP)LoadImage(hInst, L"bitmap.bmp", IMAGE_BITMAP, rect.right, rect.bottom, LR_LOADFROMFILE);
        if (bmp != NULL) {
            HDC hdcImage = CreateCompatibleDC(hdc);
            HBITMAP hOldBmpImage = (HBITMAP)SelectObject(hdcImage, bmp);
            BitBlt(hdcBuf, 0, 0, rect.right, rect.bottom, hdcImage, 0, 0, SRCCOPY);
            SelectObject(hdcImage, hOldBmpImage);
            DeleteDC(hdcImage);
            DeleteObject(bmp);
        }

        ProjectCube();

        BITMAPINFO bmi;
        ZeroMemory(&bmi, sizeof(BITMAPINFO));
        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth = rect.right;
        bmi.bmiHeader.biHeight = -rect.bottom;
        bmi.bmiHeader.biPlanes = 1;
        bmi.bmiHeader.biBitCount = 32;
        bmi.bmiHeader.biCompression = BI_RGB;

        COLORREF* pixels = new COLORREF[rect.right * rect.bottom];
        if (!GetDIBits(hdcBuf, hbmBuf, 0, rect.bottom, pixels, &bmi, DIB_RGB_COLORS)) {
            delete[] pixels;
            return 0;
        }

        double vertexNormals[points_count][3];
        calculateVertexNormals(vertexNormals);

        double vertexIntensities[points_count];
        for (int i = 0; i < points_count; ++i) {
            vertexIntensities[i] = calculateLightIntensity(cube_template[i], vertexNormals[i]);
        }

        for (int i = 0; i < faces; ++i) {
            POINT poly[fvertex];
            double intensity[fvertex];
            for (int j = 0; j < fvertex; ++j) {
                poly[j] = points[grani[i][j]];
                intensity[j] = vertexIntensities[grani[i][j]];
            }

            long S = (poly[0].x - poly[1].x) * (poly[0].y + poly[1].y) +
                (poly[1].x - poly[2].x) * (poly[1].y + poly[2].y) +
                (poly[2].x - poly[3].x) * (poly[2].y + poly[3].y) +
                (poly[3].x - poly[0].x) * (poly[3].y + poly[0].y);
            if (S > 0) {
                for (int y = 0; y < rect.bottom; ++y) {
                    for (int x = 0; x < rect.right; ++x) {
                        if (PointInPolygon(x, y, poly, fvertex)) {
                            // Интерполяция интенсивности вдоль ребер
                            double I_top = interpolateEdgeIntensity(x, poly[0].x, poly[1].x, intensity[0], intensity[1]);
                            double I_bottom = interpolateEdgeIntensity(x, poly[3].x, poly[2].x, intensity[3], intensity[2]);
                            // Интерполяция интенсивности внутри полигона
                            double interpIntensity = interpolateEdgeIntensity(y, poly[0].y, poly[3].y, I_top, I_bottom);
                            int r = min(255, static_cast<int>(colors[i][0] * interpIntensity));
                            int g = min(255, static_cast<int>(colors[i][1] * interpIntensity));
                            int b = min(255, static_cast<int>(colors[i][2] * interpIntensity));
                            pixels[x + y * rect.right] = RGB(r, g, b);
                        }
                    }
                }
            }
        }

        SetDIBits(hdcBuf, hbmBuf, 0, rect.bottom, pixels, &bmi, DIB_RGB_COLORS);
        BitBlt(hdc, 0, 0, rect.right, rect.bottom, hdcBuf, 0, 0, SRCCOPY);

        SelectObject(hdcBuf, hOldBmp);
        DeleteDC(hdcBuf);
        DeleteObject(hbmBuf);
        delete[] pixels;

        EndPaint(hWnd, &ps);
    }
                 break;
    case WM_ERASEBKGND:
        return TRUE;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    UNREFERENCED_PARAMETER(lParam);
    switch (message) {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;
    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
