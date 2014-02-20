#include <Windows.h>
#include "hh1a.h"
#include "MVector.h"
#include "MHKeypad.h"
#include "Settings.h"

extern LONG screen_x, screen_y;


void MHookHandler1a::OnMouseMove(LONG _x, LONG _y)
{
	int position;
	DWORD time_now;
		
	// ��� ������� ������ ������ ���� �� ������� � �������� � MHVector,
	// ��! ���������� ����������� last_x � last_y, �� ��������� initialized! 
	if((initialized)&&(!rbutton_pressed)) // �������� ��������� ��������� ����, ������ ������ �� ������
	{
		dx=(_x-last_x);
		dy=(_y-last_y);

		position=MHVector::NewValues(dx,dy);
		// !!! ����� �� ��������� � ��������� �������� � ��� �� �����������
		if(0<=position) position_mem=position;

		// ������� �� 0<=..., � -1<=...
		if(-1<=position) // -2=���� ���� �� ���������, -1= ����������� �� ����������
		{
			time_now=timeGetTime();

			// ���� ����������� ���������� - ����� � ���������� ������� �� ����� ��������
			// ���� �� ���������� -  �����, ����� � ������� ���������� ������� ������ ���������� �������
			if((0<=position)||(time_now-last_time>MHSettings::time_between_pushes)) 
			{
				MHKeypad::Press(position_mem,true);
				MHKeypad::Press(position_mem,false);
				last_time=time_now;
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

void MHookHandler1a::OnRDown()
{
	rbutton_pressed=true;
	MHKeypad::Reset(); // ��������� ������� ������
}

void MHookHandler1a::OnRUp()
{
	rbutton_pressed=false;
	// �������� ����� ������ ��������
	MHVector::Reset();
}