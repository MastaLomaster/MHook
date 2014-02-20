#include <Windows.h>
#include "MHKeypad.h"
#include "Settings.h"

extern HWND		MHhwnd; // Нужна для установки таймера
extern LONG screen_x, screen_y; // Для определения углов экрана

bool flag_left_button_key=false;
//static DWORD last_left_down_time;
//static DWORD last_screen_top_time;
int top_position=-1; // 0 - левый верхний угол, 1- правый верхний угол, -1 - убрали
//static bool mid_button_down=false;
static bool right_button_down=false;
//====================================================================================
// Собственно, хук 
//====================================================================================
LRESULT  CALLBACK HookProc(int disabled,WPARAM wParam,LPARAM lParam) 
{
	//static bool initialized=false;
	//static LONG last_x, last_y;
	
	
	
    if (!disabled)
	{
		MOUSEHOOKSTRUCT * pMouseStruct = (MOUSEHOOKSTRUCT *)lParam;
		if (pMouseStruct != NULL)
		{
			switch(wParam)
			{
			case WM_MOUSEMOVE:
				// А находимся ли мы в углу экрана?
				if(pMouseStruct->pt.x+pMouseStruct->pt.y<5) 
				{
					top_position=0;
					// это ваще не надо last_screen_top_time=timeGetTime();
					SetTimer(MHhwnd,2,MHSettings::timeout_mouse_switch,NULL);
				} 
				else if(pMouseStruct->pt.x-pMouseStruct->pt.y>screen_x-5) // Правый верхний угол
				{
					//top_position=0; // Пока дублируем левый верхний угол
					top_position=1; 
					SetTimer(MHhwnd,2,MHSettings::timeout_mouse_switch,NULL);
				}
				else if(top_position!=-1) // Ждали окончания сигнала таймера, но уехали из угла
				{
					KillTimer(MHhwnd,2);
					top_position=-1;
				}

				// Обычная обработка
				if(MHSettings::hh)
				{
					MHSettings::hh->OnMouseMove(pMouseStruct->pt.x, pMouseStruct->pt.y);
				}

				// Флаг запрещает двигать мышь, когда нажата правая кнопка
				if((right_button_down)&&(MHSettings::flag_no_move_right_mb)) return 1;
				break;

			case WM_MBUTTONDOWN:
			case WM_RBUTTONDOWN:
				right_button_down=true;
				if(MHSettings::hh)
				{
					if(MHSettings::flag_right_mb_iskey) MHKeypad::Press4(10,true);
					else MHSettings::hh->OnRDown();
					// Правую кнопку мыши подавляем
					return 1;
				}
				break;

			case WM_MBUTTONUP:
			case WM_RBUTTONUP:
				right_button_down=false;
				if(MHSettings::hh)
				{
					if(MHSettings::flag_right_mb_iskey) MHKeypad::Press4(10,false);
					else MHSettings::hh->OnRUp();
					// Правую кнопку мыши подавляем
					return 1;
				}
				break;

			case WM_LBUTTONDOWN:
				//last_left_down_time=timeGetTime();

				if((flag_left_button_key)&&(MHSettings::hh))
				{
					MHSettings::hh->OnLDown(); 
					return 1;
				}
				break; 

			case WM_LBUTTONUP:
				// Таперича включение левой кнопки мыши 
			/*	if(MHSettings::timeout_mouse_switch<timeGetTime()-last_left_down_time) // смена режима
				{
					Beep(450,100);
					
					if(flag_left_button_key)
					{
						MHSettings::hh->OnLUp();
						flag_left_button_key=false;
						return 1;
					}
					else 
					{
						flag_left_button_key=true;
						break; // позволить обработать отпускание мыши системе, мы ещё не перехватили её нажатие, не трогаем и отпускание
					}
				}
			*/
				if(flag_left_button_key) // держали недолго
				{
					MHSettings::hh->OnLUp();
					return 1;
				}
				break;

			}


		}

    }
	return CallNextHookEx(NULL,disabled,wParam,lParam);
}