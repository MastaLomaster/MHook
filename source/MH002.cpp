#include <Windows.h>
#include "Bitmap.h"
#include "Settings.h"

bool flag_inside_window=false;

extern HHOOK handle;
extern LONG screen_x, screen_y;
extern int top_position; // ��� � HookProc ����������, � ����� ���� ������ �� �����������.
extern bool flag_left_button_waits;
extern bool flag_right_button_waits;

LONG quad_x=0,quad_y=0; // ���������� ���������� � ����

static TRACKMOUSEEVENT tme={sizeof(TRACKMOUSEEVENT),TME_LEAVE,0,HOVER_DEFAULT};

// ����������� ������� ��������� � ����� 002
//====================================================================================
// ������� ���������
//====================================================================================
LRESULT CALLBACK WndProc(HWND hwnd,
						UINT message,
						WPARAM wparam,
						LPARAM lparam)
{
	switch (message)
	{
		// ���� �������, �� ������� �� ����������, ��������� �� ���� ��� �����
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
			// �������������� �������
			MHBitmap::Init(hwnd);
			// ��������� tme ��������� ����
			tme.hwndTrack=hwnd;
			// ������� �� ��������� - ��� �� ������ ���� ���������� � ����� ��� ������
			//SetLayeredWindowAttributes(hwnd,RGB(255,255,255),NULL,LWA_COLORKEY);
			// ���, ������ ��� ���, � �� ���� �������������
			SetLayeredWindowAttributes(hwnd,NULL,255*70/100,LWA_ALPHA);

			// � ������� ������ �������������� ������ (��� �������� � HookHandler(3) ��� �������������)
			//if(3==MHSettings::mode) SetTimer(hwnd,1,MHSettings::timeout_after_move,NULL);
			break;

		case WM_DESTROY:	// ���������� ���������
			// ������ �� �����
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
			case 1: // ������ ������� ������
				if(MHSettings::hh) MHSettings::hh->OnTimer(); // ��� �� ������, ���� ������ �����, � ������� � ������� ��������
				break;
			
			case 2: // ������ ���� ������
				KillTimer(hwnd,2); // ������ ����� ������ �������
				switch(top_position)
				{
				case 0:
					// ������ ����� ������� ���������� ������ �� ������ ���� �� �������!
					//top_position=-1;
					if(MHSettings::hh) MHSettings::hh->TopLeftCornerTimer();
					break;

				case 1:
					// ������ ����� ������� ���������� ������ �� ������ ���� �� �������!
					//top_position=-1; 
					//if(MHSettings::SettingsDialogue(hwnd))
					if(MHSettings::SettingsDialogue(NULL))
					{
						// ������ �� ����� - ��������, �������
						if((3==MHSettings::mode)||(4==MHSettings::mode)||(1==MHSettings::mode)) KillTimer(hwnd,1);
						MHKeypad::Reset();
						UnhookWindowsHookEx(handle);
						PostQuitMessage(0);
					}
					// ��� ����� ����� ������� ��������
					break;
				}
				//Beep(450,100);
				break;

			case 3:
				// ��������� �������, ������� ������ ����� ������ ����
				KillTimer(hwnd,3);
				MHKeypad::Press4(5, false);
				flag_left_button_waits=false;
				break;

			case 4:
				// ��������� �������, ������� ������ ������ ������ ����
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
			
			// ���������� ������� ������
			MHBitmap::OnDraw(hdc,MHSettings::GetPosition());

			if(MHSettings::hh) MHSettings::hh->OnDraw(hdc,200);

			//if((MHposition>-2)&&(MHposition<4)) MHBitmap::OnDraw(hdc,4,MHposition);

/*			RECT rect;

			// ��������� � �����
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

		default: // ��������� �������������� ��������
			return DefWindowProc(hwnd,message,wparam,lparam);
	}

return 0; // ���� ����������
}