#include <Windows.h>
#include "hh6.h"
#include "MVector.h"
#include "MHKeypad.h"
#include "Settings.h"

extern LONG screen_x, screen_y;
extern HWND	MHhwnd;
extern bool flag_stop_mouse; // ������������� ����, ������ ����� ���� ������ ���� !!!

void MHookHandler6::OnTimer()
{
	if(-1==position_mem) {KillTimer(MHhwnd,1);return;} // ������� �� ������, ������ ������

	// ���������, � �� �������� �� ���, ���������� � ������� ���������?
	// ��� ����� ��������, �������������� �� ������� ������ �����
	DWORD time_now=timeGetTime();
	if(time_now-last_time<MHSettings::timeout_after_move) return;

	// �������������, ����� ��������� �������!
	KillTimer(MHhwnd,1);
	MHKeypad::Press(position_mem,false);
	MHVector::Reset(); // ��� ��� �����������, ����� � ��� �� ����������� ���� �� ����� �������
	// ������-�� Reset �� �������� �����������
	InvalidateRect(MHhwnd,NULL,TRUE);

	position_mem=-1;
	
}

//========================================================================
// ���������� ������� - ���� ��� ���� ���������
//========================================================================
void MHookHandler6::OnMouseScroll(LONG _x, LONG _y)
{
	int position;

	if(initialized) // �������� ��������� ��������� ����
	{
		dx=(_x-last_x);
		dy=(_y-last_y);

		// �����, ����� ������ ����� ������?
		if(MHSettings::flag_enable_speed_button) OnFastMove(dx,dy);
		position=MHVector::NewValues(dx,dy);

		if((1==position)||(3==position)) // -2=���� ����������� �� ������������� ���������, -1= ����������� �� ����������
		{
			MHKeypad::Press(position,true);
			position_mem=position;
		}

		// ������ ������� ������ ��� ����� �������� ���� (����� �������), ���� ���� ���� ���-�� ������ �����
		// �� ���� -1!=position_mem
		if((-1!=position_mem)&&((1==position)||(3==position)||(-1==position)||(-2==position)))
		{
			last_time=timeGetTime();
			SetTimer(MHhwnd,1,MHSettings::timeout_after_move,NULL);
		}

		// � ��� ������ ��� � ����� MHVector, ���� �� dy!=0
		if(0!=dy)
		{
#ifdef _DEBUG
			OutputDebugString("Scroll\n");
#endif

			INPUT input={0};

			input.type=INPUT_MOUSE;
			input.mi.dx=0L;
			input.mi.dy=0L;
			input.mi.mouseData=-dy;
			input.mi.dwFlags=MOUSEEVENTF_WHEEL;
			input.mi.time=0;
			input.mi.dwExtraInfo=0;

			SendInput(1,&input,sizeof(INPUT));
		} // Scroll
	} // initialized
	
	if(!initialized) initialized=true;

	// ���� �� �� ��� ���� ���������, �� �� ���� ��������� last_x � last_y !!!
	// � �� ��� �� ���� ��������� � HookProc ����� nomove6:	
	if(!flag_stop_mouse)
	{
		last_x=_x;
		last_y=_y;
	}

	if(last_x<0) last_x=0;
	if(last_y<0) last_y=0;
	if(last_x>=screen_x) last_x=screen_x-1;
	if(last_y>=screen_y) last_y=screen_y-1;
}

//======================================================================================================
// ����� ������������ ������ ��� ����, ����� ���������� ����� initialized (�� ������ dx, dy)
//======================================================================================================
void MHookHandler6::OnMouseMove(LONG _x, LONG _y)
{
	
	if(!initialized) initialized=true;

	last_x=_x;
	last_y=_y;
	if(last_x<0) last_x=0;
	if(last_y<0) last_y=0;
	if(last_x>=screen_x) last_x=screen_x-1;
	if(last_y>=screen_y) last_y=screen_y-1;
}

bool MHookHandler6::OnRDown()
{
	return false; // �� ���������� ������ ����
}

bool MHookHandler6::OnRUp()
{
	return false; // �� ���������� ������ ����
}

void MHookHandler6::Halt()
{
	// ������ �������
	MHKeypad::Reset();
}