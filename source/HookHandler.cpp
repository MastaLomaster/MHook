#include <Windows.h>
#include "HookHandler.h"
#include "Settings.h"

extern HWND		MHhwnd;
extern bool flag_left_button_waits;
extern bool flag_right_button_waits;
extern LONG screen_x, screen_y;

//========================================================================
// ������� ����� ������� ��� ������� �������� ����
//========================================================================
void MHookHandler::OnFastMove(LONG _dx, LONG _dy)
{
	DWORD time_now=timeGetTime();
	
	
	// ������, ����� �� ��������� �������� - �� 1/20 �� 1/10 �������
	if(time_now-last_button5_time<50) // ������������ ������� ������
	{
		mouse_path_squared+=dx*dx+dy*dy;
		return;
	}

	if(time_now-last_button5_time>100) // ������� ����� ����� ����
	{
		// �������� ����� ������
		mouse_path_squared=dx*dx+dy*dy;
		last_button5_time=time_now;
		return;
	}
	
	// �������, ������ � ������ �������� �������. ���� �������� ���� ��� �������, �������� �������.
	mouse_path_squared+=dx*dx+dy*dy;
	if(mouse_path_squared>MHSettings::minimal_mouse_speed) // ����� ������� ��������
	{
		MHKeypad::Press4(4, true);
		MHKeypad::Press4(4, false);
#ifdef _DEBUG
		// OutputDebugString("������=>");
#endif
	}
		
	// �������� ����� ������
	mouse_path_squared=dx*dx+dy*dy;;
	last_button5_time=time_now;
}

//==========================================================================
// ���������� ������� ��� �������, ��� ������ �� �����, ������ ��������� ���
//==========================================================================
void MHookHandler::OnMouseScroll(LONG _x, LONG _y)
{
	OnMouseMove(_x,_y); // �� ��������� ��� �������� �� �������� �������� �� ��, ��� ������ �� ������ ������, �� � �����
}

	

//========================================================================
// ������� ������ ������� ��� ������, ����� ����� ������ ������������
//========================================================================
void MHookHandler::OnLDown()
{
	// ������ ���� ��� �� ��������� ������!
	if(!flag_left_button_waits)
	{
		MHKeypad::Press4(5, true);
		//if(MHSettings::flag_left_mb_push_twice) MHKeypad::Press4(5,false); // ��� ���� �� ��� ���������� ���� ��� ������� ��� ���
		if(MHSettings::flag_left_mb_push_twice) 
		{
			SetTimer(MHhwnd,3,MHSettings::timeout_mouse_click,NULL); // ��� ���� �� ��� ���������� ���� ��� ������� ��� ���
			flag_left_button_waits=true;
		}
	}
}

void MHookHandler::OnLUp()
{
	if(MHSettings::flag_left_mb_push_twice) 
	{
		if(!flag_left_button_waits)
		{
			MHKeypad::Press4(5,true); // ��� ���� �� ��� ���������� ���� ��� ������� ��� ���
			SetTimer(MHhwnd,3,MHSettings::timeout_mouse_click,NULL); 
		}
	}
	else MHKeypad::Press4(5, false);
}

//========================================================================
// ��������� / ���������� ����� ������ ����
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