#include <Windows.h>
#include "MagicWindow.h"
#include "MHRepErr.h"
#include "Settings.h"

extern HINSTANCE	MHInst;
extern HBRUSH brushes[4]; // Определён в MH001
extern HFONT hfont;

extern bool G_dialogue2_active; // чтобы окна могли писать на лету в диалог конфигурации
extern HWND Settings2HWND; // Для тех же целей
extern int IDC_EDIT_WXS[NUM_MAGIC_WINDOWS];
extern int IDC_EDIT_WYS[NUM_MAGIC_WINDOWS];
extern int IDC_EDIT_WWIDTHS[NUM_MAGIC_WINDOWS];
extern int IDC_EDIT_WHEIGHTS[NUM_MAGIC_WINDOWS];

bool MagicWindow::initialized=false;
bool MagicWindow::editmode=true;
RECT MagicWindow::adjust_rect={0}; // корректировка размеров окна с учетом заголовка и рамки

MagicWindow MagicWindow::magic_wnd[NUM_MAGIC_WINDOWS]=
{
	{0,0,0,L"Окно 1",0,300,100,200,200,0,0,21,0,false,false,{sizeof(TRACKMOUSEEVENT),TME_LEAVE,0,HOVER_DEFAULT}}, // q
	{1,0,0,L"Окно 2",1,550,100,200,200,0,0,9,0,false,false,{sizeof(TRACKMOUSEEVENT),TME_LEAVE,0,HOVER_DEFAULT}}, // e
	{2,0,0,L"Окно 3",2,800,100,200,200,0,0,21,0,false,false,{sizeof(TRACKMOUSEEVENT),TME_LEAVE,0,HOVER_DEFAULT}}, // q
	{3,0,0,L"Окно 4",3,1050,100,200,200,0,0,9,0,false,false,{sizeof(TRACKMOUSEEVENT),TME_LEAVE,0,HOVER_DEFAULT}}, // e

	{0,0,0,L"Окно 5",0,300,350,200,200,0,0,21,0,false,false,{sizeof(TRACKMOUSEEVENT),TME_LEAVE,0,HOVER_DEFAULT}}, // q
	{1,0,0,L"Окно 6",1,550,350,200,200,0,0,9,0,false,false,{sizeof(TRACKMOUSEEVENT),TME_LEAVE,0,HOVER_DEFAULT}}, // e
	{2,0,0,L"Окно 7",2,800,350,200,200,0,0,21,0,false,false,{sizeof(TRACKMOUSEEVENT),TME_LEAVE,0,HOVER_DEFAULT}}, // q
	{3,0,0,L"Окно 8",3,1050,350,200,200,0,0,9,0,false,false,{sizeof(TRACKMOUSEEVENT),TME_LEAVE,0,HOVER_DEFAULT}}, // e

	{0,0,0,L"Окно 9",0,300,600,200,200,0,0,21,0,false,false,{sizeof(TRACKMOUSEEVENT),TME_LEAVE,0,HOVER_DEFAULT}}, // q
	{1,0,0,L"Окно 10",1,550,600,200,200,0,0,9,0,false,false,{sizeof(TRACKMOUSEEVENT),TME_LEAVE,0,HOVER_DEFAULT}}, // e
	{2,0,0,L"Окно 11",2,800,600,200,200,0,0,21,0,false,false,{sizeof(TRACKMOUSEEVENT),TME_LEAVE,0,HOVER_DEFAULT}}, // q
	{3,0,0,L"Окно 12",3,1050,600,200,200,0,0,9,0,false,false,{sizeof(TRACKMOUSEEVENT),TME_LEAVE,0,HOVER_DEFAULT}}, // e

	{0,0,0,L"Окно 13",0,300,850,200,200,0,0,21,0,false,false,{sizeof(TRACKMOUSEEVENT),TME_LEAVE,0,HOVER_DEFAULT}}, // q
	{1,0,0,L"Окно 14",1,550,850,200,200,0,0,9,0,false,false,{sizeof(TRACKMOUSEEVENT),TME_LEAVE,0,HOVER_DEFAULT}}, // e
	{2,0,0,L"Окно 15",2,800,850,200,200,0,0,21,0,false,false,{sizeof(TRACKMOUSEEVENT),TME_LEAVE,0,HOVER_DEFAULT}}, // q
	{3,0,0,L"Окно 16",3,1050,850,200,200,0,0,9,0,false,false,{sizeof(TRACKMOUSEEVENT),TME_LEAVE,0,HOVER_DEFAULT}} // e
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
		case WM_GETMINMAXINFO: // минимальный размер окон 50x50
			MINMAXINFO *lpMinMaxInfo;
			lpMinMaxInfo= (MINMAXINFO *) lparam;
			lpMinMaxInfo->ptMinTrackSize.x = 50;
			lpMinMaxInfo->ptMinTrackSize.y = 50;
			break;

		case WM_MOVE:
			//
			mw=(MagicWindow *)GetWindowLongPtr(hwnd,GWLP_USERDATA);
			mw->x= (int)(short) LOWORD(lparam)+MagicWindow::adjust_rect.left;   // horizontal position 
			mw->y = (int)(short) HIWORD(lparam)+MagicWindow::adjust_rect.top;
			if(G_dialogue2_active) // Прямо в диалоге меняем циферки
			{
				TCHAR unicode_buf[80];
				_itow_s(MagicWindow::magic_wnd[mw->myindex].x,unicode_buf,10);
				SendDlgItemMessage(Settings2HWND,IDC_EDIT_WXS[mw->myindex],WM_SETTEXT, 0, (LPARAM)unicode_buf);
				_itow_s(MagicWindow::magic_wnd[mw->myindex].y,unicode_buf,10);
				SendDlgItemMessage(Settings2HWND,IDC_EDIT_WYS[mw->myindex],WM_SETTEXT, 0, (LPARAM)unicode_buf);
			}
			break;

		case WM_SIZE:
			//The low-order word of lParam specifies the new width of the client area.
			//The high-order word of lParam specifies the new height of the client area. 
			mw=(MagicWindow *)GetWindowLongPtr(hwnd,GWLP_USERDATA);
			mw->width= (int)(short) LOWORD(lparam)-MagicWindow::adjust_rect.left+MagicWindow::adjust_rect.right;   // horizontal position 
			mw->height = (int)(short) HIWORD(lparam)-MagicWindow::adjust_rect.top+MagicWindow::adjust_rect.bottom;
			if(G_dialogue2_active) // Прямо в диалоге меняем циферки
			{
				TCHAR unicode_buf[80];
				_itow_s(MagicWindow::magic_wnd[mw->myindex].width,unicode_buf,10);
				SendDlgItemMessage(Settings2HWND,IDC_EDIT_WWIDTHS[mw->myindex],WM_SETTEXT, 0, (LPARAM)unicode_buf);
				_itow_s(MagicWindow::magic_wnd[mw->myindex].height,unicode_buf,10);
				SendDlgItemMessage(Settings2HWND,IDC_EDIT_WHEIGHTS[mw->myindex],WM_SETTEXT, 0, (LPARAM)unicode_buf);
			}

			break;


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

		//case WM_ERASEBKGND:
		case WM_PAINT:

			PAINTSTRUCT ps;
			HDC hdc;
			RECT rect;
			HFONT old_font;
			LONG text_y;

			mw=(MagicWindow *)GetWindowLongPtr(hwnd,GWLP_USERDATA);

			hdc=BeginPaint(hwnd,&ps);
		
			GetClientRect(hwnd,&rect);
			FillRect(hdc,&rect,brushes[mw->mw_color]);
			
			old_font=(HFONT)SelectObject(hdc, hfont);
			text_y=(rect.bottom-rect.top)/2-20;
			//if(text_y<10)
			TextOut(hdc,20,text_y,mw->mw_name,wcslen(mw->mw_name));
			// Возвращаем старый фонт
			SelectObject(hdc, old_font);

			//MoveToEx(hdc,100,100,NULL);
			//LineTo(hdc,49,49);
					
			EndPaint(hwnd,&ps);
			//return 1; // ВЫЯСНИТЬ!!!
			break;

		// Пара событий, по которым мы определяем, находится ли мышь над окном
		case WM_MOUSEMOVE:
			mw=(MagicWindow *)GetWindowLongPtr(hwnd,GWLP_USERDATA);
			if(mw->mouse_or_eytracker!=0) break; // Только мышиные окна обрабатываем!
			if((!MagicWindow::editmode)&&(!(mw->f_inside_window)))
			{
				// Поубавляем прозрачность, если нажимаем, прозрачнеем, если отпускаем
				if(mw->pressed) SetLayeredWindowAttributes(hwnd,NULL,255*50/100,LWA_ALPHA);
				else SetLayeredWindowAttributes(hwnd,NULL,255*75/100,LWA_ALPHA);

				TrackMouseEvent(&(mw->tme));
				mw->f_inside_window=true;

				// При нажатии, если уже нажата другая кнопка в группе, отжать её (кроме самой себя)!
				if(!mw->pressed && (0!=mw->mw_group))
				{
					int i;
					for(i=0;i<NUM_MAGIC_WINDOWS;i++)
					{
						if((mw->button_index!=i)&&(MagicWindow::magic_wnd[i].mw_group==mw->mw_group)&&(MagicWindow::magic_wnd[i].pressed))
						{
							SetLayeredWindowAttributes(MagicWindow::magic_wnd[i].MWhwnd,NULL,255*50/100,LWA_ALPHA);
							MagicWindow::magic_wnd[i].Press();
						}
						// На active не проверяем, ибо только активная может быть pressed
					} // for i
				}

				mw->Press(); // будь то кнопка или переключатель - состояние меняется при входе всегда

				// Здесь отпустить другую кнопку группы, если нажимаем
			}
			break;

		case WM_MOUSELEAVE:
			mw=(MagicWindow *)GetWindowLongPtr(hwnd,GWLP_USERDATA);
			if(mw->mouse_or_eytracker!=0) break; // Только мышиные окна обрабатываем!
			if(!MagicWindow::editmode)
			{
				mw->f_inside_window=false;
				if(0==mw->button_or_switch) // Это кнопка, её надо отпустить. Переключатель при покидании не трогаем.
				{
					// Возвращаем прозрачностьeqqeeqqqe
					SetLayeredWindowAttributes(hwnd,NULL,255*50/100,LWA_ALPHA);
					mw->Press();
				}
			}
			break;

		case WM_CLOSE: // Закрывать нельзя!
			break;

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
			MHMagicWindowCName,
			magic_wnd[i].mw_name,
			 WS_CAPTION|WS_THICKFRAME,
			magic_wnd[i].x, magic_wnd[i].y,
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

	}

	// корректировка размеров окна с учетом заголовка и рамки
	AdjustWindowRect(&adjust_rect,WS_CAPTION|WS_THICKFRAME,FALSE); 
	
	initialized=true;
}

//======================================================================
// Прячем окна и отпускаем кнопки
//======================================================================
void MagicWindow::Hide()
{
	int i;

	editmode=true; // Клавиши не нажимаются (для айтрекера, мышь и так в них не попадёт)

	for(i=0;i<NUM_MAGIC_WINDOWS;i++)
	{
		if(magic_wnd[i].active) ShowWindow( magic_wnd[i].MWhwnd, SW_HIDE );
		if(magic_wnd[i].pressed) magic_wnd[i].Press(); // Отпускаем все нажатые клавиши
		magic_wnd[i].f_inside_window=false;
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
		SetWindowLongPtr(magic_wnd[i].MWhwnd,GWL_STYLE, WS_CAPTION|WS_THICKFRAME);
		//SetWindowLongPtr(magic_wnd[i].MWhwnd,GWL_STYLE, WS_THICKFRAME);

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
		SetWindowLongPtr(magic_wnd[i].MWhwnd,GWL_STYLE,WS_POPUP);

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