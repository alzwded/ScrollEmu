// RemapHJKL.cpp : Defines the entry point for the application.
//
#define UNICODE
#define _UNICODE
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <windowsx.h>
#include <shellscalingapi.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <shellapi.h>
#include <tchar.h>
#include <utility>
#include <sstream>

#define MAX_LOADSTRING 100
#define MY_NOTIFICATION_ICON 2
#define MY_SCROLLNOW (WM_USER + 88)

int hit = 0;

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

BOOL myState = FALSE;
HWND g_hwnd = NULL;
HHOOK g_hook = NULL;
long x, y;
HICON hIconSm, hIcon;


void Cleanup()
{
    //UnregisterHotKey(g_hwnd, 1);
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
    //__debugbreak();
    SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

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

    BYTE andBits2[sizeof(andBits) * 4] = {};
    BYTE xorBits2[sizeof(andBits2)] = {};
    hIcon = CreateIcon(hInstance,
            32,
            32,
            1,
            1,
            andBits2,
            xorBits2);

	// Initialize global strings
	//LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    _tcscpy(szTitle, _T("ScrollEmu -- close this window to exit"));
	//LoadString(hInstance, IDC_REMAPHJKL, szWindowClass, MAX_LOADSTRING);
    _tcscpy(szWindowClass, _T("ScrollEmuWindowClass"));
	MyRegisterClass(hInstance);


	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	//hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_REMAPHJKL));
    hAccelTable = NULL;

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		//if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		//{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		//}
	}

	Cleanup();
	
	return (int) msg.wParam;
}

volatile int numExited = 0;
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
    ++numExited;
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

#if 0
    auto hIcon = LoadIcon(hInstance, IDI_QUESTION);
    TCHAR pathToMyself[1024];
    TCHAR pathToIco[1024], *filePart;
    if(0 < GetModuleFileName(NULL, pathToMyself, sizeof(pathToMyself)/sizeof(pathToMyself[0]))) {
        GetFullPathName(pathToMyself, sizeof(pathToIco)/sizeof(pathToIco[0]), pathToIco, &filePart);
        _tcscpy(filePart, L"screen.ico");
        hIcon = LoadIcon(hInstance, pathToIco);
        _tcscpy(pathToIco, (std::wstringstream() << GetLastError()).str().c_str());
        MessageBox(NULL, pathToIco, pathToIco, MB_OK);
        ExitProcess(0);
    }
#endif
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= hIconSm;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName = NULL;// MAKEINTRESOURCE(IDC_REMAPHJKL);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= hIconSm;

	return RegisterClassEx(&wcex);
}

#include <fstream>
LRESULT CALLBACK KeyboardHook(int code, WPARAM wParam, LPARAM lParam)
{
	if (code != HC_ACTION) return CallNextHookEx(NULL, code, wParam, lParam);

	//if (!myState) return CallNextHookEx(NULL, code, wParam, lParam);

    MSLLHOOKSTRUCT* hs = (MSLLHOOKSTRUCT*)lParam;

    switch(wParam)
    {
        case WM_XBUTTONDOWN:
        case WM_NCXBUTTONDOWN:
            if(HIWORD(hs->mouseData) == XBUTTON2) {
                myState = true;
                x = hs->pt.x;
                y = hs->pt.y;
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
                    x = hs->pt.x;
                    y = hs->pt.y;
                } else {
                    POINT p;
                    GetCursorPos(&p);
                    x = p.x;
                    y = p.y;

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

   //RegisterHotKey(hWnd, 1, MOD_ALT | MOD_CONTROL, '3'); // TODO cleanup
   g_hook = SetWindowsHookEx(WH_MOUSE_LL, &KeyboardHook, hInst, 0);
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
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		//case IDM_ABOUT:
			//DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			//break;
		//case IDM_EXIT:
		//	DestroyWindow(hWnd);
		//	break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT: {
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
        RECT r = { 0, 0, 100, 100 };
        FillRect(hdc, &r, (HBRUSH) (COLOR_WINDOW+1));
        DrawText(hdc, (std::wstringstream() << hit).str().c_str(), (std::wstringstream() << hit).str().size(), &r, DT_BOTTOM|DT_RIGHT);
		EndPaint(hWnd, &ps);
                   }break;
	case WM_DESTROY:
        Cleanup();
		PostQuitMessage(0);
        //TerminateProcess(hWnd, 0);
        ExitProcess(0);
		break;
    case WM_CLOSE:
    {
        printf("WM_CLOSE\n");
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

// Message handler for about box.
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
