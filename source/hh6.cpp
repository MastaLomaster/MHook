#include <Windows.h>
#include "hh6.h"
#include "MVector.h"
#include "MHKeypad.h"
#include "Settings.h"

extern LONG screen_x, screen_y;
extern HWND	MHhwnd;
extern bool flag_stop_mouse; // Останавливаем мышь, только начав дрег внутри окна !!!

void MHookHandler6::OnTimer()
{
	if(-1==position_mem) {KillTimer(MHhwnd,1);return;} // Клавиша не нажата, делать нечего

	// Проверяем, а не рудимент ли это, оставшийся в очереди сообщений?
	// Для этого проверим, действительнор ли истекло нужное время
	DWORD time_now=timeGetTime();
	if(time_now-last_time<MHSettings::timeout_after_move) return;

	// Действительно, время отпустить клавишу!
	KillTimer(MHhwnd,1);
	MHKeypad::Press(position_mem,false);
	MHVector::Reset(); // Вот это обязательно, иначе в том же направлении мышь не нажмёт клавишу
	// Почему-то Reset не включает перерисовку
	InvalidateRect(MHhwnd,NULL,TRUE);

	position_mem=-1;
	
}

//========================================================================
// Обработчик скролла - един для всех хендлеров
//========================================================================
void MHookHandler6::OnMouseScroll(LONG _x, LONG _y)
{
	int position;

	if(initialized) // известно последнее положение мыши
	{
		dx=(_x-last_x);
		dy=(_y-last_y);

		// Может, пятую кнопку можно нажать?
		if(MHSettings::flag_enable_speed_button) OnFastMove(dx,dy);
		position=MHVector::NewValues(dx,dy);

		if((1==position)||(3==position)) // -2=мышь подвинулась на недостаточное растояние, -1= направление не изменилось
		{
			MHKeypad::Press(position,true);
			position_mem=position;
		}

		// Таймер взводим заново при любом движении мыши (кроме скролла), если было хоть что-то нажато ранее
		// то есть -1!=position_mem
		if((-1!=position_mem)&&((1==position)||(3==position)||(-1==position)||(-2==position)))
		{
			last_time=timeGetTime();
			SetTimer(MHhwnd,1,MHSettings::timeout_after_move,NULL);
		}

		// А вот скролл идёт в обход MHVector, лишь бы dy!=0
		if(0!=dy)
		{
#ifdef _DEBUG
			OutputDebugString(L"Scroll\n");
#endif

			INPUT input={0};

			input.type=INPUT_MOUSE;
			input.mi.dx=0L;
			input.mi.dy=0L;
			input.mi.mouseData=-dy;
			input.mi.dwFlags=MOUSEEVENTF_WHEEL;
			input.mi.time=0;
			input.mi.dwExtraInfo=0;

			SendInput(1,&input,sizeof(INPUT));
		} // Scroll
	} // initialized
	
	if(!initialized) initialized=true;

	// Если мы не даём мыши двигаться, то не надо обновлять last_x и last_y !!!
	// А не даём мы мыши двигаться в HookProc метка nomove6:	
	if(!flag_stop_mouse)
	{
		last_x=_x;
		last_y=_y;
	}

	if(last_x<0) last_x=0;
	if(last_y<0) last_y=0;
	if(last_x>=screen_x) last_x=screen_x-1;
	if(last_y>=screen_y) last_y=screen_y-1;
}

//======================================================================================================
// Здесь используется только для того, чтобы сохранялся режим initialized (не терять dx, dy)
//======================================================================================================
void MHookHandler6::OnMouseMove(LONG _x, LONG _y)
{
	
	if(!initialized) initialized=true;

	last_x=_x;
	last_y=_y;
	if(last_x<0) last_x=0;
	if(last_y<0) last_y=0;
	if(last_x>=screen_x) last_x=screen_x-1;
	if(last_y>=screen_y) last_y=screen_y-1;
}

bool MHookHandler6::OnRDown()
{
	return false; // НЕ подавляйте правый клик
}

bool MHookHandler6::OnRUp()
{
	return false; // НЕ подавляйте правый клик
}

void MHookHandler6::Halt()
{
	// отожмём клавиши
	MHKeypad::Reset();
}