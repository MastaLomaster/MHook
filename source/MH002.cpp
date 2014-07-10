#include <Windows.h>
#include "Bitmap.h"
#include "Settings.h"

bool flag_inside_window=false;

extern HHOOK handle;
extern LONG screen_x, screen_y;
extern int top_position; // Это в HookProc определяет, в каком углу экрана мы задержались.
extern bool flag_left_button_waits;
extern bool flag_right_button_waits;

LONG quad_x=0,quad_y=0; // Координаты квадратика в окне

static TRACKMOUSEEVENT tme={sizeof(TRACKMOUSEEVENT),TME_LEAVE,0,HOVER_DEFAULT};

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
		// Пара событий, по которым мы определяем, находится ли мышь над окном
		case WM_MOUSEMOVE:
			if(!flag_inside_window)
			{
				flag_inside_window=true;
				TrackMouseEvent(&tme);
			}
			break;

		case WM_MOUSELEAVE:
			flag_inside_window=false;
			break;

		case WM_CREATE:
			// Инициализируем битмапы
			MHBitmap::Init(hwnd);
			// Дополняем tme хендлером окна
			tme.hwndTrack=hwnd;
			// Содрано из интернета - так мы делаем окно прозрачным в белых его частях
			//SetLayeredWindowAttributes(hwnd,RGB(255,255,255),NULL,LWA_COLORKEY);
			// Нет, делаем вот так, а то мышь проваливается
			SetLayeredWindowAttributes(hwnd,NULL,255*70/100,LWA_ALPHA);

			// В третьем режиме инициализируем таймер (это делается в HookHandler(3) при необходимости)
			//if(3==MHSettings::mode) SetTimer(hwnd,1,MHSettings::timeout_after_move,NULL);
			break;

		case WM_DESTROY:	// Завершение программы
			// Чистим за собой
			if((3==MHSettings::mode)||(4==MHSettings::mode)||(1==MHSettings::mode)) KillTimer(hwnd,1);
			KillTimer(hwnd,2);
			KillTimer(hwnd,3);
			KillTimer(hwnd,4);

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
					// Теперь смена позиция происходит только по выезду мыши из области!
					//top_position=-1;
					if(MHSettings::hh) MHSettings::hh->TopLeftCornerTimer();
					break;

				case 1:
					// Теперь смена позиция происходит только по выезду мыши из области!
					//top_position=-1; 
					//if(MHSettings::SettingsDialogue(hwnd))
					if(MHSettings::SettingsDialogue(NULL))
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

			case 3:
				// Отпустить клавишу, которую нажала левая кнопка мыши
				KillTimer(hwnd,3);
				MHKeypad::Press4(5, false);
				flag_left_button_waits=false;
				break;

			case 4:
				// Отпустить клавишу, которую нажала правая кнопка мыши
				KillTimer(hwnd,4);
				MHKeypad::Press4(10,false);
				flag_right_button_waits=false;
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