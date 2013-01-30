#include <windows.h>
#include "resource.h"

int nBitmap = IDB_BITMAP2;

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam ) 
{
	switch(message)
	{
	case WM_LBUTTONDOWN:
		if (nBitmap == IDB_BITMAP1)
			nBitmap = IDB_BITMAP2;
		else if (nBitmap == IDB_BITMAP2)
			nBitmap = IDB_BITMAP1;
		RedrawWindow(hwnd,NULL,NULL,RDW_INVALIDATE);
		return 0;
		break;

	case WM_PAINT:
		{
			HDC hdc;
			HDC hMemDC;
			PAINTSTRUCT ps;
			HBITMAP hBitmapS;

			hdc = BeginPaint( hwnd, &ps );
			hBitmapS = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(nBitmap)); 
			hMemDC = CreateCompatibleDC(hdc);
			SelectObject(hMemDC, hBitmapS);
			BitBlt(hdc,10,10,121,106,hMemDC,0,0,SRCCOPY);
			DeleteDC(hMemDC);
			EndPaint( hwnd, &ps );
		}
		return 0;
		break;

	case WM_DESTROY:
		PostQuitMessage( 0 ) ;
		return 0;
		break;

	}
	return DefWindowProc(hwnd, message, wparam, lparam);
}

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR szCmdLine, int iCmdShow)
{
	WNDCLASSEX wcx = {0};

	wcx.cbSize = sizeof(WNDCLASSEX);
	wcx.hbrBackground = (HBRUSH)GetStockObject( WHITE_BRUSH );
	wcx.hCursor = LoadCursor( NULL, IDC_ARROW );
	wcx.hIcon = LoadIcon( GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
	wcx.hInstance = hInstance; 
	wcx.lpfnWndProc = WndProc;
	wcx.lpszClassName = TEXT("UniLogger");
	wcx.lpszMenuName = TEXT("Menu");    // no menu - ignore
	wcx.style = CS_HREDRAW | CS_VREDRAW; // Redraw the window
	wcx.hIconSm = LoadIcon( GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
	//
	RegisterClassEx(&wcx);
	//
	HWND hwnd = CreateWindow(TEXT("UniLogger"),TEXT("UniLogger!"),
		(WS_OVERLAPPED	| WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX),
		CW_USEDEFAULT, CW_USEDEFAULT, 300, 170, NULL, NULL, hInstance, NULL );
	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);
	MSG msg;
	while( GetMessage( &msg, NULL, 0, 0 ) )
	{
		TranslateMessage( &msg );  
		DispatchMessage( &msg );
	}
	return msg.wParam; 
}

