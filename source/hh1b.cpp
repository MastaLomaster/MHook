#include <Windows.h>
#include "hh1.h"
#include "MVector.h"
#include "MHKeypad.h"
#include "Settings.h"

#ifdef _DEBUG
//#include <stdio.h>
	//char debug_buf[4096];
#endif

extern HWND		MHhwnd;

extern LONG screen_x, screen_y;


void MHookHandler1::OnMouseMove(LONG _x, LONG _y)
{
	int position;
		
	// При нажатой правой кнопке мыши не передаём её движения в MHVector,
	// НО! продолжаем отслеживать last_x и last_y, не сбрасывая initialized! 
	// теперь по-новому: если есть прилипание к оси, то можно только противоположное
	// или то же. Поворачивать нельзя.
	if(initialized)
	{
		dx=(_x-last_x);
		dy=(_y-last_y);

		// Может, пятую кнопку можно нажать?
		if(MHSettings::flag_enable_speed_button) OnFastMove(dx,dy);

		position=MHVector::NewValues(dx,dy);

		if(!rbutton_pressed) // известно последнее положение мыши, правая кнопка не нажата
		{
			// Новая опция - из конца в конец в два движения
			if((MHSettings::flag_2moves_mode1)&&(4==MHSettings::GetNumPositions()))
			{
				if(0<=position) // новое направление
				{
					// Это противоположное направление?
					if(((0==position)&&(2==position_mem)||
						(1==position)&&(3==position_mem)||
						(2==position)&&(0==position_mem)||
						(3==position)&&(1==position_mem)))
					{
						// 1. (при невыставленном флаге противоположного направления)
						if(false==flag_opposite_direction)
						{
							// запомним время, отпустим кнопки, запомним направление (ниже)
							opposite_time=timeGetTime();
							MHKeypad::Reset();
							flag_opposite_direction=true;
							position_mem=position;
							// Почему-то Reset не включает перерисовку
							InvalidateRect(MHhwnd,NULL,TRUE);
						}
						else // при выставленном флаге противоположного направления
						{
							/*// Это возможно только тогда, когда мы вильнули в сторону и снова вернулись на нужное направление
							// А это равносильно тому, что происходит в position==-1
							if(timeGetTime()-opposite_time>50) // была пауза, можно идти в противоположном направлении
							{
								MHKeypad::Press(position_mem,true);
								flag_opposite_direction=false;
							}
							else opposite_time=timeGetTime(); // паузы в 50 мс неподвижности не было, перевзводим */
							MHKeypad::Press(position,true);
							flag_opposite_direction=false;
							position_mem=position;
						}
					}
					else // не противоположное (position_mem содержит всё что угодно),
					{
						if(true==flag_opposite_direction) 
						{
							// ждём выхода в сторону position_mem, а по дороге завернули в сторону
							// только здесь возможен поворот в сторону !!!
							// Но только если прошло время обездвиженности!!!
							if(timeGetTime()-opposite_time>100) // была пауза, можно идти в противоположном направлении
							{
								MHKeypad::Press(position,true);
								flag_opposite_direction=false;
								position_mem=position;
							}
							else 
							{
								MHVector::Reset(); // Не надо больше слать -1
								opposite_time=timeGetTime(); // паузы в 50 мс неподвижности не было, перевзводим
							}
							//MHKeypad::Press(position,true);
							//flag_opposite_direction=false; // уход в сторону - сброс ожиданий
						}
						else
						{
							// не ждём выхода в сторону position_mem. 
							// Нажимаем, только если -1==position_mem (при прилипании)
							if((-1==position_mem)||(MHSettings::flag_change_direction_ontheway))
							{
								MHKeypad::Press(position,true);
								position_mem=position;
							}
						}
					
					} // не противоположное направление
				}
				else if(-1==position)
				{
					// Обрабатываем, только если не довели до конца
					if(flag_opposite_direction)
					{
						if(timeGetTime()-opposite_time>100) // была пауза, можно идти в противоположном направлении
						{
							MHKeypad::Press(position_mem,true);
							flag_opposite_direction=false;
						}
						else opposite_time=timeGetTime(); // паузы в 50 мс неподвижности не было, перевзводим
					}
				}
				// else осталось только -2, игнорируем
			}
			else
			{
				// По-старому, как было до модификации flag_2moves_mode1
				if(0<=position) // -2=мышь ваще не двигалась, -1= направление не изменилось
				{
					MHKeypad::Press(position,true);
				}
			}
		} // правая кнопка не нажата
		else
		{
			// обработка правой кнопки мыши
			if(0<=position) // -2=мышь подвинулась на недостаточное растояние, -1= направление не изменилось
			{
				MHKeypad::Press(position,true, 6); // По движению правой кнопки нажимать альтернативные клавиши
				 
				position_mem=position;
			}
			// Таймер взводим заново при любом движении мыши, если было хоть что-то нажато ранее
			// то есть -1!=position_mem
			if(-1!=position_mem)
			{
				last_time=timeGetTime();
				SetTimer(MHhwnd,1,MHSettings::timeout_after_move,NULL);
			}
			
		} // правая кнопка нажата
	} // if initialized
		

	if(!initialized) initialized=true;

	last_x=_x;
	last_y=_y;
	if(last_x<0) last_x=0;
	if(last_y<0) last_y=0;
	if(last_x>=screen_x) last_x=screen_x-1;
	if(last_y>=screen_y) last_y=screen_y-1;

#ifdef _DEBUG
	// Покажите абсолютные значения мвшиных сообщений
	//sprintf(debug_buf,"dx: %d  dy: %d\n", dx,dy);
	//OutputDebugString(debug_buf);
#endif
}

void MHookHandler1::OnRDown()
{
	rbutton_pressed=true;
	MHKeypad::Reset(); // Отпускаем нажатые кнопки
	// Начинаем новый отсчет движений
	MHVector::Reset();

	// Почему-то Reset не включает перерисовку
	InvalidateRect(MHhwnd,NULL,TRUE);
}

void MHookHandler1::OnRUp()
{
	rbutton_pressed=false;
	MHKeypad::Reset(6); // Отпускаем нажатые кнопки (в альтернативных кодах клавиш, смещение 6)
	// Начинаем новый отсчет движений
	MHVector::Reset();

	flag_opposite_direction=false;
	position_mem=-1;

}

extern LONG quad_x,quad_y; // Координаты квадратика в окне

void MHookHandler1::OnDraw(HDC hdc, LONG window_size)
{
	RECT rect;

			// Квадратик с мышью
			//xpercent/100.0f*xsize
			//ypercent/100.0f*ysize
			rect.left=(LONG)(window_size/2+quad_x-10);
			rect.top=(LONG)(window_size/2+quad_y-10);
			rect.right=rect.left+20;
			rect.bottom=rect.top+20;
			FillRect(hdc,&rect,(HBRUSH)GetStockObject(GRAY_BRUSH));
}

void MHookHandler1::OnTimer()
{
	if(-1==position_mem) return; // Клавиша не нажата, делать нечего

	// Проверяем, а не рудимент ли это, оставшийся в очереди сообщений?
	// Для этого проверим, действительнор ли истекло нужное время
	DWORD time_now=timeGetTime();
	if(time_now-last_time<MHSettings::timeout_after_move) return;

	// Действительно, время отпустить клавишу!
	MHKeypad::Press(position_mem,false,6); // По движению правой кнопки нажимать альтернативные клавиши
	MHVector::Reset(); // Вот это обязательно, иначе в том же направлении мышь не нажмёт клавишу
	// Почему-то Reset не включает перерисовку
	InvalidateRect(MHhwnd,NULL,TRUE);

	position_mem=-1;
	KillTimer(MHhwnd,1);
}

void MHookHandler1::Halt()
{
	// отожмём клавиши
	if(rbutton_pressed) MHKeypad::Reset(6);
		
	// персонально для нас (в отличие от HaltGeneral базового класса):
	flag_opposite_direction=false;
}