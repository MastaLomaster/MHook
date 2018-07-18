#include <Windows.h>
#include "MHKeypad.h"
#include "Settings.h"

// У Саши Зенько роль правой кнопки выполняет колёсико!!! А настоящая правая делает только скролл
// Потом вынести это на экран параметров
#undef SASHA_ZENKO

extern bool flag_magic_left_click; // Клик мыши произведён волшебным окном, не отключать его в HookProc,

extern HWND		MHhwnd; // Нужна для установки таймера
extern LONG screen_x_real, screen_y_real; // Для определения углов экрана
extern bool flag_inside_window; // Определён в оконной процедуре, показывает, что мы внутри окна

bool flag_scroll_started=false;
bool flag_stop_mouse=false;

bool flag_left_button_key=false;
bool flag_stop_emulation=false;

bool flag_left_button_waits=false;
bool flag_right_button_waits=false;

static DWORD last_right_down_time;

//static DWORD last_screen_top_time;
int top_position=-1; // 0 - левый верхний угол, 1- правый верхний угол, -1 - убрали
//static bool mid_button_down=false;

bool right_button_down=false;
bool left_button_down=false;

// для отладки
LONG debug_x, debug_y;

//====================================================================================
// Собственно, хук 
//====================================================================================
LRESULT  CALLBACK HookProc(int disabled,WPARAM wParam,LPARAM lParam) 
{
	bool suppress_right_mb=true;
	
    if (!disabled)
	{
		MOUSEHOOKSTRUCT * pMouseStruct = (MOUSEHOOKSTRUCT *)lParam;
		if (pMouseStruct != NULL)
		{
			switch(wParam)
			{
			case WM_MOUSEMOVE:
				// для отладки
				debug_x=pMouseStruct->pt.x;
				debug_y=pMouseStruct->pt.y;

				//if(pMouseStruct->pt.x+pMouseStruct->pt.y<5) // А находимся ли мы в верхнем левом углу экрана?
				if((pMouseStruct->pt.y-pMouseStruct->pt.x>(screen_y_real-1)-5)&&
					(pMouseStruct->pt.x<screen_x_real)&&(pMouseStruct->pt.y<screen_y_real)) // А находимся ли мы в нижнем левом углу экрана? (но не на втором экране)
				{
					// Запускаем таймер, только если пришли в угол снаружи!!!
					if(0!=top_position) SetTimer(MHhwnd,2,MHSettings::timeout_mouse_switch,NULL); // 
					top_position=0;
					// это ваще не надо last_screen_top_time=timeGetTime();
				} 
				//else if(pMouseStruct->pt.x-pMouseStruct->pt.y>screen_x-5) // Правый верхний угол
				else if((pMouseStruct->pt.x+pMouseStruct->pt.y>(screen_x_real-1)+(screen_y_real-1)-5)&&
					(pMouseStruct->pt.x<screen_x_real)&&(pMouseStruct->pt.y<screen_y_real))// Правый нижний угол (но не на втором экране)
				{
					// Запускаем таймер, только если пришли в угол снаружи!!!
					if(1!=top_position) SetTimer(MHhwnd,2,MHSettings::timeout_mouse_switch,NULL);
					//top_position=0; // Пока дублируем левый верхний угол
					top_position=1; 
				}
				else if((top_position!=-1)&&(pMouseStruct->pt.y<screen_y_real)&&(pMouseStruct->pt.y>=0)&&
					(pMouseStruct->pt.x<screen_x_real)&&(pMouseStruct->pt.x>=0)) // Ждали окончания сигнала таймера, но уехали из угла
					// не рассматриваем выход за границы экрана
				{
					KillTimer(MHhwnd,2);
					top_position=-1;
				}

				// Обычная обработка - но теперь с оговоркой: двойной щелчок правой мог остановить эмуляцию!
				if(!flag_stop_emulation)
				{
					if(MHSettings::hh)
					{
						// Только в режиме 6 scroll отличается от move, остальным режимам всё равно, что вызывать
						if((right_button_down && left_button_down) || flag_scroll_started) 
						{
							MHSettings::hh->OnMouseScroll(pMouseStruct->pt.x, pMouseStruct->pt.y);
							// !!! Ошибка была !!! Кроме режима 6 никто не знает, что мышь на месте стоит !!!
nomove6:					if(flag_stop_mouse && 6==MHSettings::mode) return 1; // Теперь при скролле мышь не двигать, если скролл из окошка!
						}

						// Проверка возвращаемого значения была сделана из-за режима 8 (BFBC2)
						// Много кода пришлось перелопатить...
						// и не помогло...
						if(MHSettings::hh->OnMouseMove(pMouseStruct->pt.x, pMouseStruct->pt.y))
						{
							//pMouseStruct->pt.x=0;
							//pMouseStruct->pt.y=0;
							return 1;
						}
						
					}
				}

				// Флаг запрещает двигать мышь, когда нажата правая кнопка
				if((right_button_down)&&(MHSettings::flag_no_move_right_mb)) return 1;
				break;

			case WM_MBUTTONDOWN: // У Саши Зенько роль правой кнопки выполняет колёсико!!! А настоящая правая делает только скролл
#ifdef SASHA_ZENKO
				flag_scroll_started=true;
				return 1;
#endif
			case WM_RBUTTONDOWN: 


			//case WM_RBUTTONDOWN:
				right_button_down=true;

				
				// для отрисовки красных квадратиков
				InvalidateRect(MHhwnd,NULL,TRUE);

				if(MHSettings::hh)
				{
					// Проверим, не собираемся ли мы ткнуть в наш квадратик?
					// Что означает начало скролла
					if(flag_inside_window)
					{
						flag_scroll_started=true;
						flag_stop_mouse=true;
						if(MHSettings::hh) MHSettings::hh->Deinitialize();
						return 1; // Подавляем это нажатие
					}
					// Нет, это не начало скролла, обрабатываем по обычному
					else if(MHSettings::flag_right_mb_iskey) 
					{
						// Только если ещё не действует таймер!
						if(!flag_right_button_waits)
						{
							if(!MHKeypad::Press4(10,true)) suppress_right_mb=false; // Если было выбрано <ничего>, то нажимаем настоящую кнопку мыши
							// Это если мы при отпускании мыши жмём клавишу ещё раз (но только если мышь реально подавилась)
							else if(MHSettings::flag_right_mb_push_twice)
							{
								SetTimer(MHhwnd,4,MHSettings::timeout_mouse_click,NULL); // Это если мы при отпускании мыши жмём клавишу ещё раз
								flag_right_button_waits=true;
							}
						}
					}
					else suppress_right_mb=MHSettings::hh->OnRDown();
					
					// Заплаточка: двойной щелчок переводит левую кнопку мыши в другое состояние
					if(MHSettings::flag_right_mb_doubleclick)
					{
						DWORD this_time=timeGetTime();
						if(this_time-last_right_down_time<500) // 1/2 секунды
						{
#ifdef _DEBUG
		 OutputDebugString(L"Двойной щелчок правой\n");
#endif
							
							// Переключаем режим эмуляции
							if(flag_stop_emulation) flag_stop_emulation=false;
							else
							{
								flag_stop_emulation=true;
								if(MHSettings::hh) MHSettings::hh->Deinitialize();
							}
						}
						last_right_down_time=this_time;
					}
					


					// Правую кнопку мыши подавляем (но не всегда)
					if (suppress_right_mb) return 1;
				}
				break;

			case WM_MBUTTONUP:	// У Саши Зенько роль правой кнопки выполняет колёсико!!! А настоящая правая делает только скролл
#ifdef SASHA_ZENKO
				flag_scroll_started=false;
				return 1;
#endif
			case WM_RBUTTONUP:
				right_button_down=false;

			// для отрисовки красных квадратиков
			InvalidateRect(MHhwnd,NULL,TRUE);


				// Возможно, мы ждали отпускания мыши после двойного щелчка
				if(flag_stop_emulation) flag_stop_emulation=false;

				// Возможно, это был скролл?
				if(flag_scroll_started)
				{
					flag_scroll_started=false;
					flag_stop_mouse=false;
					if(MHSettings::hh) MHSettings::hh->Deinitialize();
					return 1; // подавляем, так как подавили нажатие
				}

				if(MHSettings::hh)
				{
					if(MHSettings::flag_right_mb_iskey) 
					{
						if(MHSettings::flag_right_mb_push_twice) 
						{
							// Только если уже не действует таймер!
							if(!flag_right_button_waits)
							{
								// Это если мы при отпускании мыши жмём клавишу ещё раз
								if(!MHKeypad::Press4(10,true)) suppress_right_mb=false; // Если было выбрано <ничего>, то нажимаем настоящую кнопку мыши 
								SetTimer(MHhwnd,4,MHSettings::timeout_mouse_click,NULL); 
								flag_right_button_waits=true;
							}
						}
						else if(!MHKeypad::Press4(10,false)) suppress_right_mb=false; // Если было выбрано <ничего>, то нажимаем настоящую кнопку мыши
					}
					else suppress_right_mb = MHSettings::hh->OnRUp();


					// Правую кнопку мыши подавляем (но не всегда)
					if (suppress_right_mb) return 1;
				}
				break;

			case WM_LBUTTONDOWN:
				left_button_down=true;

				
				// для отрисовки красных квадратиков
				InvalidateRect(MHhwnd,NULL,TRUE);

				// Проверим, не собираемся ли мы ткнуть в наш квадратик?
				// Что означает начало скролла
				//if((flag_left_button_key)&&(flag_inside_window))
				/* if((flag_inside_window)||(right_button_down))
				{
					flag_scroll_started=true;
					if(MHSettings::hh) MHSettings::hh->Deinitialize();
					return 1;
				}
				// Нет, это не начало скролла, обрабатываем по обычному
				else 
				*/
				//if((flag_left_button_key)&&(MHSettings::hh))
				// Теперь проверяем, что кнопку не мы сами инициировали из окна
				if((flag_left_button_key)&&(MHSettings::hh)&&(!flag_magic_left_click))
				{
					MHSettings::hh->OnLDown(); 
					return 1;
				}
				break; 

			case WM_LBUTTONUP:
				left_button_down=false;

				
				// для отрисовки красных квадратиков
				InvalidateRect(MHhwnd,NULL,TRUE);

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
			/*	if(flag_scroll_started)
				{
					flag_scroll_started=false;
					if(MHSettings::hh) MHSettings::hh->Deinitialize();
					return 1; // подавляем, так как подавили нажатие
				}
				else */ 
				if(flag_left_button_key&&(!flag_magic_left_click)) // держали недолго
				{
					MHSettings::hh->OnLUp();
					return 1;
				}
				if(flag_magic_left_click) flag_magic_left_click=false;
				break;

			}


		}

    }
	return CallNextHookEx(NULL,disabled,wParam,lParam);
}