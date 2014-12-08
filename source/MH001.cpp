#include <Windows.h>
//#include <stdio.h>
#include "Bitmap.h"
//#include "MVector.h"
#include "Settings.h"
#include "MHRepErr.h"
#include "MagicWindow.h"
#include "CircleWindow.h"
#include "TET.h"
#include "TobiiREX.h"

#define MH_WINDOW_SIZE 200

//char debug_buf[4096];

// Глобальные переменные, которые могут потребоваться везде
TCHAR*		MHAppName=L"Из мыши в клавиатуру V2 28.07";
HINSTANCE	MHInst;
HWND		MHhwnd=NULL;
HBRUSH green_brush, yellow_brush, red_brush, blue_brush, brushes[4];
HPEN green_pen;
HFONT hfont;

short xsize,ysize; // Размер окна
LONG screen_x, screen_y;
double screen_scale=1.0;
HHOOK handle;

// Оконная процедура определена в MH002.cpp
LRESULT CALLBACK WndProc(HWND hwnd,	UINT message, WPARAM wparam, LPARAM lparam);
// А эта - в HookProc
LRESULT  CALLBACK HookProc(int disabled,WPARAM wParam,LPARAM lParam);

extern bool G_eytracker_is_working;
extern int G_eytracker_num; // какой из трекеров выбран
extern bool timer5_needed; // Из MagicWindow
//=======================================================================
// программа
//=======================================================================
int WINAPI WinMain(HINSTANCE hInst,HINSTANCE,LPSTR cline,INT)
// Командную строку не обрабатываем
{
	ATOM aresult; // Для всяких кодов возврата
	BOOL boolresult;
	MSG msg; // Сообщение
	TCHAR *MHWindowCName=L"MHook20";
	RECT rect={0,0,MH_WINDOW_SIZE,MH_WINDOW_SIZE};

	// Делаем hInst доступной для всех	
	MHInst=hInst;

	// Найдём размер экрана
	screen_x=GetSystemMetrics(SM_CXSCREEN);
	screen_y=GetSystemMetrics(SM_CYSCREEN);

	// Козлиная система разрешений экрана в windows8.1...
	DEVMODE dm;
	ZeroMemory (&dm, sizeof (dm));
	EnumDisplaySettings (NULL, ENUM_CURRENT_SETTINGS, &dm);

	screen_scale=((double)screen_x)/dm.dmPelsWidth;

	// Кисти
	green_brush=CreateSolidBrush(RGB(100,255,100));
	yellow_brush=CreateSolidBrush(RGB(227,198,2));
	red_brush=CreateSolidBrush(RGB(234,36,36));
	blue_brush=CreateSolidBrush(RGB(36,36,234));
	brushes[0]=green_brush;
	brushes[1]=yellow_brush;
	brushes[2]=red_brush;
	brushes[3]=blue_brush;
	hfont=CreateFont( -32, 0, 0, 0, FW_BOLD, 0, 0, 0,
		RUSSIAN_CHARSET,
		0, 0, 0, 0, L"Arial");
		// Кисти
	green_pen=CreatePen(PS_SOLID,4,RGB(100,255,100)); // зелёная

	// Создаём окно с кружком
	CircleWindow::Init();
	
	
	// С самого начала пытаемся загрузить конфигурацию по умолчанию
	MHSettings::OpenMHookConfig(NULL,L"default.MHOOK");
	
	
	// Регистрация класса окна
	WNDCLASS wcl={CS_HREDRAW | CS_VREDRAW, WndProc, 0, 0, hInst,
                          //LoadIcon( hInst, MAKEINTRESOURCE(IDI_ICON1)),
						  LoadIcon( NULL, IDI_APPLICATION),
                          LoadCursor(NULL, IDC_ARROW), 
                          (HBRUSH)GetStockObject(WHITE_BRUSH), 
						  //NULL,
						  NULL,
						  //MAKEINTRESOURCE(IDR_MENU1),
						  MHWindowCName };

	aresult=RegisterClass(&wcl);

	if (aresult==0)
	{
		MHReportError(__WIDEFILE__,L"RegisterClass",__LINE__);
		return (1);
	}

	// Какой нам нужен размер окна для клиентской области 200x200?
	//AdjustWindowRect(&rect,	WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX, false);
	AdjustWindowRect(&rect,	WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN, false);
	
	//Создание главного окна
	MHhwnd=CreateWindowEx(WS_EX_LAYERED,
	//MHhwnd=CreateWindowEx(WS_EX_LAYERED|WS_EX_TOPMOST,
	//MHhwnd=CreateWindow( 
		MHWindowCName,
		MHAppName,
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

	// Показываем окно (перед диалогом настроек свёрнуто)
    ShowWindow(MHhwnd, SW_MINIMIZE );

	boolresult=UpdateWindow( MHhwnd ); 
	if(boolresult==0)
	{
		MHReportError(__WIDEFILE__,L"UpdateWindow",__LINE__);
		return (1);
	}

	// Для волшебных окон (и диалога настроек) нужно окно MHhwnd
	// Создаём волшебные окна (пока скрытые)
	MagicWindow::Init();

	// Диалог настроек
	if(MHSettings::SettingsDialogue(MHhwnd)) return -1;

	// Разворачиваем окно mhook
	ShowWindow( MHhwnd, SW_SHOWNORMAL );
		
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

	// Выключаем айтрекер
	if(G_eytracker_is_working) // выключаем
	{
		// здесь будет выключение
		if(0==G_eytracker_num) BKBTobiiREX::Halt(NULL);
		else BKBTET::Halt(NULL);
	}

	// битмапы
	MHBitmap::Halt();

	// Отпускаем нажатые кнопки волшебных окон
	MagicWindow::Hide();

	// Кисти
	DeleteObject(green_brush);
	DeleteObject(yellow_brush);
	DeleteObject(red_brush);
	DeleteObject(blue_brush);
	// Фонт
	DeleteObject(hfont);
	// pen
	DeleteObject(green_pen);

	return 0;
}

