// tsak-1.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "task-2.h"
#include <vector>
#include <algorithm>

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна

// Прототипы функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

// Структура для хранения информации о значении и его процентном вкладе
struct ChartData {
    int value;
    double percent;
    COLORREF color;
};

// Глобальный массив для хранения данных диаграммы
std::vector<ChartData> chartData;

// Массив доступных цветов для секторов
COLORREF availableColors[] = { RGB(255, 0, 0), RGB(0, 255, 0), RGB(0, 0, 255), RGB(255, 255, 0), RGB(255, 0, 255), RGB(0, 255, 255) };
int numColors = sizeof(availableColors) / sizeof(availableColors[0]);

// Функция для рисования сектора окружности
void DrawPieSegment(HDC hdc, double percent, int centerX, int centerY, int radius, double startAngle, double sweepAngle, COLORREF color) {
    int start_x = centerX + static_cast<int>(radius * cos(startAngle * 3.14159265358979323846 / 180));
    int start_y = centerY - static_cast<int>(radius * sin(startAngle * 3.14159265358979323846 / 180));
    int end_x = centerX + static_cast<int>(radius * cos((startAngle + sweepAngle) * 3.14159265358979323846 / 180));
    int end_y = centerY - static_cast<int>(radius * sin((startAngle + sweepAngle) * 3.14159265358979323846 / 180));
    Pie(hdc, centerX - radius, centerY - radius, centerX + radius, centerY + radius, start_x, start_y, end_x, end_y);
}

// Функция для обновления данных диаграммы и перерисовки окна
void UpdateChart(HWND hWnd) {
    // Вычисляем общую сумму значений
    int totalValue = 0;
    for (const auto& data : chartData) {
        totalValue += data.value;
    }

    // Рассчитываем процентный вклад каждого значения и выбираем цвет для каждого сектора
    for (size_t i = 0; i < chartData.size(); ++i) {
        chartData[i].percent = (static_cast<double>(chartData[i].value) / totalValue) * 360.0;
        chartData[i].color = availableColors[i % numColors];
    }

    // Перерисовываем окно
    InvalidateRect(hWnd, NULL, TRUE);
    UpdateWindow(hWnd);
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Инициализируем строки глобальных переменных
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_TASK2, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Выполняем инициализацию приложения
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TASK2));

    MSG msg;

    // Основной цикл сообщений:
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
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TASK2));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_TASK2);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // Сохраняем маркер экземпляра в глобальной переменной

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
        // Инициализация начальных данных диаграммы
        chartData.push_back({ 10, 0.0, RGB(255, 0, 0) }); // Пример: значение 50
        chartData.push_back({ 70, 0.0, RGB(0, 255, 0) }); // Пример: значение 30
        chartData.push_back({ 10, 0.0, RGB(0, 0, 255) }); // Пример: значение 10
        chartData.push_back({ 5, 0.0, RGB(0, 0, 255) }); // Пример: значение 5
        chartData.push_back({ 5, 0.0, RGB(0, 0, 255) }); // Пример: значение 5
        UpdateChart(hWnd);
        break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        // Определение размеров клиентской области окна
        RECT clientRect;
        GetClientRect(hWnd, &clientRect);
        int centerX = (clientRect.right - clientRect.left) / 2;
        int centerY = (clientRect.bottom - clientRect.top) / 2;
        int radius = min(centerX, centerY) * 0.8; // Радиус окружности

        // Рисуем секторы окружности в соответствии с процентами и цветами
        double startAngle = 0.0;
        for (const auto& data : chartData) {
            SelectObject(hdc, CreateSolidBrush(data.color));
            DrawPieSegment(hdc, data.percent, centerX, centerY, radius, startAngle, data.percent, data.color);
            startAngle += data.percent;
            DeleteObject(SelectObject(hdc, GetStockObject(NULL_BRUSH))); // Очищаем выбранную кисть
        }

        EndPaint(hWnd, &ps);
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

// Обработчик сообщений для диалогового окна.
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
