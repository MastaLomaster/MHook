#include <Windows.h>
#include "hh3.h"
#include "MVector.h"
#include "MHKeypad.h"
#include "Settings.h"

extern LONG screen_x, screen_y;
extern HWND	MHhwnd;

static int offset=0;

void MHookHandler3::OnTimer()
{
	if(-1==position_mem) {KillTimer(MHhwnd,1);return;} // ������� �� ������, ������ ������

	// ���������, � �� �������� �� ���, ���������� � ������� ���������?
	// ��� ����� ��������, �������������� �� ������� ������ �����
	DWORD time_now=timeGetTime();
	if(time_now-last_time<MHSettings::timeout_after_move) return;

	// �������������, ����� ��������� �������!
	KillTimer(MHhwnd,1);
	MHKeypad::Press(position_mem,false,offset);
	MHVector::Reset(); // ��� ��� �����������, ����� � ��� �� ����������� ���� �� ����� �������
	// ������-�� Reset �� �������� �����������
	InvalidateRect(MHhwnd,NULL,TRUE);

	position_mem=-1;
	
}

void MHookHandler3::OnMouseMove(LONG _x, LONG _y)
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

		if(0<=position) // -2=���� ����������� �� ������������� ���������, -1= ����������� �� ����������
		{
			MHKeypad::Press(position,true,offset);
			position_mem=position;
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

	last_x=_x;
	last_y=_y;
	if(last_x<0) last_x=0;
	if(last_y<0) last_y=0;
	if(last_x>=screen_x) last_x=screen_x-1;
	if(last_y>=screen_y) last_y=screen_y-1;
}

void MHookHandler3::OnRDown()
{
	KillTimer(MHhwnd,1);
	rbutton_pressed=true;
	MHKeypad::Reset(offset); // ��������� ������� ������
	MHVector::Reset();
	position_mem=-1;
	offset=6;

}

void MHookHandler3::OnRUp()
{
	KillTimer(MHhwnd,1);
	rbutton_pressed=false;
	MHKeypad::Reset(offset); // ��������� ������� ������
	// �������� ����� ������ ��������
	MHVector::Reset();
	position_mem=-1;
	offset=0;
}

void MHookHandler3::Halt()
{
	// ������ �������
	MHKeypad::Reset(offset);
	offset=0;
}