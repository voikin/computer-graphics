#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <windows.h>
#include <tchar.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define NUMP 20

struct pnt {
    POINT p;
    double angle;
};

void generate_random_points(RECT rect, struct pnt points[]);
int find_min_y_point(struct pnt points[]);
double polar(int x1, int y1, int x2, int y2);
void calculate_polar_angles(struct pnt points[], int min_index);
int comp(const void* A, const void* B);
void sort_points_by_angle(struct pnt points[]);
void draw_star(HDC hdc, struct pnt points[]);

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    const wchar_t CLASS_NAME[] = L"Sample Window Class";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0,                              // Optional window styles.
        CLASS_NAME,                     // Window class
        L"Star Polygon",                // Window text
        WS_OVERLAPPEDWINDOW,            // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

        NULL,       // Parent window    
        NULL,       // Menu
        hInstance,  // Instance handle
        NULL        // Additional application data
    );

    if (hwnd == NULL) {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);

    // Run the message loop.
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static struct pnt points[NUMP];

    switch (uMsg) {
    case WM_CREATE: {
        RECT rect;
        GetClientRect(hwnd, &rect);
        generate_random_points(rect, points);

        int min_index = find_min_y_point(points);
        calculate_polar_angles(points, min_index);
        sort_points_by_angle(points);

        break;
    }

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        draw_star(hdc, points);

        EndPaint(hwnd, &ps);
        break;
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}

void generate_random_points(RECT rect, struct pnt points[]) {
    srand((unsigned int)time(NULL));

    for (int i = 0; i < NUMP; i++) {
        points[i].p.x = rand() % (rect.right - 20) + 10;
        points[i].p.y = rand() % (rect.bottom - 20) + 10;
    }
}

int find_min_y_point(struct pnt points[]) {
    int select = 0;
    int miny = points[0].p.y;

    for (int i = 1; i < NUMP; i++) {
        if (points[i].p.y < miny) {
            select = i;
            miny = points[i].p.y;
        }
    }
    return select;
}

double polar(int x1, int y1, int x2, int y2) {
    double x = x2 - x1;
    double y = y2 - y1;

    if (x == 0) {
        if (y == 0) return 0;
        return (y > 0) ? M_PI / 2.0 : 3 * M_PI / 2.0;
    }

    if (x < 0) return atan(y / x) + M_PI;

    double angle = atan(y / x);
    return (y >= 0) ? angle : angle + 2.0 * M_PI;
}

void calculate_polar_angles(struct pnt points[], int min_index) {
    std::swap(points[0], points[min_index]);
    points[0].angle = 0;

    for (int i = 1; i < NUMP; i++) {
        points[i].angle = polar(points[0].p.x, points[0].p.y, points[i].p.x, points[i].p.y);
    }
}

int comp(const void* A, const void* B) {
    double angleA = (*(struct pnt*)A).angle;
    double angleB = (*(struct pnt*)B).angle;
    return (angleA > angleB) - (angleA < angleB);
}

void sort_points_by_angle(struct pnt points[]) {
    qsort(points, NUMP, sizeof(struct pnt), comp);
}

void draw_star(HDC hdc, struct pnt points[]) {
    MoveToEx(hdc, points[0].p.x, points[0].p.y, NULL);
    for (int i = 1; i < NUMP; i++) {
        LineTo(hdc, points[i].p.x, points[i].p.y);
    }
    LineTo(hdc, points[0].p.x, points[0].p.y); // Замыкаем многоугольник
}
