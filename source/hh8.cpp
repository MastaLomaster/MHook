#include <Windows.h>
#include "hh8.h"
#include "Settings.h"

#ifdef _DEBUG
#include <stdio.h>
static TCHAR debug_buf[4096];
#endif

#define bfbc2_mspeed 20

bool flag_mode8_ignore_fast_mouse=false;

static LONG game_x, game_y;
extern HWND game_hwnd; // Это в Settings
extern HWND		MHhwnd;
extern LONG screen_x, screen_y;
extern HBRUSH green_brush, yellow_brush;

static RECT game_rect;
static POINT game_p;

int MHookHandler8::OnMouseMove(LONG _x, LONG _y)
{
	float Qspeed;
	int retcode=0;
	static DWORD time_now,last_time; // Только для отслеживания быстрых движений

	dx=(_x-last_x);
	dy=(_y-last_y);

	if(flag_mode8_ignore_fast_mouse)
	{
		time_now=timeGetTime();

		if(initialized)
		{
			// Просто игнорируем быстрое движение мыши (бесконечный коврик, содрано из режима 4.3)
			Qspeed=100.0f*(dx*dx+dy*dy)/(time_now-last_time); // пикселов в квадрате за 100 мс
			if(Qspeed>MHSettings::minimal_mouse_speed) // Это элемент быстрого движения!!!
				retcode=1;
#ifdef _DEBUG
		swprintf(debug_buf,_countof(debug_buf),L"Qspeed: %f \n", Qspeed);
		OutputDebugString(debug_buf);
#endif
		}
		else initialized=true;

		last_x=_x;
		last_y=_y;
		if(last_x<0) last_x=0;
		if(last_y<0) last_y=0;
		if(last_x>=screen_x) last_x=screen_x-1;
		if(last_y>=screen_y) last_y=screen_y-1;

		last_time=time_now;

		return retcode;
	}
	else
	{
		// Режим продолжения движения мыши на краю окна
		if(initialized)
		{
			// Просто запомним координаты. Главное - в обработке таймера
			game_x=_x, game_y=_y;
		}
		else
		{
			SetTimer(MHhwnd,1,25,NULL); // 1/40 секунды
			//SetTimer(MHhwnd,1,500,NULL); // 1/2 секунды - отладка
			initialized=true;
		}

		// Для рисования квадратиков это надо делать всегда
		InvalidateRect(MHhwnd,NULL,TRUE);
	}

	return 0; // Новая директива ВЦСПС
}

void MHookHandler8::OnTimer()
{
	// проверки режима не делаем, ибо он вызывается только при режиме продолжения движения мыши на границе окна
	
	
	LONG x_direction=0, y_direction=0;
	game_p.x=game_x;
	game_p.y=game_y;

	// Для отладки
	//KillTimer(MHhwnd,1);

	// Оконный режим
	if(game_hwnd)
	{
		// Велико ли окно?
		GetClientRect(game_hwnd,&game_rect);

		// Переведём экранные координаты в оконные
		ScreenToClient(game_hwnd,&game_p);
	}
	else // Полноэкранный режим
	{
		game_rect.bottom=screen_y-1;
		game_rect.right=screen_x-1;
	}
	
	// Находимся ли на границе (окна или экрана)
	if(game_p.x<3) x_direction=-1;
	else if(game_p.x>game_rect.right-3) x_direction=1;

	if(game_p.y<3) y_direction=-1;
	else if(game_p.y>game_rect.bottom-3) y_direction=1;
#ifdef _DEBUG
		//swprintf(debug_buf,_countof(debug_buf),L"dx: %ld  dy: %ld\n bottom: %ld", game_p.x,game_p.y,game_rect.bottom);
		//OutputDebugString(debug_buf);
#endif
	if((0!=x_direction)||(0!=y_direction))
	{
		INPUT input={0};
		input.type=INPUT_MOUSE;
		input.mi.dx=x_direction*bfbc2_mspeed;
		input.mi.dy=y_direction*bfbc2_mspeed;
		input.mi.mouseData=0; // Нужно для всяких колёс прокрутки 
		//input.mi.dwFlags=MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_MOVE;
		input.mi.dwFlags=MOUSEEVENTF_MOVE;
		input.mi.time=0;
		input.mi.dwExtraInfo=0;
		SendInput(1,&input,sizeof(INPUT));
	}
	
	// Для отладки
	//SetTimer(MHhwnd,1,100,NULL); // 1/10 секунды
}

void MHookHandler8::Halt()
{
	KillTimer(MHhwnd,1);
}

// Творчески слизано из mode4
void MHookHandler8::OnDraw(HDC hdc, LONG window_size)
{
	RECT rect;
	LONG quad_x,quad_y;

	if(flag_mode8_ignore_fast_mouse) return;

	// Y-жёлтым, X-зелёным
	quad_x=window_size/2-10;
	quad_y=(((float)game_p.y)/game_rect.bottom)*window_size-10;

	rect.left=(LONG)(quad_x);
	rect.top=(LONG)(quad_y);
	rect.right=rect.left+20;
	rect.bottom=rect.top+20;
	FillRect(hdc,&rect,yellow_brush);

#ifdef _DEBUG
//		swprintf(debug_buf,_countof(debug_buf),L"dx: %ld  dy: %ld\n bottom: %ld", game_p.x,game_p.y,game_rect.bottom);
	//	OutputDebugString(debug_buf);
#endif

	quad_y=window_size/2-10;
	quad_x=(((float)game_p.x)/game_rect.right)*window_size-10;
	rect.left=(LONG)(quad_x);
	rect.top=(LONG)(quad_y);
	rect.right=rect.left+20;
	rect.bottom=rect.top+20;
	FillRect(hdc,&rect,green_brush);
}
