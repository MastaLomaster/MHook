#include <Windows.h>
#include "MHKeypad.h"
#include "Settings.h"

extern HWND		MHhwnd; // ����� ��� ��������� �������
extern LONG screen_x, screen_y; // ��� ����������� ����� ������
extern bool flag_inside_window; // �������� � ������� ���������, ����������, ��� �� ������ ����
bool flag_scroll_started=false;


bool flag_left_button_key=false;
bool flag_stop_emulation=false;

bool flag_left_button_waits=false;
bool flag_right_button_waits=false;

static DWORD last_right_down_time;

//static DWORD last_screen_top_time;
int top_position=-1; // 0 - ����� ������� ����, 1- ������ ������� ����, -1 - ������
//static bool mid_button_down=false;

static bool right_button_down=false;
static bool left_button_down=false;

//====================================================================================
// ����������, ��� 
//====================================================================================
LRESULT  CALLBACK HookProc(int disabled,WPARAM wParam,LPARAM lParam) 
{
	
    if (!disabled)
	{
		MOUSEHOOKSTRUCT * pMouseStruct = (MOUSEHOOKSTRUCT *)lParam;
		if (pMouseStruct != NULL)
		{
			switch(wParam)
			{
			case WM_MOUSEMOVE:
				//if(pMouseStruct->pt.x+pMouseStruct->pt.y<5) // � ��������� �� �� � ������� ����� ���� ������?
				if((pMouseStruct->pt.y-pMouseStruct->pt.x>(screen_y-1)-5)) // � ��������� �� �� � ������ ����� ���� ������?
				{
					// ��������� ������, ������ ���� ������ � ���� �������!!!
					if(0!=top_position) SetTimer(MHhwnd,2,MHSettings::timeout_mouse_switch,NULL); // 
					top_position=0;
					// ��� ���� �� ���� last_screen_top_time=timeGetTime();
				} 
				//else if(pMouseStruct->pt.x-pMouseStruct->pt.y>screen_x-5) // ������ ������� ����
				else if(pMouseStruct->pt.x+pMouseStruct->pt.y>(screen_x-1)+(screen_y-1)-5) // ������ ������ ����
				{
					// ��������� ������, ������ ���� ������ � ���� �������!!!
					if(1!=top_position) SetTimer(MHhwnd,2,MHSettings::timeout_mouse_switch,NULL);
					//top_position=0; // ���� ��������� ����� ������� ����
					top_position=1; 
				}
				else if((top_position!=-1)&&(pMouseStruct->pt.y<screen_y)&&(pMouseStruct->pt.y>=0)&&
					(pMouseStruct->pt.x<screen_x)&&(pMouseStruct->pt.x>=0)) // ����� ��������� ������� �������, �� ������ �� ����
					// �� ������������� ����� �� ������� ������
				{
					KillTimer(MHhwnd,2);
					top_position=-1;
				}

				// ������� ��������� - �� ������ � ���������: ������� ������ ������ ��� ���������� ��������!
				if(!flag_stop_emulation)
				{
					if(MHSettings::hh)
					{
						if(!flag_scroll_started) 
						{
							MHSettings::hh->OnMouseMove(pMouseStruct->pt.x, pMouseStruct->pt.y);
						}
						else MHSettings::hh->OnMouseScroll(pMouseStruct->pt.x, pMouseStruct->pt.y);
					}
				}

				// ���� ��������� ������� ����, ����� ������ ������ ������
				if((right_button_down)&&(MHSettings::flag_no_move_right_mb)) return 1;
				break;

			case WM_MBUTTONDOWN:
			case WM_RBUTTONDOWN:
				right_button_down=true;
				if(MHSettings::hh)
				{
					if(left_button_down) // ��� ������ ������ - �������� ����� �������
					{
						flag_scroll_started=true;
						if(MHSettings::hh) MHSettings::hh->Deinitialize();
						return 1;
					}

					if(MHSettings::flag_right_mb_iskey) 
					{
						// ������ ���� ��� �� ��������� ������!
						if(!flag_right_button_waits)
						{
							MHKeypad::Press4(10,true);
							//if(MHSettings::flag_right_mb_push_twice) MHKeypad::Press4(10,false); // ��� ���� �� ��� ���������� ���� ��� ������� ��� ���
							if(MHSettings::flag_right_mb_push_twice)
							{
								SetTimer(MHhwnd,4,MHSettings::timeout_mouse_click,NULL); // ��� ���� �� ��� ���������� ���� ��� ������� ��� ���
								flag_right_button_waits=true;
							}
						}
					}
					else MHSettings::hh->OnRDown();
					
					// ����������: ������� ������ ��������� ����� ������ ���� � ������ ���������
					if(MHSettings::flag_right_mb_doubleclick)
					{
						DWORD this_time=timeGetTime();
						if(this_time-last_right_down_time<500) // 1/2 �������
						{
#ifdef _DEBUG
		 OutputDebugString("������� ������ ������\n");
#endif
							
							// ����������� ����� ��������
							if(flag_stop_emulation) flag_stop_emulation=false;
							else
							{
								flag_stop_emulation=true;
								if(MHSettings::hh) MHSettings::hh->Deinitialize();
							}
						}
						last_right_down_time=this_time;
					}
					


					// ������ ������ ���� ���������
					return 1;
				}
				break;

			case WM_MBUTTONUP:
			case WM_RBUTTONUP:
				right_button_down=false;

				// ��������, �� ����� ���������� ���� ����� �������� ������
				if(flag_stop_emulation) flag_stop_emulation=false;

				if(flag_scroll_started)
				{
					flag_scroll_started=false;
					if(MHSettings::hh) MHSettings::hh->Deinitialize();
					return 1; // ���������, ��� ��� �������� �������
				}

				if(MHSettings::hh)
				{
					if(MHSettings::flag_right_mb_iskey) 
					{
						if(MHSettings::flag_right_mb_push_twice) 
						{
							// ������ ���� ��� �� ��������� ������!
							if(!flag_right_button_waits)
							{
								MHKeypad::Press4(10,true); // ��� ���� �� ��� ���������� ���� ��� ������� ��� ���
								SetTimer(MHhwnd,4,MHSettings::timeout_mouse_click,NULL); 
								flag_right_button_waits=true;
							}
						}
						else MHKeypad::Press4(10,false);
					}
					else MHSettings::hh->OnRUp();


					// ������ ������ ���� ���������
					return 1;
				}
				break;

			case WM_LBUTTONDOWN:
				left_button_down=true;
				// ��������, �� ���������� �� �� ������ � ��� ���������?
				// ��� �������� ������ �������
				//if((flag_left_button_key)&&(flag_inside_window))
				if((flag_inside_window)||(right_button_down))
				{
					flag_scroll_started=true;
					if(MHSettings::hh) MHSettings::hh->Deinitialize();
					return 1;
				}
				// ���, ��� �� ������ �������, ������������ �� ��������
				else if((flag_left_button_key)&&(MHSettings::hh))
				{
					MHSettings::hh->OnLDown(); 
					return 1;
				}
				break; 

			case WM_LBUTTONUP:
				left_button_down=false;
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
				if(flag_scroll_started)
				{
					flag_scroll_started=false;
					if(MHSettings::hh) MHSettings::hh->Deinitialize();
					return 1; // ���������, ��� ��� �������� �������
				}
				else if(flag_left_button_key) // ������� �������
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