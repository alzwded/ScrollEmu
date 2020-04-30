// RemapHJKL.cpp : Defines the entry point for the application.
//
#define UNICODE
#define _UNICODE
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <windowsx.h>
#include <shellscalingapi.h>
#include <shellapi.h>
#include <tchar.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <utility>
#include <sstream>

#define MAX_LOADSTRING 100

// debug
int hit = 0;

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);

BOOL myState = FALSE;
HWND g_hwnd = NULL;
HHOOK g_hook = NULL;
HICON hIconSm;

void Cleanup()
{
    if (g_hook) {
        UnhookWindowsHookEx(g_hook);
        g_hook = NULL;
    }
}

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

    BYTE andBits[] = {
        /*         LO    HI */
        /* 00 */ 0xFF, 0xFF,
        /* 01 */ 0x40, 0x02,
        /* 02 */ 0x40, 0x02,
        /* 03 */ 0x40, 0x02,
        /* 04 */ 0x40, 0x02,
        /* 05 */ 0x40, 0x02,
        /* 06 */ 0x40, 0x02,
        /* 07 */ 0x40, 0x02,
        /* 08 */ 0x40, 0x02,
        /* 09 */ 0x40, 0x02,
        /* 0A */ 0x40, 0x02,
        /* 0B */ 0x40, 0x02,
        /* 0C */ 0x20, 0x04,
        /* 0D */ 0x10, 0x08,
        /* 0E */ 0x0F, 0xF0,
        /* 0F */ 0x00, 0x00,
    };
    BYTE xorBits[] = {
        /*         LO    HI */
        /* 00 */ 0x00, 0x00,
        /* 01 */ 0x1F, 0xF8,
        /* 02 */ 0x1E, 0x78,
        /* 03 */ 0x1E, 0x78,
        /* 04 */ 0x1E, 0x78,
        /* 05 */ 0x1E, 0x78,
        /* 06 */ 0x1F, 0x78,
        /* 07 */ 0x1F, 0xF8,
        /* 08 */ 0x1F, 0xF8,
        /* 09 */ 0x1F, 0xF8,
        /* 0A */ 0x1F, 0xF8,
        /* 0B */ 0x1F, 0xF8,
        /* 0C */ 0x0F, 0xF0,
        /* 0D */ 0x07, 0xE0,
        /* 0E */ 0x00, 0x00,
        /* 0F */ 0x00, 0x00,
    };
    hIconSm = CreateIcon(hInstance,
            16,
            16,
            1,
            1,
            andBits,
            xorBits);

#if 0
    BYTE andBits2[sizeof(andBits) * 4] = {};
    BYTE xorBits2[sizeof(andBits2) * 4] = {};
    hIcon = CreateIcon(hInstance,
            32,
            32,
            1,
            1,
            andBits2,
            xorBits2);
#endif

	// Initialize global strings
    _tcscpy(szTitle, _T("ScrollEmu -- close this window to exit"));
    _tcscpy(szWindowClass, _T("ScrollEmuWindowClass"));
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	// Main message loop:
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
        TranslateMessage(&msg);
        DispatchMessage(&msg);
	}

	Cleanup();
	
	return (int) msg.wParam;
}

DWORD WINAPI ThrdProc(LPVOID data)
{
    long dx = ((std::pair<long, long>*)data)->first;
    long dy = ((std::pair<long, long>*)data)->second;
    delete ((std::pair<long, long>*)data);
    INPUT tInput[] = { { INPUT_MOUSE, }, { INPUT_MOUSE, } };
    long adjust = 2;
    long sz = 0;
    if (dx / adjust != 0 && dy / adjust == 0) {
        ++sz;
        tInput[0].mi = {};
        tInput[0].mi.dx = tInput[1].mi.dy = 0;
        //tInput[0].mi.mouseData = dx / adjust * WHEEL_DELTA;
        tInput[0].mi.mouseData = (dx / adjust < 0) ? -WHEEL_DELTA : WHEEL_DELTA;
        tInput[0].mi.dwFlags = MOUSEEVENTF_HWHEEL;
        tInput[0].mi.dwExtraInfo = NULL;
    }
    else if (dy / adjust != 0) {
        ++sz;
        tInput[0].mi = {};
        tInput[0].mi.dx = tInput[1].mi.dy = 0;
        //tInput[0].mi.mouseData = dy / adjust * WHEEL_DELTA;
        tInput[0].mi.mouseData = (dy / adjust < 0) ? -WHEEL_DELTA : WHEEL_DELTA;
        tInput[0].mi.dwFlags = MOUSEEVENTF_WHEEL;
        tInput[0].mi.dwExtraInfo = NULL;
        if (dx / adjust != 0) {
            ++sz;
            tInput[1].mi = {};
            tInput[1].mi.dx = tInput[1].mi.dy = 0;
            //tInput[1].mi.mouseData = dx / adjust * WHEEL_DELTA;
            tInput[0].mi.mouseData = (dx / adjust < 0) ? -WHEEL_DELTA : WHEEL_DELTA;
            tInput[1].mi.time = 0;
            tInput[1].mi.dwFlags = MOUSEEVENTF_HWHEEL;
            tInput[1].mi.dwExtraInfo = NULL;
        }
    }
    LPINPUT pInputs[] = { &tInput[0], &tInput[1] };
    static_assert(sizeof(pInputs) / sizeof(pInputs[0]) == sizeof(tInput) / sizeof(tInput[0]), "forgot something?");
    if (sz) {
        SendInput(sz, tInput, sizeof(INPUT));
    }
    ExitThread(0);
}


//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= hIconSm;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= hIconSm;

	return RegisterClassEx(&wcex);
}

LRESULT CALLBACK LLMouseHook(int code, WPARAM wParam, LPARAM lParam)
{
	if (code != HC_ACTION) return CallNextHookEx(NULL, code, wParam, lParam);

    MSLLHOOKSTRUCT* hs = (MSLLHOOKSTRUCT*)lParam;

    switch(wParam)
    {
        case WM_XBUTTONDOWN:
        case WM_NCXBUTTONDOWN:
            if(HIWORD(hs->mouseData) == XBUTTON2) {
                myState = true;
            }
            break;
        case WM_XBUTTONUP:
        case WM_NCXBUTTONUP:
            if(HIWORD(hs->mouseData) == XBUTTON2) {
                myState = false;
            }
            break;
        case WM_MOUSEMOVE:
            if(myState) {
                if(hs->flags & (LLMHF_LOWER_IL_INJECTED|LLMHF_INJECTED)) {
                    // FALLTHROUGH
                } else {
                    POINT p;
                    GetCursorPos(&p);
                    long x = p.x;
                    long y = p.y;

                    long dx, dy;
                    dx = x - hs->pt.x;
                    dy = hs->pt.y - y;
                    auto h = CreateThread(NULL, 0, &ThrdProc, new std::pair<long, long>(dx, dy), 0, NULL);
                    CloseHandle(h);
                }
                return 1;
            }
            break;
        default:
            break;
    }

	return CallNextHookEx(NULL, code, wParam, lParam);
}


//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindowEx(0, szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
	   CW_USEDEFAULT, CW_USEDEFAULT, 500, 100, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   // TODO also keyboardhook to handle scroll lock
   g_hook = SetWindowsHookEx(WH_MOUSE_LL, &LLMouseHook, hInst, 0);
   g_hwnd = hWnd;

   ShowWindow(hWnd, SW_SHOWDEFAULT);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_PAINT: {
		hdc = BeginPaint(hWnd, &ps);
        RECT r = { 0, 0, 100, 100 };
        FillRect(hdc, &r, (HBRUSH) (COLOR_WINDOW+1));
        DrawText(hdc, (std::wstringstream() << hit).str().c_str(), (std::wstringstream() << hit).str().size(), &r, DT_BOTTOM|DT_RIGHT);
		EndPaint(hWnd, &ps);
                   }break;
	case WM_DESTROY:
        Cleanup();
		PostQuitMessage(0);
        ExitProcess(0);
		break;
    case WM_CLOSE:
    {
        DestroyWindow(hWnd);
        return 0;
    }
	case WM_HOTKEY:
        break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
