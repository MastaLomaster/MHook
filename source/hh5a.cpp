#include <Windows.h>
#include "hh5a.h"
#include "MVector.h"
#include "MHKeypad.h"
#include "Settings.h"

#ifdef _DEBUG
#include <stdio.h>
static char debug_buf[4096];
#endif

extern LONG screen_x, screen_y;
extern HWND	MHhwnd;

static int offset=0;
static bool movement_happened=false;

void MHookHandler5a::OnTimer()
{
	if(-1==position_mem) 
	{
		KillTimer(MHhwnd,1);
		return; // ������� �� ������, ������ ������
	}

	if(-2==position_mem) 
	{
		KillTimer(MHhwnd,1);
		position_mem=-1;
		MHVector::Reset(); // �� ��������� ������� �������� � ��� �� ����������� �����������, ��� �������.
		return; // ������� �� ������, ������ ������
	}

	// ���������, � �� �������� �� ���, ���������� � ������� ���������?
	// ��� ����� ��������, �������������� �� ������� ������ �����
	DWORD time_now=timeGetTime();
	if(time_now-last_time<MHSettings::timeout_after_move) return;

	// �������������, ����� ��������� �������!
	MHKeypad::Press(position_mem,false,offset);
	MHVector::Reset(); // ��� ��� �����������, ����� � ��� �� ����������� ���� �� ����� �������
	// ������-�� Reset �� �������� �����������
	InvalidateRect(MHhwnd,NULL,TRUE);

	position_mem=-1;
	KillTimer(MHhwnd,1);
}

void MHookHandler5a::OnMouseMove(LONG _x, LONG _y)
{
	int position;
		
	// ��� ������� ������ ������ ���� �� ������� � �������� � MHVector,
	// ��! ���������� ����������� last_x � last_y, �� ��������� initialized! 
	//if((initialized)&&(!rbutton_pressed)) // �������� ��������� ��������� ����, ������ ������ �� ������
	if(initialized) // �������� ��������� ��������� ����, ������ ������ ������ ���� ���������� �������
	{
		dx=(_x-last_x);
		dy=(_y-last_y);

		// �����, ����� ������ ����� ������?
		if(MHSettings::flag_enable_speed_button) OnFastMove(dx,dy);
		position=MHVector::NewValues(dx,dy);


		// ���� �� ������ ������ ������ ����, ������ ����������
		if((0<=position)&&(rbutton_pressed)) // -2=���� ����������� �� ������������� ���������, -1= ����������� �� ����������
		{
			movement_happened=true; // ��� ���� �� ������ ������� ������ ������, � ������� � ���������

#ifdef _DEBUG
	sprintf(debug_buf,"Position: %d dxdy: %d %d  xy: %d %d last_xy: %d %d\n", position, dx, dy, _x, _y, last_x, last_y);
	OutputDebugString(debug_buf);
#endif

			// �������� ���� �� 8 ������
			if((6==offset)||(11==offset))
			{
				// ��������, ������ ��� �� �������� ������ �������
				if(-1!=position_mem) MHKeypad::Press(position_mem,false,offset);

				MHKeypad::Press(position,true,offset);
				position_mem=position;
				//MHVector::Reset();
			}
			else if((1==position)||(3==position))
			{
				// ��������, ������ ��� �� �������� ������ �������
				if(-1!=position_mem) MHKeypad::Press(position_mem,false,offset);
				position_mem=-1;

				if(1==position)
				{
					offset=6; 
					position_mem=-2; // ����.�������� ��� �������, ������� ������� //MHVector::Reset();
				} // �� ������ � �������� � ��� �� �����������
				if(3==position) 
				{
					offset=11;
					position_mem=-2; // ����.�������� ��� �������, ������� ������� //MHVector::Reset();
				} // �� ������ � �������� � ��� �� �����������
				
			} 
		}
		else
		{
#ifdef _DEBUG
	sprintf(debug_buf,"-----------: %d dxdy: %d %d  xy: %d %d last_xy: %d %d\n", position, dx, dy, _x, _y, last_x, last_y);
	OutputDebugString(debug_buf);
#endif
		}


		// ������ ������� ������ ��� ����� �������� ����, ���� ���� ���� ���-�� ������ �����
		// �� ���� -1!=position_mem
		if(-1!=position_mem)
		{
			last_time=timeGetTime();
			SetTimer(MHhwnd,1,MHSettings::timeout_after_move,NULL);
		}
	}

	if(!initialized) initialized=true;

	// �, ����� ������� ������� ������!
	// ���� �� �� ��� ���� ���������, �� �� ���� ��������� last_x � last_y !!!
	if(!((rbutton_pressed)&&(MHSettings::flag_no_move_right_mb)))
	{
		last_x=_x;
		last_y=_y;
	}
	if(last_x<0) last_x=0;
	if(last_y<0) last_y=0;
	if(last_x>=screen_x) last_x=screen_x-1;
	if(last_y>=screen_y) last_y=screen_y-1;
}

void MHookHandler5a::OnRDown()
{
	KillTimer(MHhwnd,1);
	rbutton_pressed=true;
	MHKeypad::Reset(offset); // ��������� ������� ������
	MHVector::Reset();
	position_mem=-1;
	offset=0;
	movement_happened=false;

}

void MHookHandler5a::OnRUp()
{
	KillTimer(MHhwnd,1);
	rbutton_pressed=false;
	MHKeypad::Reset(offset); // ��������� ������� ������
	// �������� ����� ������ ��������
	MHVector::Reset();
	position_mem=-1;
	offset=0;

	if(!movement_happened)
	{
		// ������ ������, �� ���� �� �������. ���� �� ��������� ���-��
	}

}

void MHookHandler5a::Halt()
{
	KillTimer(MHhwnd,1);
	rbutton_pressed=false;

	// ������ �������
	MHKeypad::Reset(offset);
	position_mem=-1;
	offset=0;
}