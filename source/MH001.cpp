#include <Windows.h>
//#include <stdio.h>
#include "Bitmap.h"
//#include "MVector.h"
#include "Settings.h"
#include "MHRepErr.h"

#define MH_WINDOW_SIZE 200

//char debug_buf[4096];

// Глобальные переменные, которые могут потребоваться везде
char*		MHAppName="Из мыши в клавиатуру 03.03g";
HINSTANCE	MHInst;
HWND		MHhwnd;
HBRUSH green_brush, yellow_brush;

short xsize,ysize; // Размер окна
LONG screen_x, screen_y;
HHOOK handle;

// Оконная процедура определена в MH002.cpp
LRESULT CALLBACK WndProc(HWND hwnd,	UINT message, WPARAM wparam, LPARAM lparam);
// А эта - в HookProc
LRESULT  CALLBACK HookProc(int disabled,WPARAM wParam,LPARAM lParam);

// программа
int WINAPI WinMain(HINSTANCE hInst,HINSTANCE,LPSTR cline,INT)
// Командную строку не обрабатываем
{
	ATOM aresult; // Для всяких кодов возврата
	BOOL boolresult;
	MSG msg; // Сообщение
	char *MHWindowCName="MHook10";
	RECT rect={0,0,MH_WINDOW_SIZE,MH_WINDOW_SIZE};

	// Делаем hInst доступной для всех	
	MHInst=hInst;

	// Найдём размер экрана
	screen_x=GetSystemMetrics(SM_CXSCREEN);
	screen_y=GetSystemMetrics(SM_CYSCREEN);

	// С самого начала пытаемся загрузить конфигурацию по умолчанию
	MHSettings::OpenMHookConfig(NULL,"default.MHOOK");
	// До появления окна выводим окно установок 
	if(MHSettings::SettingsDialogue(NULL)) return -1;
	
	// Кисти
	green_brush=CreateSolidBrush(RGB(100,255,100));
	yellow_brush=CreateSolidBrush(RGB(227,198,2));


	// Регистрация класса окна
	WNDCLASS wcl={CS_HREDRAW | CS_VREDRAW, WndProc, 0, 0, hInst,
                          //LoadIcon( hInst, MAKEINTRESOURCE(IDI_ICON1)),
						  LoadIcon( NULL, IDI_APPLICATION),
                          LoadCursor(NULL, IDC_ARROW), 
                          (HBRUSH)GetStockObject(WHITE_BRUSH), 
						  //NULL,
						  NULL,
						  //MAKEINTRESOURCE(IDR_MENU1),
						  TEXT(MHWindowCName) };

	aresult=RegisterClass(&wcl);

	if (aresult==0)
	{
		MHReportError(__FILE__,"RegisterClass",__LINE__);
		return (1);
	}

	// Какой нам нужен размер окна для клиентской области 200x200?
	//AdjustWindowRect(&rect,	WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX, false);
	AdjustWindowRect(&rect,	WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN, false);
	
	//Создание главного окна
	MHhwnd=CreateWindowEx(WS_EX_LAYERED|WS_EX_TOPMOST,
	//MHhwnd=CreateWindow( 
		TEXT(MHWindowCName),
		TEXT(MHAppName),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
		//WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		CW_USEDEFAULT, CW_USEDEFAULT,
		//CW_USEDEFAULT, CW_USEDEFAULT,
		rect.right-rect.left, rect.bottom-rect.top,
		0L, 0L,
		hInst, 0L );

	if (MHhwnd==NULL)
	{
		//BKBReportError(__FILE__,"CreateWindow",__LINE__);
		return (1);
	}

	// Показываем окно
    ShowWindow( MHhwnd, SW_SHOWNORMAL );
	//ShowWindow( KBhwnd, SW_MAXIMIZE );

	boolresult=UpdateWindow( MHhwnd ); 
	if(boolresult==0)
	{
		MHReportError(__FILE__,"UpdateWindow",__LINE__);
		return (1);
	}

	
	
	// Инициализируем работу хука
	handle = SetWindowsHookEx(WH_MOUSE_LL, 
									HookProc, 
                                 GetModuleHandle(NULL), 
                                 NULL);

	//Цикл обработки сообщений
	while(GetMessage(&msg,NULL,0,0)) 
    {
		TranslateMessage( &msg );
        DispatchMessage( &msg );
	}// while !WM_QUIT

	// Чистим за собой (на всякий случай ещё раз, если выходим не по WM_CLOSE)
	UnhookWindowsHookEx(handle);
	// битмапы
	MHBitmap::Halt();

	// Кисти
	DeleteObject(green_brush);
	DeleteObject(yellow_brush);

	return 0;
}

