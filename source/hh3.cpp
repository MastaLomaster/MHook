#include <Windows.h>
#include "hh3.h"
#include "MVector.h"
#include "MHKeypad.h"
#include "Settings.h"

extern LONG screen_x, screen_y;
extern HWND	MHhwnd;

static int offset=0;

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
		
	// При нажатой правой кнопке мыши не передаём её движения в MHVector,
	// НО! продолжаем отслеживать last_x и last_y, не сбрасывая initialized! 
	//if((initialized)&&(!rbutton_pressed)) // известно последнее положение мыши, правая кнопка не нажата
	if(initialized) // известно последнее положение мыши, правая кнопка теперь тоже генерирует клавиши
	{
		dx=(_x-last_x);
		dy=(_y-last_y);

		
		// Может, пятую кнопку можно нажать?
		if(MHSettings::flag_enable_speed_button) OnFastMove(dx,dy);
		
		// Теперь берём цифры из поля  "быстрое движение - это со скоростью, большей..."
		// Цифры не настоящие, мы меряем не скорость, а один отсчёт, не глядя на время.
		// Но позволяет регулировать на уровне больше-меньше....
		//if((!MHSettings::flag_skip_fast)||(dx*dx+dy*dy<30)) 
		if((!MHSettings::flag_skip_fast)||(dx*dx+dy*dy<MHSettings::minimal_mouse_speed/54)) 
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
				last_time=timeGetTime();
				SetTimer(MHhwnd,1,MHSettings::timeout_after_move,NULL);
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