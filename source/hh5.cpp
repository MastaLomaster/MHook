#include <Windows.h>
#include "hh5.h"
#include "MVector.h"
#include "MHKeypad.h"
#include "Settings.h"

extern LONG screen_x, screen_y;
static bool movement_happened=false;

void MHookHandler5::OnMouseMove(LONG _x, LONG _y)
{
	int position;
	DWORD time_now;
	static int x;
		
	// При нажатой правой кнопке мыши не передаём её движения в MHVector,
	// НО! продолжаем отслеживать last_x и last_y, не сбрасывая initialized! 
	if((initialized)&&(rbutton_pressed)) // известно последнее положение мыши, правая кнопка не нажата
	{
		dx=(_x-last_x);
		dy=(_y-last_y);

		// Может, пятую кнопку можно нажать?
		if(MHSettings::flag_enable_speed_button) OnFastMove(dx,dy);
		
		if(0==MHSettings::circle_scale_factor) // Просим опознать 8 направлений
		{
			position=MHVector::NewValues(dx,dy);

			// Если не нажата правая кнопка мыши, вообще игнорируем
			if(0<=position) // -2=мышь подвинулась на недостаточное растояние, -1= направление не изменилось
			{
				MHKeypad::Press8(position,true);
				position_mem=position;
				movement_happened=true;
			}
		}
		else
		{
			x+=dx;
			if(x<0) x+=(8*MHSettings::circle_scale_factor-1); 
			if(x>(8*MHSettings::circle_scale_factor-1)) x-=(8*MHSettings::circle_scale_factor-1);
			position=x/MHSettings::circle_scale_factor;

			if(position!=position_mem)
			{
				MHKeypad::Press8(position,true);
				position_mem=position;
			}
			movement_happened=true;
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

bool MHookHandler5::OnRDown()
{
	rbutton_pressed=true;
	position_mem=-1;
	movement_happened=false;
	//MHKeypad::Reset(); // Отпускаем нажатые кнопки
	return true; // подавляйте правый клик
}

bool MHookHandler5::OnRUp()
{
	rbutton_pressed=false;
	// Начинаем новый отсчет движений
	MHVector::Reset();
	if(-1!=position_mem) MHKeypad::Press8(position_mem,false);


	// Автоклик
	if(movement_happened)
	{
		if(MHSettings::flag_mode5autoclick)
		{
			INPUT input[2]={0};
			input[0].type=INPUT_MOUSE;
			input[1].type=INPUT_MOUSE;
			input[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
			input[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;

			SendInput(2,&input[0],sizeof(INPUT));
		}
	}
	else // Нажимаем кнопку (сброс)
	{
		MHKeypad::Press4(10,true);
		MHKeypad::Press4(10,false);
	}
	return true; // подавляйте правый клик
}