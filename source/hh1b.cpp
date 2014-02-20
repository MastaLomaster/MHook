#include <Windows.h>
#include "hh1.h"
#include "MVector.h"
#include "MHKeypad.h"
#include "Settings.h"

#ifdef _DEBUG
//#include <stdio.h>
	//char debug_buf[4096];
#endif

extern HWND		MHhwnd;

extern LONG screen_x, screen_y;


void MHookHandler1::OnMouseMove(LONG _x, LONG _y)
{
	int position;
		
	// ��� ������� ������ ������ ���� �� ������� � �������� � MHVector,
	// ��! ���������� ����������� last_x � last_y, �� ��������� initialized! 
	// ������ ��-������: ���� ���� ���������� � ���, �� ����� ������ ���������������
	// ��� �� ��. ������������ ������.
	if(initialized)
	{
		dx=(_x-last_x);
		dy=(_y-last_y);

		// �����, ����� ������ ����� ������?
		if(MHSettings::flag_enable_speed_button) OnFastMove(dx,dy);

		position=MHVector::NewValues(dx,dy);

		if(!rbutton_pressed) // �������� ��������� ��������� ����, ������ ������ �� ������
		{
			// ����� ����� - �� ����� � ����� � ��� ��������
			if((MHSettings::flag_2moves_mode1)&&(4==MHSettings::GetNumPositions()))
			{
				if(0<=position) // ����� �����������
				{
					// ��� ��������������� �����������?
					if(((0==position)&&(2==position_mem)||
						(1==position)&&(3==position_mem)||
						(2==position)&&(0==position_mem)||
						(3==position)&&(1==position_mem)))
					{
						// 1. (��� �������������� ����� ���������������� �����������)
						if(false==flag_opposite_direction)
						{
							// �������� �����, �������� ������, �������� ����������� (����)
							opposite_time=timeGetTime();
							MHKeypad::Reset();
							flag_opposite_direction=true;
							position_mem=position;
							// ������-�� Reset �� �������� �����������
							InvalidateRect(MHhwnd,NULL,TRUE);
						}
						else // ��� ������������ ����� ���������������� �����������
						{
							/*// ��� �������� ������ �����, ����� �� �������� � ������� � ����� ��������� �� ������ �����������
							// � ��� ����������� ����, ��� ���������� � position==-1
							if(timeGetTime()-opposite_time>50) // ���� �����, ����� ���� � ��������������� �����������
							{
								MHKeypad::Press(position_mem,true);
								flag_opposite_direction=false;
							}
							else opposite_time=timeGetTime(); // ����� � 50 �� ������������� �� ����, ����������� */
							MHKeypad::Press(position,true);
							flag_opposite_direction=false;
							position_mem=position;
						}
					}
					else // �� ��������������� (position_mem �������� �� ��� ������),
					{
						if(true==flag_opposite_direction) 
						{
							// ��� ������ � ������� position_mem, � �� ������ ��������� � �������
							// ������ ����� �������� ������� � ������� !!!
							// �� ������ ���� ������ ����� ���������������!!!
							if(timeGetTime()-opposite_time>100) // ���� �����, ����� ���� � ��������������� �����������
							{
								MHKeypad::Press(position,true);
								flag_opposite_direction=false;
								position_mem=position;
							}
							else 
							{
								MHVector::Reset(); // �� ���� ������ ����� -1
								opposite_time=timeGetTime(); // ����� � 50 �� ������������� �� ����, �����������
							}
							//MHKeypad::Press(position,true);
							//flag_opposite_direction=false; // ���� � ������� - ����� ��������
						}
						else
						{
							// �� ��� ������ � ������� position_mem. 
							// ��������, ������ ���� -1==position_mem (��� ����������)
							if((-1==position_mem)||(MHSettings::flag_change_direction_ontheway))
							{
								MHKeypad::Press(position,true);
								position_mem=position;
							}
						}
					
					} // �� ��������������� �����������
				}
				else if(-1==position)
				{
					// ������������, ������ ���� �� ������ �� �����
					if(flag_opposite_direction)
					{
						if(timeGetTime()-opposite_time>100) // ���� �����, ����� ���� � ��������������� �����������
						{
							MHKeypad::Press(position_mem,true);
							flag_opposite_direction=false;
						}
						else opposite_time=timeGetTime(); // ����� � 50 �� ������������� �� ����, �����������
					}
				}
				// else �������� ������ -2, ����������
			}
			else
			{
				// ��-�������, ��� ���� �� ����������� flag_2moves_mode1
				if(0<=position) // -2=���� ���� �� ���������, -1= ����������� �� ����������
				{
					MHKeypad::Press(position,true);
				}
			}
		} // ������ ������ �� ������
		else
		{
			// ��������� ������ ������ ����
			if(0<=position) // -2=���� ����������� �� ������������� ���������, -1= ����������� �� ����������
			{
				MHKeypad::Press(position,true, 6); // �� �������� ������ ������ �������� �������������� �������
				 
				position_mem=position;
			}
			// ������ ������� ������ ��� ����� �������� ����, ���� ���� ���� ���-�� ������ �����
			// �� ���� -1!=position_mem
			if(-1!=position_mem)
			{
				last_time=timeGetTime();
				SetTimer(MHhwnd,1,MHSettings::timeout_after_move,NULL);
			}
			
		} // ������ ������ ������
	} // if initialized
		

	if(!initialized) initialized=true;

	last_x=_x;
	last_y=_y;
	if(last_x<0) last_x=0;
	if(last_y<0) last_y=0;
	if(last_x>=screen_x) last_x=screen_x-1;
	if(last_y>=screen_y) last_y=screen_y-1;

#ifdef _DEBUG
	// �������� ���������� �������� ������� ���������
	//sprintf(debug_buf,"dx: %d  dy: %d\n", dx,dy);
	//OutputDebugString(debug_buf);
#endif
}

void MHookHandler1::OnRDown()
{
	rbutton_pressed=true;
	MHKeypad::Reset(); // ��������� ������� ������
	// �������� ����� ������ ��������
	MHVector::Reset();

	// ������-�� Reset �� �������� �����������
	InvalidateRect(MHhwnd,NULL,TRUE);
}

void MHookHandler1::OnRUp()
{
	rbutton_pressed=false;
	MHKeypad::Reset(6); // ��������� ������� ������ (� �������������� ����� ������, �������� 6)
	// �������� ����� ������ ��������
	MHVector::Reset();

	flag_opposite_direction=false;
	position_mem=-1;

}

extern LONG quad_x,quad_y; // ���������� ���������� � ����

void MHookHandler1::OnDraw(HDC hdc, LONG window_size)
{
	RECT rect;

			// ��������� � �����
			//xpercent/100.0f*xsize
			//ypercent/100.0f*ysize
			rect.left=(LONG)(window_size/2+quad_x-10);
			rect.top=(LONG)(window_size/2+quad_y-10);
			rect.right=rect.left+20;
			rect.bottom=rect.top+20;
			FillRect(hdc,&rect,(HBRUSH)GetStockObject(GRAY_BRUSH));
}

void MHookHandler1::OnTimer()
{
	if(-1==position_mem) return; // ������� �� ������, ������ ������

	// ���������, � �� �������� �� ���, ���������� � ������� ���������?
	// ��� ����� ��������, �������������� �� ������� ������ �����
	DWORD time_now=timeGetTime();
	if(time_now-last_time<MHSettings::timeout_after_move) return;

	// �������������, ����� ��������� �������!
	MHKeypad::Press(position_mem,false,6); // �� �������� ������ ������ �������� �������������� �������
	MHVector::Reset(); // ��� ��� �����������, ����� � ��� �� ����������� ���� �� ����� �������
	// ������-�� Reset �� �������� �����������
	InvalidateRect(MHhwnd,NULL,TRUE);

	position_mem=-1;
	KillTimer(MHhwnd,1);
}

void MHookHandler1::Halt()
{
	// ������ �������
	if(rbutton_pressed) MHKeypad::Reset(6);
		
	// ����������� ��� ��� (� ������� �� HaltGeneral �������� ������):
	flag_opposite_direction=false;
}