#include <Windows.h>
#include "MagicWindow.h"
#include "MHRepErr.h"
#include "Settings.h"

extern HINSTANCE	MHInst;
extern HBRUSH brushes[4]; // Определён в MH001

bool MagicWindow::initialized=false;
bool MagicWindow::editmode=false;

MagicWindow MagicWindow::magic_wnd[NUM_MAGIC_WINDOWS]=
{
	{0,true,L"Окно 1",0,300,100,200,200,1,1,21,false,false,{sizeof(TRACKMOUSEEVENT),TME_LEAVE,0,HOVER_DEFAULT}}, // q
	{0,true,L"Окно 2",1,500,120,200,200,1,1,9,false,false,{sizeof(TRACKMOUSEEVENT),TME_LEAVE,0,HOVER_DEFAULT}} // e
};
	
//static TRACKMOUSEEVENT tme={sizeof(TRACKMOUSEEVENT),TME_LEAVE,0,HOVER_DEFAULT};

//======================================================================
// Оконная процедура 
//======================================================================
LRESULT CALLBACK MHMagicWndProc(HWND hwnd,
						UINT message,
						WPARAM wparam,
						LPARAM lparam)
{
	MagicWindow *mw;
	

	switch (message)
	{
		case WM_CREATE:
			LONG l;

			// Делаем окно полупрозрачным
			SetLayeredWindowAttributes(hwnd,NULL,255*50/100,LWA_ALPHA);
			// Запоминаем в загашнике указательна объект MagicWindow
			l=(LONG)(((CREATESTRUCT *)lparam)->lpCreateParams); 
			SetWindowLongPtr(hwnd,GWLP_USERDATA,l);
			// Для TrackMouseEvent
			mw=(MagicWindow *)l;
			mw->tme.hwndTrack=hwnd;
			break;

		case WM_ERASEBKGND:
			PAINTSTRUCT ps;
			HDC hdc;
			RECT rect;
			mw=(MagicWindow *)GetWindowLongPtr(hwnd,GWLP_USERDATA);

			hdc=BeginPaint(hwnd,&ps);
		
			GetClientRect(hwnd,&rect);
			FillRect(hdc,&rect,brushes[mw->mw_color]);
			
			//MoveToEx(hdc,100,100,NULL);
			//LineTo(hdc,49,49);
					
			EndPaint(hwnd,&ps);
			return 1; // ВЫЯСНИТЬ!!!
			break;

		// Пара событий, по которым мы определяем, находится ли мышь над окном
		case WM_MOUSEMOVE:
			mw=(MagicWindow *)GetWindowLongPtr(hwnd,GWLP_USERDATA);
			if((!MagicWindow::editmode)&&(!(mw->f_inside_window)))
			{
				TrackMouseEvent(&(mw->tme));
				mw->f_inside_window=true;
				mw->Press(); // будь то кнопка или переключатель - состояние меняется при входе всегда
			}
			break;

		case WM_MOUSELEAVE:
			mw=(MagicWindow *)GetWindowLongPtr(hwnd,GWLP_USERDATA);
			if(!MagicWindow::editmode)
			{
				mw->f_inside_window=false;
				if(mw->button_or_switch==true) // Это кнопка, её надо отпустить. Переключатель при покидании не трогаем.
					mw->Press();
			}
			break;
/*
	 case WM_PAINT:
		PAINTSTRUCT ps;
		HDC hdc;
		hdc=BeginPaint(hwnd,&ps);
		
		MoveToEx(hdc,100,100,NULL);
		LineTo(hdc,49,49);

		
		EndPaint(hwnd,&ps);
		break;

	 case WM_USER_MOVEWINDOW:
		 MoveWindow(hwnd,wparam,lparam,100,100,FALSE);
		 break;
*/
		default:
			return DefWindowProc(hwnd,message,wparam,lparam);
	}

	return 0; // Обработали, свалились сюда по break
}

//======================================================================
// Создание окон!
//======================================================================
void MagicWindow::Init()
{
	int i;
	BOOL boolresult;
	ATOM aresult; // Для всяких кодов возврата
	TCHAR *MHMagicWindowCName=L"MHMagic20";
	

	if(initialized) return;
	// 1. Регистрация класса окна
	WNDCLASS wcl={CS_HREDRAW | CS_VREDRAW, MHMagicWndProc, 0,
		sizeof(LONG_PTR), // Сюда пропишем ссылку на объект
		//0,
		MHInst,
		LoadIcon( NULL, IDI_APPLICATION),
        LoadCursor(NULL, IDC_ARROW), 
		NULL, //(HBRUSH)GetStockObject(WHITE_BRUSH),
		NULL,
		MHMagicWindowCName
	};

	aresult=::RegisterClass(&wcl); 


	if (aresult==0)
	{
		MHReportError(__WIDEFILE__,L"RegisterClass (",__LINE__);
		return;
	}

	// Создание окон в цикле
	for(i=0;i<NUM_MAGIC_WINDOWS;i++)
	{
		//Создание главного окна
		magic_wnd[i].MWhwnd=CreateWindowEx(WS_EX_LAYERED|WS_EX_TOPMOST,
		//MHhwnd=CreateWindow( 
			MHMagicWindowCName,
			magic_wnd[i].mw_name,
			WS_BORDER,
			// WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
			//WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
			//WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
			magic_wnd[i].x, magic_wnd[i].y,
			//CW_USEDEFAULT, CW_USEDEFAULT,
			magic_wnd[i].width, magic_wnd[i].height,
			0L, 0L,
			MHInst,
			&magic_wnd[i] // укажем на объект
		);

		if (magic_wnd[i].MWhwnd==NULL)
		{
			MHReportError(__WIDEFILE__,L"CreateWindow",__LINE__);
			return;
		}

/*
		// Показываем окно
		ShowWindow( magic_wnd[i].MWhwnd, SW_SHOWNORMAL );
	

		boolresult=UpdateWindow( magic_wnd[i].MWhwnd ); 
		if(boolresult==0)
		{
			MHReportError(__WIDEFILE__,L"UpdateWindow",__LINE__);
			return;
		}
*/	
	}

	initialized=true;
}

//======================================================================
// Прячем окна
//======================================================================
void MagicWindow::Hide()
{
	int i;

	for(i=0;i<NUM_MAGIC_WINDOWS;i++)
	{
		if(magic_wnd[i].active) ShowWindow( magic_wnd[i].MWhwnd, SW_HIDE );
		if(magic_wnd[i].pressed) magic_wnd[i].Press(); // Отпускаем все нажатые клавиши
	}
}

//======================================================================
// Окна в режиме редактирования
//======================================================================
void MagicWindow::ShowEditable()
{
	int i;

	editmode=true; // Клавиши не нажимаются

	for(i=0;i<NUM_MAGIC_WINDOWS;i++)
	{
		// Установить нужные параметры отображения

		// Показать окна
		if(magic_wnd[i].active) ShowWindow( magic_wnd[i].MWhwnd, SW_SHOWNORMAL );
	}
}

//======================================================================
// Окна в режиме редактирования
//======================================================================
void MagicWindow::ShowRuntime()
{
	int i;

	editmode=false;

	for(i=0;i<NUM_MAGIC_WINDOWS;i++)
	{
		// Установить нужные параметры отображения

		// Показать окна
		if(magic_wnd[i].active) ShowWindow( magic_wnd[i].MWhwnd, SW_SHOWNORMAL );
	}
}

//======================================================================
// Если нажата - отжимаем. И наоборот.
// Содрано из MHKeypad
//======================================================================
void MagicWindow::Press()
{
	if(pressed) pressed=false; 
	else pressed=true;

	INPUT input={0};
	input.type=INPUT_KEYBOARD;
	input.ki.dwFlags = KEYEVENTF_SCANCODE;
	if(false==pressed)
	{
		input.ki.dwFlags|=KEYEVENTF_KEYUP;
	}

	if(dlg_scancodes[button_index].value>0xFF) // Этот скан-код из двух байтов, где первый - E0
	{
		input.ki.dwFlags|=KEYEVENTF_EXTENDEDKEY;
	}

	input.ki.wScan=dlg_scancodes[button_index].value;
	SendInput(1,&input,sizeof(INPUT));

}