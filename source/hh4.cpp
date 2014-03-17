#include <Windows.h>
#include "hh4.h"
#include "MHKeypad.h"
#include "Settings.h"
#include "MVector.h"

extern HWND		MHhwnd;

#ifdef _DEBUG
//#include <stdio.h>
	//char debug_buf[4096];
#endif

extern LONG screen_x, screen_y;
extern HBRUSH green_brush, yellow_brush;

static LONG X,Y,last_x_direction,last_y_direction;

// Какие кнопки нажимать в зависимости от направлений x_direction+1, y_direction+1;
static int button_pressed[3][3]=
{
	{7,6,5},
	{0,-1,4},
	{1,2,3}
};

void MHookHandler4::OnMouseMove(LONG _x, LONG _y)
{
	int position;
	LONG x_direction=0,y_direction=0;
		
	// При нажатой правой кнопке мыши не передаём её движения в MHVector,
	// НО! продолжаем отслеживать last_x и last_y, не сбрасывая initialized! 
	if((initialized)&&(!rbutton_pressed)) // известно последнее положение мыши, правая кнопка не нажата
	{
		dx=(_x-last_x);
		dy=(_y-last_y);

		// Может, пятую кнопку можно нажать?
		if(MHSettings::flag_enable_speed_button) OnFastMove(dx,dy);

		X+=dx; Y+=dy;
		
		// А может в два движения? (ось X не должна работать в режиме 3)
		if((true==MHSettings::flag_2moves)&&(0!=MHSettings::mode3axe))
		{
			DWORD thistime=timeGetTime();
			if(0!=x_limitation)
			{
				if((0==last_x_direction)&&(thistime-steps2timeout>50)) // Можно снимать ограничения, выждали 100 мс в середине
				{
					x_limitation=0;
				}
				else	// Ограничения продолжают действовать, таймер перевзводится, так как мышь двигалась
				{
					steps2timeout=thistime;
					// При действующих ограничпениях не давать уходить Х 
					if((X<0)&&(x_limitation>0)) X=0;
					if((X>0)&&(x_limitation<0)) X=0;
				}
			}
		} // движения в два шага


		if(0==MHSettings::mode3axe) // Не работает ли ось Х по режиму 3?
		{
			position=MHVector::NewValues(dx,0); // Движение по оси Y не передаём

			if(0<=position) // -2=мышь подвинулась на недостаточное растояние, -1= направление не изменилось
			{
				// По оси x возможно только 2 направления: 2 и 6
				if(2==position) x_direction=1;
				else x_direction=-1;
				
				position_mem=position; // нужно ли отпустить кнопку по таймеру?
			}
			else // сохраняем направление по x
			{
				x_direction=last_x_direction;
			}

			// Таймер взводим заново при любом движении мыши, если было хоть что-то нажато ранее
			// то есть -1!=position_mem
			if(-1!=position_mem)
			{
				last_time=timeGetTime();
				SetTimer(MHhwnd,1,MHSettings::timeout_after_move,NULL);
			}
		}
		else // Ось X работает в режиме 4
		{
			// Находим направление по оси x
			if(X<-3*MHSettings::deadx/2) // вышло за границы слева
			{
				x_direction=-1;
				X=-3*MHSettings::deadx/2;
			}
			else if(X<-MHSettings::deadx/2) // в пределах границ, направление -1
			{
				x_direction=-1;
			}
			else if(X>3*MHSettings::deadx/2) // вышло за границы справа
			{
				x_direction=1;
				X=3*MHSettings::deadx/2;
			}
			else if(X>MHSettings::deadx/2) // в пределах границ, направление 1
			{
				x_direction=1;
			}
			else x_direction=0; // в пределах мертвой зоны

			if((true==MHSettings::flag_2moves)&&(0!=x_direction)) x_limitation=x_direction;
		}

		if(1==MHSettings::mode3axe) // Не работает ли ось Y по режиму 3?
		{
			position=MHVector::NewValues(0,dy); // Движение по оси X не передаём

			if(0<=position) // -2=мышь подвинулась на недостаточное растояние, -1= направление не изменилось
			{
				// По оси y возможно только 2 направления: 0 и 4
				// Здесь внимание на знаки!!!
				if(0==position) y_direction=-1;
				else y_direction=1;
				
				position_mem=position; // нужно ли отпустить кнопку по таймеру?
			}
			else // сохраняем направление по x
			{
				y_direction=last_y_direction;
			}

			// Таймер взводим заново при любом движении мыши, если было хоть что-то нажато ранее
			// то есть -1!=position_mem
			if(-1!=position_mem)
			{
				last_time=timeGetTime();
				SetTimer(MHhwnd,1,MHSettings::timeout_after_move,NULL);
			}
		}
		else
		{
			// Находим направление по оси y
			if(Y<-3*MHSettings::deady/2) // вышло за границы слева
			{
				y_direction=-1;
				Y=-3*MHSettings::deady/2;
			}
			else if(Y<-MHSettings::deady/2) // в пределах границ, направление -1
			{
				y_direction=-1;
			}
			else if(Y>3*MHSettings::deady/2) // вышло за границы справа
			{
				y_direction=1;
				Y=3*MHSettings::deady/2;
			}
			else if(Y>MHSettings::deady/2) // в пределах границ, направление 1
			{
				y_direction=1;
			}
			else y_direction=0; // в пределах мертвой зоны
		}
/*
		// А может в два движения? (ось X не должна работать в режиме 3)
		if((true==MHSettings::flag_2moves)&&(0!=MHSettings::mode3axe))
		{
			if((1==last_x_direction)&&(1!=x_direction)) last_right=timeGetTime(); // Ушли из правого положения в это время
			if((-1==last_x_direction)&&(-1!=x_direction)) last_left=timeGetTime(); // Ушли из левого положения в это время

			// Можно ли занять новое положение? Прошло ли 100 мс с последнего движения?
			if((-1==x_direction)&&(-1!=last_x_direction)) // Рано ещё, иди в центр и жди без движения 100 мс
			{
				X=0;
				x_direction=0;
				last_right=timeGetTime();
			}
		}
*/		
		if((last_y_direction!=y_direction)||(last_x_direction!=x_direction)) // Нажатые кнопки поменялись
		{
			position=button_pressed[x_direction+1][y_direction+1];
			if(-1==position) 
			{
				MHKeypad::Reset(); // Отпускаем нажатые кнопки
				// Почему-то Reset не включает перерисовку
				//InvalidateRect(MHhwnd,NULL,TRUE);
			}
			else MHKeypad::Press(position,true);
		}
	}

	// Для рисования квадратиков это надо делать всегда
	InvalidateRect(MHhwnd,NULL,TRUE);

	if(!initialized) initialized=true;

	last_x=_x;
	last_y=_y;
	if(last_x<0) last_x=0;
	if(last_y<0) last_y=0;
	if(last_x>=screen_x) last_x=screen_x-1;
	if(last_y>=screen_y) last_y=screen_y-1;
	last_x_direction=x_direction;
	last_y_direction=y_direction;


#ifdef _DEBUG
	// Покажите абсолютные значения мвшиных сообщений
	//sprintf(debug_buf,"dx: %d  dy: %d\n", dx,dy);
	//OutputDebugString(debug_buf);
#endif
}

bool MHookHandler4::OnRDown()
{
	rbutton_pressed=true;
	MHKeypad::Reset(); // Отпускаем нажатые кнопки
	X=0;Y=0;
	last_x_direction=0;
	last_y_direction=0;
	// Почему-то Reset не включает перерисовку
	InvalidateRect(MHhwnd,NULL,TRUE);
	return true; // подавляйте правый клик
}

bool MHookHandler4::OnRUp()
{
	rbutton_pressed=false;
	// Начинаем новый отсчет движений
	MHVector::Reset();
	X=0;Y=0;
	last_x_direction=0;
	last_y_direction=0;
	//initialized=false;
	return true; // подавляйте правый клик
}

void MHookHandler4::OnDraw(HDC hdc, LONG window_size)
{
	RECT rect;
	LONG quad_x,quad_y;

	// Газ-жёлтым, поворот-зелёным
	if(1!=MHSettings::mode3axe) // Не работает ли ось Y по режиму 3?
	{
		quad_x=0;
		//quad_y=Y*window_size/2/(MHSettings::deady*2); // Сокращаем 
		quad_y=Y*window_size/MHSettings::deady/3; 
		rect.left=(LONG)(window_size/2+quad_x-10);
		rect.top=(LONG)(window_size/2+quad_y-10);
		rect.right=rect.left+20;
		rect.bottom=rect.top+20;
		FillRect(hdc,&rect,yellow_brush);
	}

	// поворот
	if(0!=MHSettings::mode3axe) // Не работает ли ось X по режиму 3?
	{
		quad_y=0;
		quad_x=X*window_size/MHSettings::deadx/3; 
		rect.left=(LONG)(window_size/2+quad_x-10);
		rect.top=(LONG)(window_size/2+quad_y-10);
		rect.right=rect.left+20;
		rect.bottom=rect.top+20;
		FillRect(hdc,&rect,green_brush);
	}
	
}

void MHookHandler4::OnTimer()
{
	int position;

	if(-1==position_mem) return; // Клавиша не нажата, делать нечего

	// Проверяем, а не рудимент ли это, оставшийся в очереди сообщений?
	// Для этого проверим, действительно ли истекло нужное время
	DWORD time_now=timeGetTime();
	if(time_now-last_time<MHSettings::timeout_after_move) return;

	// Действительно, время отпустить клавишу!
	// !!! Здесь не так, как в режиме 3 !!! Отпускаем только кнопки на одной оси !!!
	//MHKeypad::Press(position_mem,false);
	if(0==MHSettings::mode3axe) // Не работает ли ось Х по режиму 3?
	{
		// теперь x_direction=0;
		if(0!=last_x_direction) // направление по оси X поменялось
		{
			last_x_direction=0;
			position=button_pressed[last_x_direction+1][last_y_direction+1];
			if(-1==position) 
			{
				MHKeypad::Reset(); // Отпускаем нажатые кнопки
				// Почему-то Reset не включает перерисовку
				//InvalidateRect(MHhwnd,NULL,TRUE);
			}
			else MHKeypad::Press(position,true);
		}
	}

	if(1==MHSettings::mode3axe) // А может работает ось Y по режиму 3?
	{
		// теперь y_direction=0;
		if(0!=last_y_direction) // направление по оси Y поменялось
		{
			last_y_direction=0;
			position=button_pressed[last_x_direction+1][last_y_direction+1];
			if(-1==position) 
			{
				MHKeypad::Reset(); // Отпускаем нажатые кнопки
				// Почему-то Reset не включает перерисовку
				//InvalidateRect(MHhwnd,NULL,TRUE);
			}
			else MHKeypad::Press(position,true);
		}
	}

	MHVector::Reset(); // Вот это обязательно, иначе в том же направлении мышь не нажмёт клавишу
	// Почему-то Reset не включает перерисовку
	InvalidateRect(MHhwnd,NULL,TRUE);

	position_mem=-1;
	KillTimer(MHhwnd,1);
}