#include <Windows.h>
#include "Bitmap.h"
#include "Settings.h"

extern HHOOK handle;
extern LONG screen_x, screen_y;
extern int top_position; // Это в HookProc определяет, в каком углу экрана мы задержались.

LONG quad_x=0,quad_y=0; // Координаты квадратика в окне

// Традиционно оконная процедура в файле 002
//====================================================================================
// Оконная процедура
//====================================================================================
LRESULT CALLBACK WndProc(HWND hwnd,
						UINT message,
						WPARAM wparam,
						LPARAM lparam)
{
	switch (message)
	{
		case WM_CREATE:
			// Инициализируем битмапы
			MHBitmap::Init(hwnd);
			// В третьем режиме инициализируем таймер (это делается в HookHandler(3) при необходимости)
			//if(3==MHSettings::mode) SetTimer(hwnd,1,MHSettings::timeout_after_move,NULL);
			break;

		case WM_DESTROY:	// Завершение программы
			// Чистим за собой
			if((3==MHSettings::mode)||(4==MHSettings::mode)||(1==MHSettings::mode)) KillTimer(hwnd,1);
			MHKeypad::Reset();
			UnhookWindowsHookEx(handle);
			PostQuitMessage(0);
			break;

		case WM_TIMER:
			switch(wparam)
			{
			case 1: // Таймер нажатых клавиш
				if(MHSettings::hh) MHSettings::hh->OnTimer(); // Это на случай, если меняем режим, а события в очереди остались
				break;
			
			case 2: // Таймер угла экрана
				KillTimer(hwnd,2); // Первым делом таймер прибить
				switch(top_position)
				{
				case 0:
					top_position=-1;
					if(MHSettings::hh) MHSettings::hh->TopLeftCornerTimer();
					break;
				case 1:
					top_position=-1;
					if(MHSettings::SettingsDialogue(hwnd))
					{
						// Чистим за собой - возможно, излишне
						if((3==MHSettings::mode)||(4==MHSettings::mode)||(1==MHSettings::mode)) KillTimer(hwnd,1);
						MHKeypad::Reset();
						UnhookWindowsHookEx(handle);
						PostQuitMessage(0);
					}
					// Тут будет вывод диалога настроек
					break;
				}
				//Beep(450,100);
				break;
			}
			
			break;

		case WM_DISPLAYCHANGE:
			screen_x=LOWORD(lparam);
			screen_y=HIWORD(lparam);
			break;

		case WM_PAINT:
			PAINTSTRUCT ps;
			HDC hdc;
			hdc=BeginPaint(hwnd,&ps);
			
			// Подсветить нажатые кнопки
			MHBitmap::OnDraw(hdc,MHSettings::GetPosition());

			if(MHSettings::hh) MHSettings::hh->OnDraw(hdc,200);

			//if((MHposition>-2)&&(MHposition<4)) MHBitmap::OnDraw(hdc,4,MHposition);

/*			RECT rect;

			// Квадратик с мышью
			//xpercent/100.0f*xsize
			//ypercent/100.0f*ysize
			rect.left=(LONG)(MH_WINDOW_SIZE/2+quad_x-10);
			rect.top=(LONG)(MH_WINDOW_SIZE/2+quad_y-10);
			rect.right=rect.left+20;
			rect.bottom=rect.top+20;
			FillRect(hdc,&rect,(HBRUSH)GetStockObject(GRAY_BRUSH));
			*/
			
			EndPaint(hwnd,&ps);
			break;


			break;

		default: // Сообщения обрабатываются системой
			return DefWindowProc(hwnd,message,wparam,lparam);
	}

return 0; // сами обработали
}