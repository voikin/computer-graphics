#include <Windows.h>

// Глобальные переменные
HWND g_hWnd;
int g_radius = 100;
const int g_minRadius = 1;
const int g_maxRadius = 400;
bool g_isIncreasing = true; // Флаг для определения, увеличиваем или уменьшаем радиус

// Прототипы функций
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void UpdateRadius();

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Регистрация класса окна
    const wchar_t CLASS_NAME[] = L"Sample Window Class";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    // Создание окна
    g_hWnd = CreateWindowEx(
        0,                              // Опции стиля
        CLASS_NAME,                     // Имя класса окна
        L"Resizable Circle",            // Заголовок окна
        WS_OVERLAPPEDWINDOW,            // Стиль окна
        CW_USEDEFAULT, CW_USEDEFAULT,   // Позиция окна
        800, 600,                       // Размер окна
        NULL, NULL, hInstance, NULL);

    if (g_hWnd == NULL) {
        return 0;
    }

    ShowWindow(g_hWnd, nCmdShow);

    // Создание таймера
    SetTimer(g_hWnd, 1, 10, NULL); // Задаем интервал таймера в 1 секунду

    // Основной цикл сообщений
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

// Обработчик сообщений для окна
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        // Очищаем область окна
        RECT rc;
        GetClientRect(hWnd, &rc);
        FillRect(hdc, &rc, (HBRUSH)(COLOR_WINDOW + 1));

        // Рисуем окружность
        Ellipse(hdc, 400 - g_radius, 300 - g_radius, 400 + g_radius, 300 + g_radius);

        EndPaint(hWnd, &ps);
    }
                 break;
    case WM_TIMER:
        UpdateRadius();
        InvalidateRect(hWnd, NULL, TRUE);
        break;
    case WM_DESTROY:
        KillTimer(hWnd, 1); // Удаляем таймер перед выходом
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

// Функция для обновления радиуса окружности
void UpdateRadius() {
    static int delta = 1; // Переменная для определения направления изменения радиуса

    // Если радиус меньше половины окна и мы увеличиваем радиус
    if (g_radius < (400 / 2) && g_isIncreasing) {
        g_radius += delta; // Увеличиваем радиус на значение delta
    }
    // Иначе, если радиус больше минимального значения и мы уменьшаем радиус
    else if (g_radius > g_minRadius && !g_isIncreasing) {
        g_radius -= delta; // Уменьшаем радиус на значение delta
    }
    // Если радиус достиг половины окна, меняем направление на уменьшение
    else {
        g_isIncreasing = false;
    }

    // Если радиус достиг половины окна, меняем направление на увеличение
    if (g_radius >= (400 / 2)) {
        g_isIncreasing = false;
    }
    // Если радиус достиг минимального значения, меняем направление на увеличение
    else if (g_radius <= g_minRadius) {
        g_isIncreasing = true;
    }
}