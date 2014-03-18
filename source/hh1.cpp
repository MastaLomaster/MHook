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

static int alt2_offset=0; // �������� ��� �������������� ���������
static int position_mem_opposite=0; // ������ �������� ������� �� ������� � �� ���������������� �����������. ������ ��� ������ ����������. 


//=====================================================================================================
// �������, ������� ����������, �������� �� ����� ����������� ��������������� � �������
// ��� 8 ����������� �������, ��� ���������������� �������� ��� ����������� >90 �������� �� �������
//=====================================================================================================
static bool IsOpposite(int old_direction, int new_direction)
{
	int num_directions=MHSettings::GetNumPositions();
	int opposite8;

	if((-1==old_direction)||(-1==new_direction)) return false;

	if(4==num_directions)
	{
		if((old_direction+2)%4==new_direction) return true;
	}
	else // 8 �������
	{
		opposite8=(old_direction+4)%8;

		if(new_direction==opposite8) return true;

		if(0==opposite8) // ���� �����, ��� ������ ��������� � 0
		{
			if((7==new_direction)||(1==new_direction)) return true;
		}
		else
		{
			if((new_direction==opposite8-1)||(new_direction==opposite8+1)) return true;
		}
	}

	return false;
}


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

		// ���� ���� � ���� = ������ ����, ������ ������� 3 � 5 �� 4
		if((8==MHSettings::GetNumPositions())&&((3==position)||(5==position)))
			position=4;

		if(!rbutton_pressed) // �������� ��������� ��������� ����, ������ ������ �� ������
		{
			// ����� ����� - �� ����� � ����� � ��� �������� (������ ��� 0==alt_offset)
			if((MHSettings::flag_2moves_mode1)&&(0==alt2_offset))
			{
				if(0<=position) // ����� �����������
				{
					// ��� ��������������� �����������?
					if(IsOpposite(position_mem_opposite,position))
					{
						// 1. (��� �������������� ����� ���������������� �����������)
						// ������� ������� �������. ������ ��������������� ������ ������ ����� ��������
						if(false==flag_opposite_direction)
						{
							// �������� �����, �������� ������, �������� ����������� (����)
							opposite_time=timeGetTime();
							MHKeypad::Reset(alt2_offset);
#ifdef _DEBUG
		OutputDebugString("OppDir keyup");
#endif
							flag_opposite_direction=true;
							position_mem_opposite=position;
							 
							// ������-�� Reset �� �������� �����������
							InvalidateRect(MHhwnd,NULL,TRUE);
						}
						else // ��� ������������ ����� ���������������� �����������
						{
							// 2. ��� ����� ���������?
							MHKeypad::Press(position,true,alt2_offset);
							flag_opposite_direction=false;
							position_mem_opposite=position;
						}
					}
					else // �� ��������������� (position_mem �������� �� ��� ������),
					{
						if(true==flag_opposite_direction) 
						{
							// 3.
							// ��� ������ � ������� position_mem, � �� ������ ��������� � �������
							// ������ ����� �������� ������� � ������� !!!
							// �� ������ ���� ������ ����� ���������������!!!
							if(timeGetTime()-opposite_time>100) // ���� �����, ����� ���� � ��������������� �����������
							{
								MHKeypad::Press(position,true, alt2_offset);
								flag_opposite_direction=false;
								position_mem_opposite=position;
							}
							else 
							{
								MHVector::Reset(); // �� ���� ������ ����� -1
								opposite_time=timeGetTime(); // ����� � 50 �� ������������� �� ����, �����������
							}
							//MHKeypad::Press(position,true);
							//flag_opposite_direction=false; // ���� � ������� - ����� ��������
						}
						else // flag_opposite_direction=false
						{
							// �� ��� ������ � ������� position_mem. 
							// ��������, ������ ���� -1==position_mem (��� ����������)
							if((-1==position_mem)||(MHSettings::flag_change_direction_ontheway))
							{
								MHKeypad::Press(position,true, alt2_offset);
								position_mem_opposite=position;
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
							MHKeypad::Press(position_mem_opposite,true, alt2_offset);
							flag_opposite_direction=false;
						}
						else opposite_time=timeGetTime(); // ����� � 50 �� ������������� �� ����, �����������
					}
				}
				// else �������� ������ -2, ����������
			} //flag_2moves_mode1 � 4 �������
			else 
			{
				if(0==alt2_offset)
				{
					// ����� ��-�������, ��� ���� �� ����������� flag_2moves_mode1
					if(0<=position) // -2=���� ���� �� ���������, -1= ����������� �� ����������
					{
						MHKeypad::Press(position,true,alt2_offset);
					}
				}
				else // ���� ��� �������������� ���������, �� ������� ������
				{
					// ��� ������ ��� ��������� ������ ������ ����
					if(0<=position) // -2=���� ����������� �� ������������� ���������, -1= ����������� �� ����������
					{
						MHKeypad::Press(position,true, alt2_offset); // �� �������� ������ ������ �������� �������������� ������� �� ������ ���������
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
			}
		} // ������ ������ �� ������
		else // ������ ������ ������. ��������!!!! ����� ����� ���� 8 �������, ����� �������� � ������ ������� ���������� !!!!
		{
			if(4==MHSettings::GetNumPositions())
			{
				if(!MHSettings::flag_alt2) // ��� ����, ���� �� ����� ������ ��������������: �������� � ������� ������ �������� ������� c ��������
				{
					// ��������� ������ ������ ����
					if(0<=position) // -2=���� ����������� �� ������������� ���������, -1= ����������� �� ����������
					{
						//MHKeypad::Press(position,true, 6); // �� �������� ������ ������ �������� �������������� ������� �� ������ ���������
						MHKeypad::Press(position,true, alt2_offset);
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
				else // flag_alt2 - ������ �������� � ������� ������ - ��� ����� ���������
				{
					switch(position)
					{
						
					case 1: // ������� ������� - ������ ��������������
						// ������ ����� - ��������� ������� �������
						MHKeypad::Reset(alt2_offset);
						position_mem=-1;
						switch(alt2_offset)
						{
						case 6: // �������� ���, ���������
							alt2_offset=0;
							break;

						case 0: // ��������, ��������
						case 11: // �������� ������, ��������
							alt2_offset=6;
							break;
						}
						break;

					case 3: // ������� ����� - ����� ������ ��������������
						// ������ ����� - ��������� ������� �������
						MHKeypad::Reset(alt2_offset);
						position_mem=-1;
						switch(alt2_offset)
						{
						case 11: // �������� ���, ���������
							alt2_offset=0;
							break;

						case 0: // ��������, ��������
						case 6: // �������� ������, ��������
							alt2_offset=11;
							break;
						}
						break;
						// ��������� ����������� (������� ����� � ����) ����������
					}
				} // ��������� ���� alt2, ������ ���������
			} // 4 �������, � � 8 �������� � ������ ������� ������ �� ������ ������
			
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

bool MHookHandler1::OnRDown()
{
	rbutton_pressed=true;
#ifdef _DEBUG
		OutputDebugString("OnRDown keyup");
#endif
	MHKeypad::Reset(alt2_offset); // ��������� ������� ������
	position_mem=-1;
	position_mem_opposite=-1;
	// �������� ����� ������ ��������
	MHVector::Reset();

	// ������-�� Reset �� �������� �����������
	InvalidateRect(MHhwnd,NULL,TRUE);

	if(!MHSettings::flag_alt2) // ��� ����, ���� �� ����� ������ ��������������
	{
		alt2_offset=6;
	}

	return true; // ���������� ������ ����
}

bool MHookHandler1::OnRUp()
{
	rbutton_pressed=false;
#ifdef _DEBUG
		OutputDebugString("OnRUP keyup");
#endif
	MHKeypad::Reset(alt2_offset); // ��������� ������� ������ (� �������������� ����� ������, �������� 6)
	position_mem=-1;
	position_mem_opposite=-1;
	// �������� ����� ������ ��������
	MHVector::Reset();
	
	if(!MHSettings::flag_alt2) // ��� ����, ���� �� ����� ������ ��������������
	{
		alt2_offset=0;
	}
	flag_opposite_direction=false;

	// ������-�� Reset �� �������� �����������
	InvalidateRect(MHhwnd,NULL,TRUE);
	
	return true; // ���������� ������ ����
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


			// � ������ ���� �������������� ��������� ������ �������
			if(MHSettings::flag_alt2)
			{
				switch(alt2_offset)
				{
				case 0:
					TextOut(hdc,90,90,"0",1);
					break;

				case 11:
					TextOut(hdc,90,90,"<",1);
					break;

				case 6:
					TextOut(hdc,90,90,">",1);
					break;
				}
			}
}

void MHookHandler1::OnTimer()
{
	if(-1==position_mem)
	{
		KillTimer(MHhwnd,1);
		return; // ������� �� ������, ������ ������
	}

	// ���������, � �� �������� �� ���, ���������� � ������� ���������?
	// ��� ����� ��������, �������������� �� ������� ������ �����
	DWORD time_now=timeGetTime();
	if(time_now-last_time<MHSettings::timeout_after_move) return;

	// �������������, ����� ��������� �������!
	KillTimer(MHhwnd,1); // ����� � ��������� �� �������� ��������� ��� ��������� Press
#ifdef _DEBUG
		OutputDebugString("Timer keyup");
#endif
	MHKeypad::Press(position_mem,false,alt2_offset); // �� �������� ������ ������ �������� �������������� �������
	MHVector::Reset(); // ��� ��� �����������, ����� � ��� �� ����������� ���� �� ����� �������
	// ������-�� Reset �� �������� �����������
	InvalidateRect(MHhwnd,NULL,TRUE);

	position_mem=-1;
	
}

void MHookHandler1::Halt()
{
	KillTimer(MHhwnd,1);

	// ������ �������
	//if(
	//if((rbutton_pressed)&&(!MHSettings::flag_alt2)) 
	MHKeypad::Reset(alt2_offset);
		
	alt2_offset=0;

	// ����������� ��� ��� (� ������� �� HaltGeneral �������� ������):
	flag_opposite_direction=false;
	position_mem=-1;
	position_mem_opposite=-1;
}