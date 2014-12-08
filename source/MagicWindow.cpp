#include <Windows.h>
#include "MagicWindow.h"
#include "MHRepErr.h"
#include "Settings.h"

bool flag_magic_left_click=false; 
bool timer5_needed=false;
int mouse_auto_x_direction=0;
int mouse_auto_y_direction=0;
int mouse_auto_w_direction=0;


extern LONG screen_x, screen_y;

extern HINSTANCE	MHInst;
extern HWND		MHhwnd;
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
	{0,0,0,L"Окно 1",0,300,100,200,200,0,0,21,0,false,false,0,{sizeof(TRACKMOUSEEVENT),TME_LEAVE,0,HOVER_DEFAULT}}, // q
	{1,0,0,L"Окно 2",1,550,100,200,200,0,0,9,0,false,false,0,{sizeof(TRACKMOUSEEVENT),TME_LEAVE,0,HOVER_DEFAULT}}, // e
	{2,0,0,L"Окно 3",2,800,100,200,200,0,0,21,0,false,false,0,{sizeof(TRACKMOUSEEVENT),TME_LEAVE,0,HOVER_DEFAULT}}, // q
	{3,0,0,L"Окно 4",3,1050,100,200,200,0,0,9,0,false,false,0,{sizeof(TRACKMOUSEEVENT),TME_LEAVE,0,HOVER_DEFAULT}}, // e

	{4,0,0,L"Окно 5",0,300,350,200,200,0,0,21,0,false,false,0,{sizeof(TRACKMOUSEEVENT),TME_LEAVE,0,HOVER_DEFAULT}}, // q
	{5,0,0,L"Окно 6",1,550,350,200,200,0,0,9,0,false,false,0,{sizeof(TRACKMOUSEEVENT),TME_LEAVE,0,HOVER_DEFAULT}}, // e
	{6,0,0,L"Окно 7",2,800,350,200,200,0,0,21,0,false,false,0,{sizeof(TRACKMOUSEEVENT),TME_LEAVE,0,HOVER_DEFAULT}}, // q
	{7,0,0,L"Окно 8",3,1050,350,200,200,0,0,9,0,false,false,0,{sizeof(TRACKMOUSEEVENT),TME_LEAVE,0,HOVER_DEFAULT}}, // e

	{8,0,0,L"Окно 9",0,300,600,200,200,0,0,21,0,false,false,0,{sizeof(TRACKMOUSEEVENT),TME_LEAVE,0,HOVER_DEFAULT}}, // q
	{9,0,0,L"Окно 10",1,550,600,200,200,0,0,9,0,false,false,0,{sizeof(TRACKMOUSEEVENT),TME_LEAVE,0,HOVER_DEFAULT}}, // e
	{10,0,0,L"Окно 11",2,800,600,200,200,0,0,21,0,false,false,0,{sizeof(TRACKMOUSEEVENT),TME_LEAVE,0,HOVER_DEFAULT}}, // q
	{11,0,0,L"Окно 12",3,1050,600,200,200,0,0,9,0,false,false,0,{sizeof(TRACKMOUSEEVENT),TME_LEAVE,0,HOVER_DEFAULT}}, // e

	{12,0,0,L"Окно 13",0,300,850,200,200,0,0,21,0,false,false,0,{sizeof(TRACKMOUSEEVENT),TME_LEAVE,0,HOVER_DEFAULT}}, // q
	{13,0,0,L"Окно 14",1,550,850,200,200,0,0,9,0,false,false,0,{sizeof(TRACKMOUSEEVENT),TME_LEAVE,0,HOVER_DEFAULT}}, // e
	{14,0,0,L"Окно 15",2,800,850,200,200,0,0,21,0,false,false,0,{sizeof(TRACKMOUSEEVENT),TME_LEAVE,0,HOVER_DEFAULT}}, // q
	{15,0,0,L"Окно 16",3,1050,850,200,200,0,0,9,0,false,false,0,{sizeof(TRACKMOUSEEVENT),TME_LEAVE,0,HOVER_DEFAULT}}, // e


	{16,0,0,L"Окно 17",0,1300,100,200,200,0,0,21,0,false,false,0,{sizeof(TRACKMOUSEEVENT),TME_LEAVE,0,HOVER_DEFAULT}}, // q
	{17,0,0,L"Окно 18",1,1550,100,200,200,0,0,9,0,false,false,0,{sizeof(TRACKMOUSEEVENT),TME_LEAVE,0,HOVER_DEFAULT}}, // e
	
	{18,0,0,L"Окно 19",0,1300,350,200,200,0,0,21,0,false,false,0,{sizeof(TRACKMOUSEEVENT),TME_LEAVE,0,HOVER_DEFAULT}}, // q
	{19,0,0,L"Окно 20",1,1550,350,200,200,0,0,9,0,false,false,0,{sizeof(TRACKMOUSEEVENT),TME_LEAVE,0,HOVER_DEFAULT}}, // e
	
	{20,0,0,L"Окно 21",0,1300,600,200,200,0,0,21,0,false,false,0,{sizeof(TRACKMOUSEEVENT),TME_LEAVE,0,HOVER_DEFAULT}}, // q
	{21,0,0,L"Окно 22",1,1550,600,200,200,0,0,9,0,false,false,0,{sizeof(TRACKMOUSEEVENT),TME_LEAVE,0,HOVER_DEFAULT}}, // e
	
	{23,0,0,L"Окно 23",0,1300,850,200,200,0,0,21,0,false,false,0,{sizeof(TRACKMOUSEEVENT),TME_LEAVE,0,HOVER_DEFAULT}}, // q
	{24,0,0,L"Окно 24",1,1550,850,200,200,0,0,9,0,false,false,0,{sizeof(TRACKMOUSEEVENT),TME_LEAVE,0,HOVER_DEFAULT}} // e
	

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
			mw->x= (int)(short) LOWORD(lparam);
			if(MagicWindow::editmode) mw->x+=MagicWindow::adjust_rect.left;   // horizontal position 
			mw->y = (int)(short) HIWORD(lparam);
			if(MagicWindow::editmode) mw->y+=MagicWindow::adjust_rect.top;
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
			mw->width= (int)(short) LOWORD(lparam);
			if(MagicWindow::editmode) mw->width=mw->width-MagicWindow::adjust_rect.left+MagicWindow::adjust_rect.right;   // horizontal position 
			mw->height = (int)(short) HIWORD(lparam);
			if(MagicWindow::editmode) mw->height = mw->height-MagicWindow::adjust_rect.top+MagicWindow::adjust_rect.bottom;
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
			LONG_PTR l;

			// Делаем окно полупрозрачным
			SetLayeredWindowAttributes(hwnd,NULL,255*25/100,LWA_ALPHA);
			// Запоминаем в загашнике указательна объект MagicWindow
			l=(LONG_PTR)(((CREATESTRUCT *)lparam)->lpCreateParams); 
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
				TrackMouseEvent(&(mw->tme));
				mw->f_inside_window=true;

				if(mw->flag_ignore_mouse_move>0)
				{
					mw->flag_ignore_mouse_move--;
					break;
				}
				// Поубавляем прозрачность, если нажимаем, прозрачнеем, если отпускаем
				if(mw->pressed) SetLayeredWindowAttributes(hwnd,NULL,255*25/100,LWA_ALPHA);
				else SetLayeredWindowAttributes(hwnd,NULL,255*75/100,LWA_ALPHA);

				// При нажатии, если уже нажата другая кнопка в группе, отжать её (кроме самой себя)!
				if(!mw->pressed && (0!=mw->mw_group))
				{
					int i;
					for(i=0;i<NUM_MAGIC_WINDOWS;i++)
					{
						if((mw->button_index!=i)&&(MagicWindow::magic_wnd[i].mw_group==mw->mw_group)&&(MagicWindow::magic_wnd[i].pressed))
						{
							SetLayeredWindowAttributes(MagicWindow::magic_wnd[i].MWhwnd,NULL,255*25/100,LWA_ALPHA);
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
					SetLayeredWindowAttributes(hwnd,NULL,255*25/100,LWA_ALPHA);
					// Эта проверка возникла из-за ситуации, когда нажимаем курсор мыши/скрываем окно
					if(mw->pressed) mw->Press();
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
	//BOOL boolresult;
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
			MHhwnd, // Чтобы в таскбаре и при альт-табе не появлялись лишние окна
			0L,
			MHInst,
			//(LPVOID)1L
			(LPVOID)&magic_wnd[i] // укажем на объект !!! Далее - враньё, всё работает. (В 64-битной версии укзатель обрезается, приходится делать SetWindowLong (ниже))
			//0
		);


		//SetWindowLongPtr(magic_wnd[i].MWhwnd,GWLP_USERDATA,(LONG_PTR)&magic_wnd[i]);

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
	mouse_auto_x_direction=0;
	mouse_auto_y_direction=0;
	KillTimer(MHhwnd,5);

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
	timer5_needed=false;
	
	for(i=0;i<NUM_MAGIC_WINDOWS;i++)
	{
		// Установить нужные параметры отображения
		SetWindowLongPtr(magic_wnd[i].MWhwnd,GWL_STYLE,WS_POPUP);

		// Показать окна
		// А ещё здесь будем проверять, нужен ли он нам вообще

		if(magic_wnd[i].active) 
		{
			ShowWindow( magic_wnd[i].MWhwnd, SW_SHOWNORMAL );
			// Есть ли среди активных окон такие, которым нужен Timer 5
			if((dlg_scancodes[magic_wnd[i].button_index].value==0xE102)||
				(dlg_scancodes[magic_wnd[i].button_index].value==0xE103)||
				(dlg_scancodes[magic_wnd[i].button_index].value==0xE104)||
				(dlg_scancodes[magic_wnd[i].button_index].value==0xE105))
				timer5_needed=true;
		}
	}
	
	if(timer5_needed&&MHhwnd) SetTimer(MHhwnd,5,25,NULL); // 40 раз в секунду
}

//======================================================================
// Если нажата - отжимаем. И наоборот.
// Содрано из MHKeypad
//======================================================================
void MagicWindow::Press()
{
	BYTE lobyte=LOBYTE(dlg_scancodes[button_index].value),hibyte=HIBYTE(dlg_scancodes[button_index].value);

	if(pressed) pressed=false; 
	else pressed=true;

	// Специальные значения клавиш: E1-в старшем байте + число, обозначающее операцию
	if(0xE1==hibyte) 
	{
		PressSpecial(lobyte);
		return;
	}

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

//======================================================================
// вместо нажатия на клавишу выполняет какую-нибудь хитрую операцию
//======================================================================
void MagicWindow::PressSpecial(BYTE operation)
{
	switch(operation)
	{
	case 1: // Щелчок мышью + F12
		{
		if(false==pressed) return; // Щелкаем только при вхождении, а не при выходе

		// 1.0. Этот щелчок HookProc должен пропустить!
		flag_magic_left_click=true;

		// 1.01
		 SetActiveWindow(MWhwnd);
		 Sleep(500);

		// 1.1. убираем окно
		ShowWindow( MWhwnd, SW_HIDE );
		// 1.2. Двигаем и щелкаем мышью
		INPUT input[5]={0};

		input[0].type=INPUT_MOUSE;
		input[0].mi.dx=(x+width/2)*65535/(screen_x-1);
		input[0].mi.dy=(y+height/2)*65535/(screen_y-1);
		input[0].mi.dwFlags=MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_MOVE;

		input[1].type=INPUT_MOUSE;
		input[1].mi.dwFlags = MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_LEFTDOWN;
		input[0].mi.dx=(x+width/2)*65535/(screen_x-1);
		input[0].mi.dy=(y+height/2)*65535/(screen_y-1);
	

		input[2].type=INPUT_MOUSE;
		input[2].mi.dwFlags = MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_LEFTUP;	
		input[2].mi.dx=(x+width/2)*65535/(screen_x-1);
		input[2].mi.dy=(y+height/2)*65535/(screen_y-1);
		
		input[3].type=INPUT_KEYBOARD;
		input[3].ki.dwFlags = KEYEVENTF_SCANCODE;
		input[3].ki.wScan=0x58;

		input[4].type=INPUT_KEYBOARD;
		input[4].ki.dwFlags = KEYEVENTF_SCANCODE|KEYEVENTF_KEYUP;;
		input[4].ki.wScan=0x58;

		SendInput(3,input,sizeof(INPUT));
		Sleep(100);
		SendInput(2,&input[3],sizeof(INPUT));
		Sleep(100);

		// 1.3. показываем окно
		ShowWindow( MWhwnd, SW_SHOWNORMAL );

		flag_ignore_mouse_move=1;  // При спрятывании и появлении окна генерируется ложное mouse_move 

		break;
		}

	case 2: // Мышь едет влево
		if(false==pressed) mouse_auto_x_direction=0;
		else mouse_auto_x_direction=-1;
		break;

	case 3: // Мышь едет вправо
		if(false==pressed) mouse_auto_x_direction=0;
		else mouse_auto_x_direction=1;
		break;

	case 4: // Колёсико мыши едет туда
		if(false==pressed) mouse_auto_w_direction=0;
		else mouse_auto_w_direction=1;
		break;

	case 5: // Колёсико мыши едет cюда
		if(false==pressed) mouse_auto_w_direction=0;
		else mouse_auto_w_direction=-1;
		break;
	}
}

//======================================================================
// для имитации движения мыши (и скролла)
//======================================================================
#define bfbc2_mspeed 30
#define wheel_mspeed 50
void MagicWindow::OnTimer5()
{
	if(editmode) return; // на всякий пожарный. таймер не должен работать в режиме редактирования
	
	if((0!=mouse_auto_x_direction)||(0!=mouse_auto_y_direction))
	{
		INPUT input={0};
		input.type=INPUT_MOUSE;
		input.mi.dx=mouse_auto_x_direction*bfbc2_mspeed;
		input.mi.dy=mouse_auto_y_direction*bfbc2_mspeed;
		input.mi.mouseData=0; // Нужно для всяких колёс прокрутки 
		//input.mi.dwFlags=MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_MOVE;
		input.mi.dwFlags=MOUSEEVENTF_MOVE;
		input.mi.time=0;
		input.mi.dwExtraInfo=0;
		SendInput(1,&input,sizeof(INPUT));
	}

	if(0!=mouse_auto_w_direction)
	{
		INPUT input={0};

#ifdef _DEBUG
			OutputDebugString(L"scroll-");
#endif

		input.type=INPUT_MOUSE;
		input.mi.dx=0L;
		input.mi.dy=0L;
		input.mi.mouseData=mouse_auto_w_direction*wheel_mspeed;
		input.mi.dwFlags=MOUSEEVENTF_WHEEL;
		input.mi.time=0;
		input.mi.dwExtraInfo=0;

		SendInput(1,&input,sizeof(INPUT));
	}
}