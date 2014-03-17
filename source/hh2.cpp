#include <Windows.h>
#include "hh2.h"
#include "MVector.h"
#include "MHKeypad.h"
#include "Settings.h"

extern LONG screen_x, screen_y;
extern HWND		MHhwnd;


void MHookHandler2::OnMouseMove(LONG _x, LONG _y)
{
	int position;
		
	// При нажатой правой кнопке мыши не передаём её движения в MHVector,
	// НО! продолжаем отслеживать last_x и last_y, не сбрасывая initialized! 
	if(initialized) // известно последнее положение мыши, правая кнопка не нажата
	{
		dx=(_x-last_x);
		dy=(_y-last_y);

		// Может, пятую кнопку можно нажать?
		if(MHSettings::flag_enable_speed_button) OnFastMove(dx,dy);
		position=MHVector::NewValues(dx,dy);

		if(0<=position) // -2=мышь ваще не двигалась, -1= направление не изменилось
		{
			// Запомнить новое направление, которое пригодится нам при нажатии клавиши
			position_mem=position;

			// В этом режиме - по другому
			if(rbutton_pressed) // Если поменяли направление при нажатой правой кнопке мыши - нажать клавишу
			{
				MHKeypad::Press(position,true);
			}
			else
			{
				// просто перерисовать квадратик
				InvalidateRect(MHhwnd,NULL,TRUE);
			}
		}
	}

	if(!initialized) initialized=true;

	last_x=_x;
	last_y=_y;
	if(last_x<0) last_x=0;
	if(last_y<0) last_y=0;
	if(last_x>=screen_x) last_x=screen_x-1;
	if(last_y>=screen_y) last_y=screen_y-1;
}

bool MHookHandler2::OnRDown()
{
	rbutton_pressed=true;
	MHKeypad::Press(position_mem,true);
	return true; // подавляйте правый клик
}

bool MHookHandler2::OnRUp()
{
	rbutton_pressed=false;
	MHKeypad::Reset(); // Отпускаем нажатые кнопки
	return true; // подавляйте правый клик
}