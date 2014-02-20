#include <Windows.h>
#include "Bitmap.h"
#include "Settings.h"

extern HHOOK handle;
extern LONG screen_x, screen_y;
extern int top_position; // ��� � HookProc ����������, � ����� ���� ������ �� �����������.

LONG quad_x=0,quad_y=0; // ���������� ���������� � ����

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
		case WM_CREATE:
			// �������������� �������
			MHBitmap::Init(hwnd);
			// � ������� ������ �������������� ������ (��� �������� � HookHandler(3) ��� �������������)
			//if(3==MHSettings::mode) SetTimer(hwnd,1,MHSettings::timeout_after_move,NULL);
			break;

		case WM_DESTROY:	// ���������� ���������
			// ������ �� �����
			if((3==MHSettings::mode)||(4==MHSettings::mode)||(1==MHSettings::mode)) KillTimer(hwnd,1);
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
					top_position=-1;
					if(MHSettings::hh) MHSettings::hh->TopLeftCornerTimer();
					break;
				case 1:
					top_position=-1;
					if(MHSettings::SettingsDialogue(hwnd))
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