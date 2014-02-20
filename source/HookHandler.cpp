#include <Windows.h>
#include "HookHandler.h"
#include "Settings.h"

//========================================================================
// Нажатие пятой клавиши при быстром движении мыши
//========================================================================
void MHookHandler::OnFastMove(LONG _dx, LONG _dy)
{
	DWORD time_now=timeGetTime();
	
	
	// Период, когда мы вычисляем движение - от 1/20 до 1/10 секунды
	if(time_now-last_button5_time<50) // недостаточно набрали данных
	{
		mouse_path_squared+=dx*dx+dy*dy;
		return;
	}

	if(time_now-last_button5_time>100) // Слишком долго ехала мышь
	{
		// Начинаем новый отсчет
		mouse_path_squared=dx*dx+dy*dy;
		last_button5_time=time_now;
		return;
	}
	
	// Наконец, попали в нужный интервал времени. Если скорость мыши нас устроит, нажимаем клавишу.
	mouse_path_squared+=dx*dx+dy*dy;
	if(mouse_path_squared>MHSettings::minimal_mouse_speed) // Потом подберём значения
	{
		MHKeypad::Press4(4, true);
		MHKeypad::Press4(4, false);
#ifdef _DEBUG
		// OutputDebugString("Быстро=>");
#endif
	}
		
	// Начинаем новый отсчет
	mouse_path_squared=dx*dx+dy*dy;;
	last_button5_time=time_now;
}

//========================================================================
// Нажатие шестой клавиши при режиме, когда левая кнопка бездействует
//========================================================================
void MHookHandler::OnLDown()
{
	MHKeypad::Press4(5, true);
}

void MHookHandler::OnLUp()
{
	MHKeypad::Press4(5, false);
}

//========================================================================
// Включение / выключение левой кнопки мыши
//========================================================================
extern bool flag_left_button_key;

void MHookHandler::TopLeftCornerTimer()
{
	if(flag_left_button_key)
	{
		flag_left_button_key=false;
		
		PlaySound( TEXT("DeviceDisconnect"), NULL, SND_ALIAS|SND_ASYNC );
		//PlaySound( TEXT("DeviceConnect"), NULL, SND_ALIAS|SND_ASYNC );
		//cout << "\a";

		//Beep(500,100);
		//Beep(400,100);
	}
	else
	{
		flag_left_button_key=true;
		PlaySound( TEXT("DeviceConnect"), NULL, SND_ALIAS|SND_ASYNC );
		//Beep(400,100);
		//Beep(500,100);
	}
}