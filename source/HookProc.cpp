#include <Windows.h>
#include "MHKeypad.h"
#include "Settings.h"

extern HWND		MHhwnd; // ����� ��� ��������� �������
extern LONG screen_x, screen_y; // ��� ����������� ����� ������

bool flag_left_button_key=false;
//static DWORD last_left_down_time;
//static DWORD last_screen_top_time;
int top_position=-1; // 0 - ����� ������� ����, 1- ������ ������� ����, -1 - ������
//static bool mid_button_down=false;
static bool right_button_down=false;
//====================================================================================
// ����������, ��� 
//====================================================================================
LRESULT  CALLBACK HookProc(int disabled,WPARAM wParam,LPARAM lParam) 
{
	//static bool initialized=false;
	//static LONG last_x, last_y;
	
	
	
    if (!disabled)
	{
		MOUSEHOOKSTRUCT * pMouseStruct = (MOUSEHOOKSTRUCT *)lParam;
		if (pMouseStruct != NULL)
		{
			switch(wParam)
			{
			case WM_MOUSEMOVE:
				// � ��������� �� �� � ���� ������?
				if(pMouseStruct->pt.x+pMouseStruct->pt.y<5) 
				{
					top_position=0;
					// ��� ���� �� ���� last_screen_top_time=timeGetTime();
					SetTimer(MHhwnd,2,MHSettings::timeout_mouse_switch,NULL);
				} 
				else if(pMouseStruct->pt.x-pMouseStruct->pt.y>screen_x-5) // ������ ������� ����
				{
					//top_position=0; // ���� ��������� ����� ������� ����
					top_position=1; 
					SetTimer(MHhwnd,2,MHSettings::timeout_mouse_switch,NULL);
				}
				else if(top_position!=-1) // ����� ��������� ������� �������, �� ������ �� ����
				{
					KillTimer(MHhwnd,2);
					top_position=-1;
				}

				// ������� ���������
				if(MHSettings::hh)
				{
					MHSettings::hh->OnMouseMove(pMouseStruct->pt.x, pMouseStruct->pt.y);
				}

				// ���� ��������� ������� ����, ����� ������ ������ ������
				if((right_button_down)&&(MHSettings::flag_no_move_right_mb)) return 1;
				break;

			case WM_MBUTTONDOWN:
			case WM_RBUTTONDOWN:
				right_button_down=true;
				if(MHSettings::hh)
				{
					if(MHSettings::flag_right_mb_iskey) MHKeypad::Press4(10,true);
					else MHSettings::hh->OnRDown();
					// ������ ������ ���� ���������
					return 1;
				}
				break;

			case WM_MBUTTONUP:
			case WM_RBUTTONUP:
				right_button_down=false;
				if(MHSettings::hh)
				{
					if(MHSettings::flag_right_mb_iskey) MHKeypad::Press4(10,false);
					else MHSettings::hh->OnRUp();
					// ������ ������ ���� ���������
					return 1;
				}
				break;

			case WM_LBUTTONDOWN:
				//last_left_down_time=timeGetTime();

				if((flag_left_button_key)&&(MHSettings::hh))
				{
					MHSettings::hh->OnLDown(); 
					return 1;
				}
				break; 

			case WM_LBUTTONUP:
				// �������� ��������� ����� ������ ���� 
			/*	if(MHSettings::timeout_mouse_switch<timeGetTime()-last_left_down_time) // ����� ������
				{
					Beep(450,100);
					
					if(flag_left_button_key)
					{
						MHSettings::hh->OnLUp();
						flag_left_button_key=false;
						return 1;
					}
					else 
					{
						flag_left_button_key=true;
						break; // ��������� ���������� ���������� ���� �������, �� ��� �� ����������� � �������, �� ������� � ����������
					}
				}
			*/
				if(flag_left_button_key) // ������� �������
				{
					MHSettings::hh->OnLUp();
					return 1;
				}
				break;

			}


		}

    }
	return CallNextHookEx(NULL,disabled,wParam,lParam);
}