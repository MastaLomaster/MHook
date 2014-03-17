#include <Windows.h>
#include "hh2.h"
#include "MVector.h"
#include "MHKeypad.h"
#include "Settings.h"

extern LONG screen_x, screen_y;
extern HWND		MHhwnd;


void MHookHandler2::OnMouseMove(LONG _x, LONG _y)
{
	int position;
		
	// ��� ������� ������ ������ ���� �� ������� � �������� � MHVector,
	// ��! ���������� ����������� last_x � last_y, �� ��������� initialized! 
	if(initialized) // �������� ��������� ��������� ����, ������ ������ �� ������
	{
		dx=(_x-last_x);
		dy=(_y-last_y);

		// �����, ����� ������ ����� ������?
		if(MHSettings::flag_enable_speed_button) OnFastMove(dx,dy);
		position=MHVector::NewValues(dx,dy);

		if(0<=position) // -2=���� ���� �� ���������, -1= ����������� �� ����������
		{
			// ��������� ����� �����������, ������� ���������� ��� ��� ������� �������
			position_mem=position;

			// � ���� ������ - �� �������
			if(rbutton_pressed) // ���� �������� ����������� ��� ������� ������ ������ ���� - ������ �������
			{
				MHKeypad::Press(position,true);
			}
			else
			{
				// ������ ������������ ���������
				InvalidateRect(MHhwnd,NULL,TRUE);
			}
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

bool MHookHandler2::OnRDown()
{
	rbutton_pressed=true;
	MHKeypad::Press(position_mem,true);
	return true; // ���������� ������ ����
}

bool MHookHandler2::OnRUp()
{
	rbutton_pressed=false;
	MHKeypad::Reset(); // ��������� ������� ������
	return true; // ���������� ������ ����
}