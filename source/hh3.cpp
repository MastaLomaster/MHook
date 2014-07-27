#include <Windows.h>
#include "hh3.h"
#include "MVector.h"
#include "MHKeypad.h"
#include "Settings.h"

#ifdef _DEBUG
	#include <stdio.h>
	#include <math.h>
	TCHAR debug_buf[4096];
#endif

bool IsOpposite(int old_direction, int new_direction); // Определена в hh1.cpp

extern LONG screen_x, screen_y;
extern HWND	MHhwnd;

static int offset=0, last_any_position=-1,
	serial_fasts=0, oast_allowed=0, b2st_allowed=0;

static DWORD last_any_time=0, // Время последнего пперемещения мыши
	last_fast_time=0,
	last_bast_time=0;

void MHookHandler3::OnTimer()
{
	if(-1==position_mem) {KillTimer(MHhwnd,1);return;} // Клавиша не нажата, делать нечего

	// Проверяем, а не рудимент ли это, оставшийся в очереди сообщений?
	// Для этого проверим, действительнор ли истекло нужное время
	DWORD time_now=timeGetTime();
	if(time_now-last_time<MHSettings::timeout_after_move) return;

	// Действительно, время отпустить клавишу!
	KillTimer(MHhwnd,1);
	MHKeypad::Press(position_mem,false,offset);
	MHVector::Reset(); // Вот это обязательно, иначе в том же направлении мышь не нажмёт клавишу
	// Почему-то Reset не включает перерисовку
	InvalidateRect(MHhwnd,NULL,TRUE);

	position_mem=-1;
	
}

void MHookHandler3::OnMouseMove(LONG _x, LONG _y)
{
	int position;
	DWORD time_now=timeGetTime();
	float Qspeed;
	int IsOp;

	// При нажатой правой кнопке мыши не передаём её движения в MHVector,
	// НО! продолжаем отслеживать last_x и last_y, не сбрасывая initialized! 
	//if((initialized)&&(!rbutton_pressed)) // известно последнее положение мыши, правая кнопка не нажата
	if(initialized) // известно последнее положение мыши, правая кнопка теперь тоже генерирует клавиши
	{
		dx=(_x-last_x);
		dy=(_y-last_y);

		
		// Может, пятую кнопку можно нажать?
		if(MHSettings::flag_enable_speed_button) OnFastMove(dx,dy);


		// Чтобы не запутаться - здесь кусок без контроля скорости
		if(!MHSettings::flag_skip_fast)
		{
			position=MHVector::NewValues(dx,dy);

			if(0<=position) // -2=мышь подвинулась на недостаточное растояние, -1= направление не изменилось
			{
				MHKeypad::Press(position,true,offset);
				position_mem=position;
			}
			// Таймер взводим заново при любом движении мыши, если было хоть что-то нажато ранее
			// то есть -1!=position_mem
			if(-1!=position_mem)
			{
				last_time=time_now;
				SetTimer(MHhwnd,1,MHSettings::timeout_after_move,NULL);
			}
		}
		else // а вот здесь нажимаем не всегда
		{
			// для начала вычислим скорость
			Qspeed=100.0f*(dx*dx+dy*dy)/(time_now-last_any_time); // пикселов в квадрате за 100 мс
			if(Qspeed>MHSettings::minimal_mouse_speed) // Это элемент быстрого движения!!!
			{
				serial_fasts++;
				if(serial_fasts>2) oast_allowed=3; else oast_allowed=0; // Мы достаточно набрали fast, чтобы игнорировать хвост

				// Безусловно ничего не жмём, запоминаем время
				last_fast_time=time_now;
#ifdef _DEBUG
				swprintf(debug_buf,_countof(debug_buf),L"FAST dx: %d  dy: %d Qspeed: %f time: %lu\n", dx,dy,Qspeed, time_now-last_any_time);
				OutputDebugString(debug_buf);
#endif
			}
			else // Скорость низкая, проверяем дополнительно
			{
				serial_fasts=0; // Последовательность FAST-движений прервана.

				// 2.1. Может, это хвост быстрого движения (OAST)?
				// Чиста нулевой OAST - это BAST !!!!
				//if((time_now-last_fast_time<80)&&((dx!=0)||(dy!=0)))
				if((time_now-last_fast_time<80)&&(oast_allowed))
				{
					oast_allowed--;
#ifdef _DEBUG
				swprintf(debug_buf,_countof(debug_buf),L"OAST dx: %d  dy: %d Qspeed: %f time: %lu ftime: %lu\n", dx,dy,Qspeed, time_now-last_any_time, time_now-last_fast_time);
				OutputDebugString(debug_buf);
#endif
				}
				// 2.2 а,б - начало, запоминаем его время 
				//else if(((Qspeed<2.0f)&&(time_now-last_any_time>30))||((0==dx)&&(0==dy)))
				else if((Qspeed<2.0f)&&(time_now-last_any_time>30))
				{
					last_bast_time=time_now;
					b2st_allowed=2;
#ifdef _DEBUG
				swprintf(debug_buf,_countof(debug_buf),L"BAST dx: %d  dy: %d Qspeed: %f time: %lu ftime: %lu\n", dx,dy,Qspeed, time_now-last_any_time, time_now-last_fast_time);
				OutputDebugString(debug_buf);
#endif
				}
				// 2.2. в: возможно, разгоняемся
				else if((time_now-last_bast_time<50)&&(b2st_allowed))
				{
					b2st_allowed--;
#ifdef _DEBUG
				swprintf(debug_buf,_countof(debug_buf),L"B2ST dx: %d  dy: %d Qspeed: %f time: %lu ftime: %lu\n", dx,dy,Qspeed, time_now-last_any_time, time_now-last_fast_time);
				OutputDebugString(debug_buf);
#endif
				}
				else // Всё-таки настояли на SLOW! (теперь сюда попадают и dx==0, dy==0)
				{
					// Всё-таки обрабатываем движение (копипаст из начала)
					position=MHVector::NewValues(dx,dy);
					IsOp=IsOpposite(last_any_position, position);
					if(position>=0) last_any_position=position;

					if(IsOp) // Поменяли направление - нужно считать этот отсчёт - BAST (B-ST в диагностике)
					{
						last_bast_time=time_now;
						b2st_allowed=2;
						MHVector::Reset(); // Вот это обязательно, иначе в том же направлении мышь не нажмёт клавишу
						InvalidateRect(MHhwnd,NULL,TRUE); // Правильные квадратики
#ifdef _DEBUG
				swprintf(debug_buf,_countof(debug_buf),L"OPPO dx: %d  dy: %d Qspeed: %f time: %lu ftime: %lu\n", dx,dy,Qspeed, time_now-last_any_time, time_now-last_fast_time);
				OutputDebugString(debug_buf);
#endif

					}
					else
					{
						// Только сейчас считаем себя SLOW
						if(0<=position) // -2=мышь подвинулась на недостаточное растояние, -1= направление не изменилось
						{
							MHKeypad::Press(position,true,offset);
							position_mem=position;
						}
						// Таймер взводим заново при любом движении мыши, если было хоть что-то нажато ранее
						// то есть -1!=position_mem
						if(-1!=position_mem)
						{
							last_time=time_now;
							SetTimer(MHhwnd,1,MHSettings::timeout_after_move,NULL);
						}
#ifdef _DEBUG
						swprintf(debug_buf,_countof(debug_buf),L"SLOW dx: %d  dy: %d Qspeed: %f time: %lu ftime: %lu pos: %d IsOp: %d\n", dx,dy,Qspeed, time_now-last_any_time, time_now-last_fast_time, position, IsOp);
						OutputDebugString(debug_buf);
#endif
					}
				} // обрабатываем движение



			} // низкая скорость

		}

/*
		// Теперь берём цифры из поля  "быстрое движение - это со скоростью, большей..."
		// Цифры не настоящие, мы меряем не скорость, а один отсчёт, не глядя на время.
		// Но позволяет регулировать на уровне больше-меньше....
		//if((!MHSettings::flag_skip_fast)||(dx*dx+dy*dy<30)) 
		if((!MHSettings::flag_skip_fast)||(dx*dx+dy*dy<MHSettings::minimal_mouse_speed/54)) 
		{
#ifdef _DEBUG
		//	swprintf(debug_buf,_countof(debug_buf),L"SLOW: dx: %d  dy: %d time: %ul\n", dx,dy,time_now-last_fast_time);
		//	OutputDebugString(debug_buf);
#endif
			// нажатие разрешено при flag_skip_test только, если успокоились после быстрого движения
			//if((!MHSettings::flag_skip_fast)||(time_now-last_fast_time>1000))
			{
				position=MHVector::NewValues(dx,dy);

#ifdef _DEBUG
				//swprintf(debug_buf,_countof(debug_buf),L"SLOW dx: %d  dy: %d Qspeed: %f time: %lu pos: %d\n", dx,dy,sqrt((float)(dx*dx+dy*dy))/(time_now-last_any_time), time_now-last_fast_time, position);
				//swprintf(debug_buf,_countof(debug_buf),L"SLOW dx: %d  dy: %d Qspeed: %f time: %lu pos: %d\n", dx,dy,5625.0*(dx*dx+dy*dy)/(time_now-last_any_time)/(time_now-last_any_time), time_now-last_fast_time, position);
				swprintf(debug_buf,_countof(debug_buf),L"SLOW dx: %d  dy: %d Qspeed: %f time: %lu ftime: %lu pos: %d\n", dx,dy,100.0f*(dx*dx+dy*dy)/(time_now-last_any_time), time_now-last_any_time, time_now-last_fast_time, position);
				OutputDebugString(debug_buf);
#endif
				if(0<=position) // -2=мышь подвинулась на недостаточное растояние, -1= направление не изменилось
				{
					MHKeypad::Press(position,true,offset);
					position_mem=position;
				}
				// Таймер взводим заново при любом движении мыши, если было хоть что-то нажато ранее
				// то есть -1!=position_mem
				if(-1!=position_mem)
				{
					last_time=time_now;
					SetTimer(MHhwnd,1,MHSettings::timeout_after_move,NULL);
				}
			} // нажатие разрешено
		}
		else // быстрое движение и флаг быстрого движения выставлен
		{
			last_fast_time=time_now;

		}
*/
		last_any_time=time_now;

	}

	if(!initialized) initialized=true;

	last_x=_x;
	last_y=_y;
	if(last_x<0) last_x=0;
	if(last_y<0) last_y=0;
	if(last_x>=screen_x) last_x=screen_x-1;
	if(last_y>=screen_y) last_y=screen_y-1;
}

bool MHookHandler3::OnRDown()
{
	KillTimer(MHhwnd,1);
	rbutton_pressed=true;
	MHKeypad::Reset(offset); // Отпускаем нажатые кнопки
	MHVector::Reset();
	position_mem=-1;
	offset=6;
	return true; // подавляйте правый клик
}

bool MHookHandler3::OnRUp()
{
	KillTimer(MHhwnd,1);
	rbutton_pressed=false;
	MHKeypad::Reset(offset); // Отпускаем нажатые кнопки
	// Начинаем новый отсчет движений
	MHVector::Reset();
	position_mem=-1;
	offset=0;
	return true; // подавляйте правый клик
}

void MHookHandler3::Halt()
{
	// отожмём клавиши
	MHKeypad::Reset(offset);
	offset=0;
}