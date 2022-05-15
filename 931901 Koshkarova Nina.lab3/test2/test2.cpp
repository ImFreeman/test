#include "stdafx.h"
#include "test2.h"
#include <windows.h>
#include <math.h>
#include <d2d1.h>
#pragma comment(lib, "d2d1")

#define NUM    1000
#define TWOPI  (2 * 3.14159)

#define MAX_LOADSTRING 100

// Глобальные переменные:
HINSTANCE hInst;                                // Указатель на текущий запрос
WCHAR szTitle[MAX_LOADSTRING];                  // Текст в строке заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // Имя класса главного окна

// Объявление функций:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

ID2D1Factory* g_pD2DFactory = NULL; // Direct2D factory
ID2D1HwndRenderTarget* g_pRenderTarget = NULL; // Render target
ID2D1SolidColorBrush* g_pBlackBrush = NULL; // A black brush, reflect the line color

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

void CreateD2DResource(HWND hWnd)
{
	if (!g_pRenderTarget)
	{
		HRESULT hr;

		hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &g_pD2DFactory);

		// Obtain the size of the drawing area
		RECT rc;
		GetClientRect(hWnd, &rc);

		// Create a Direct2D render target
		hr = g_pD2DFactory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(
				hWnd,
				D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top)
			),
			&g_pRenderTarget
		);

		// Create a brush
		hr = g_pRenderTarget->CreateSolidColorBrush(
			D2D1::ColorF(D2D1::ColorF::Black),
			&g_pBlackBrush
		);
	}
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_TEST2, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TEST2));

    MSG msg;

    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TEST2));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_TEST2);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

// Сохраняем запрос в глобальной переменной
// Создаем и отражаем главное окно программы
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance;

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   CreateD2DResource(hWnd);
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

// Обрабатываем сообщение для главного окна
// WM_COMMAND - Обработка команды
// WM_PAINT   - Рисует главное окно
// WM_DESTROY - Завершает и возвращает

int	num_func = 0; //1-sin, 2-cos, 3-tan, 4-ctg

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
    HDC         hdc;
	static int  cxClient, cyClient;
	int         i;
	POINT       apt[NUM];

    switch (message)
    {
	case WM_SIZE:
	{
		cxClient = LOWORD(lParam);
		cyClient = HIWORD(lParam);
		return 0;
	}

    case WM_COMMAND:
    {
		int wmId = LOWORD(wParam);
		// Анализируем выбранный пункт в меню
		switch (wmId)
		{
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;

		case ID_FUNCTIONS_SIN:
			InvalidateRect(hWnd, NULL, TRUE);
			num_func = 1;
			UpdateWindow(hWnd);
			break;

		case ID_FUNCTIONS_COS:
			InvalidateRect(hWnd, NULL, TRUE);
			num_func = 2;
			UpdateWindow(hWnd);
			break;

		case ID_FUNCTIONS_TAN:
			InvalidateRect(hWnd, NULL, TRUE);
			num_func = 3;
			UpdateWindow(hWnd);
			break;

		case ID_FUNCTIONS_CTG:
			InvalidateRect(hWnd, NULL, TRUE);
			num_func = 4;
			UpdateWindow(hWnd);
			break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
    }

	break;

    case WM_PAINT:
    {
        hdc = BeginPaint(hWnd, &ps);

        g_pRenderTarget->BeginDraw();

        g_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

        g_pRenderTarget->DrawLine(
            D2D1::Point2F(0, cyClient / 2),
            D2D1::Point2F(cxClient, cyClient / 2),
            g_pBlackBrush, 0.5f
        );

        switch (num_func)
        {
        case 1:
            for (i = 0; i < NUM; i++)
            {
                g_pRenderTarget->DrawLine(
                    D2D1::Point2F(i * cxClient / NUM, cyClient / 2 * (1 - sin(TWOPI * i / NUM))),
                    D2D1::Point2F((i + 1) * cxClient / NUM, cyClient / 2 * (1 - sin(TWOPI * (i + 1) / NUM))),
                    g_pBlackBrush, 0.5f
                );
            }
            g_pRenderTarget->EndDraw();
            break;
        case 2:
            for (i = 0; i < NUM; i++)
            {
                g_pRenderTarget->DrawLine(
                    D2D1::Point2F(i * cxClient / NUM, cyClient / 2 * (1 - cos(TWOPI * i / NUM))),
                    D2D1::Point2F((i + 1) * cxClient / NUM, cyClient / 2 * (1 - cos(TWOPI * (i + 1) / NUM))),
                    g_pBlackBrush, 0.5f
                );
            }
            g_pRenderTarget->EndDraw();
            break;
        case 3:
            for (i = 0; i < NUM; i++)
            {
                g_pRenderTarget->DrawLine(
                    D2D1::Point2F(i * cxClient / NUM, cyClient / 2 * (1 - tan(TWOPI * i / NUM))),
                    D2D1::Point2F((i + 1) * cxClient / NUM, cyClient / 2 * (1 - tan(TWOPI * (i + 1) / NUM))),
                    g_pBlackBrush, 0.5f
                );
            }
            g_pRenderTarget->EndDraw();
            break;
        case 4:
            for (i = 0; i < NUM; i++)
            {
                g_pRenderTarget->DrawLine(
                    D2D1::Point2F(i * cxClient / NUM, cyClient / 2 * (1 - 1 / tan(TWOPI * i / NUM))),
                    D2D1::Point2F((i + 1) * cxClient / NUM, cyClient / 2 * (1 - 1 / tan(TWOPI * (i + 1) / NUM))),
                    g_pBlackBrush, 0.5f
                );
            }
            g_pRenderTarget->EndDraw();
            break;
        }
        return 0;    
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}