#include <Windows.h>
#include "hh5a.h"
#include "MVector.h"
#include "MHKeypad.h"
#include "Settings.h"

#ifdef _DEBUG
#include <stdio.h>
static char debug_buf[4096];
#endif

extern LONG screen_x, screen_y;
extern HWND	MHhwnd;

static int offset=0;
static bool movement_happened=false;

void MHookHandler5a::OnTimer()
{
	if(-1==position_mem) 
	{
		KillTimer(MHhwnd,1);
		return; // Клавиша не нажата, делать нечего
	}

	if(-2==position_mem) 
	{
		KillTimer(MHhwnd,1);
		position_mem=-1;
		MHVector::Reset(); // По истечению таймера движение в том же направлении оценивается, как нажатие.
		return; // Клавиша не нажата, делать нечего
	}

	// Проверяем, а не рудимент ли это, оставшийся в очереди сообщений?
	// Для этого проверим, действительнор ли истекло нужное время
	DWORD time_now=timeGetTime();
	if(time_now-last_time<MHSettings::timeout_after_move) return;

	// Действительно, время отпустить клавишу!
	MHKeypad::Press(position_mem,false,offset);
	MHVector::Reset(); // Вот это обязательно, иначе в том же направлении мышь не нажмёт клавишу
	// Почему-то Reset не включает перерисовку
	InvalidateRect(MHhwnd,NULL,TRUE);

	position_mem=-1;
	KillTimer(MHhwnd,1);
}

void MHookHandler5a::OnMouseMove(LONG _x, LONG _y)
{
	int position;
		
	// При нажатой правой кнопке мыши не передаём её движения в MHVector,
	// НО! продолжаем отслеживать last_x и last_y, не сбрасывая initialized! 
	//if((initialized)&&(!rbutton_pressed)) // известно последнее положение мыши, правая кнопка не нажата
	if(initialized) // известно последнее положение мыши, правая кнопка теперь тоже генерирует клавиши
	{
		dx=(_x-last_x);
		dy=(_y-last_y);

		// Может, пятую кнопку можно нажать?
		if(MHSettings::flag_enable_speed_button) OnFastMove(dx,dy);
		position=MHVector::NewValues(dx,dy);


		// Если не нажата правая кнопка мыши, вообще игнорируем
		if((0<=position)&&(rbutton_pressed)) // -2=мышь подвинулась на недостаточное растояние, -1= направление не изменилось
		{
			movement_happened=true; // Это было не просто нажатие правой кнопки, а нажатие с движением

#ifdef _DEBUG
	sprintf(debug_buf,"Position: %d dxdy: %d %d  xy: %d %d last_xy: %d %d\n", position, dx, dy, _x, _y, last_x, last_y);
	OutputDebugString(debug_buf);
#endif

			// выбираем один из 8 умений
			if((6==offset)||(11==offset))
			{
				// Возможно, таймер ещё не отпустил другую клавишу
				if(-1!=position_mem) MHKeypad::Press(position_mem,false,offset);

				MHKeypad::Press(position,true,offset);
				position_mem=position;
				//MHVector::Reset();
			}
			else if((1==position)||(3==position))
			{
				// Возможно, таймер ещё не отпустил другую клавишу
				if(-1!=position_mem) MHKeypad::Press(position_mem,false,offset);
				position_mem=-1;

				if(1==position)
				{
					offset=6; 
					position_mem=-2; // Спец.значение для таймера, который сделает //MHVector::Reset();
				} // Мы готовы к нажатиям в том же направлении
				if(3==position) 
				{
					offset=11;
					position_mem=-2; // Спец.значение для таймера, который сделает //MHVector::Reset();
				} // Мы готовы к нажатиям в том же направлении
				
			} 
		}
		else
		{
#ifdef _DEBUG
	sprintf(debug_buf,"-----------: %d dxdy: %d %d  xy: %d %d last_xy: %d %d\n", position, dx, dy, _x, _y, last_x, last_y);
	OutputDebugString(debug_buf);
#endif
		}


		// Таймер взводим заново при любом движении мыши, если было хоть что-то нажато ранее
		// то есть -1!=position_mem
		if(-1!=position_mem)
		{
			last_time=timeGetTime();
			SetTimer(MHhwnd,1,MHSettings::timeout_after_move,NULL);
		}
	}

	if(!initialized) initialized=true;

	// О, здесь крылась ужасная ошибка!
	// Если мы не даём мыши двигаться, то не надо обновлять last_x и last_y !!!
	if(!((rbutton_pressed)&&(MHSettings::flag_no_move_right_mb)))
	{
		last_x=_x;
		last_y=_y;
	}
	if(last_x<0) last_x=0;
	if(last_y<0) last_y=0;
	if(last_x>=screen_x) last_x=screen_x-1;
	if(last_y>=screen_y) last_y=screen_y-1;
}

void MHookHandler5a::OnRDown()
{
	KillTimer(MHhwnd,1);
	rbutton_pressed=true;
	MHKeypad::Reset(offset); // Отпускаем нажатые кнопки
	MHVector::Reset();
	position_mem=-1;
	offset=0;
	movement_happened=false;

}

void MHookHandler5a::OnRUp()
{
	KillTimer(MHhwnd,1);
	rbutton_pressed=false;
	MHKeypad::Reset(offset); // Отпускаем нажатые кнопки
	// Начинаем новый отсчет движений
	MHVector::Reset();
	position_mem=-1;
	offset=0;

	if(!movement_happened)
	{
		// Кнопку нажали, но мышь не двигали. Надо бы выполнить что-то
	}

}

void MHookHandler5a::Halt()
{
	KillTimer(MHhwnd,1);
	rbutton_pressed=false;

	// отожмём клавиши
	MHKeypad::Reset(offset);
	position_mem=-1;
	offset=0;
}