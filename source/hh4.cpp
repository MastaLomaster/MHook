#include <Windows.h>
#include "hh4.h"
#include "MHKeypad.h"
#include "Settings.h"
#include "MVector.h"

extern HWND		MHhwnd;

#ifdef _DEBUG
//#include <stdio.h>
	//char debug_buf[4096];
#endif

extern LONG screen_x, screen_y;
extern HBRUSH green_brush, yellow_brush;

static LONG X,Y,last_x_direction,last_y_direction;

// ����� ������ �������� � ����������� �� ����������� x_direction+1, y_direction+1;
static int button_pressed[3][3]=
{
	{7,6,5},
	{0,-1,4},
	{1,2,3}
};

void MHookHandler4::OnMouseMove(LONG _x, LONG _y)
{
	int position;
	LONG x_direction=0,y_direction=0;
		
	// ��� ������� ������ ������ ���� �� ������� � �������� � MHVector,
	// ��! ���������� ����������� last_x � last_y, �� ��������� initialized! 
	if((initialized)&&(!rbutton_pressed)) // �������� ��������� ��������� ����, ������ ������ �� ������
	{
		dx=(_x-last_x);
		dy=(_y-last_y);

		// �����, ����� ������ ����� ������?
		if(MHSettings::flag_enable_speed_button) OnFastMove(dx,dy);

		X+=dx; Y+=dy;
		
		// � ����� � ��� ��������? (��� X �� ������ �������� � ������ 3)
		if((true==MHSettings::flag_2moves)&&(0!=MHSettings::mode3axe))
		{
			DWORD thistime=timeGetTime();
			if(0!=x_limitation)
			{
				if((0==last_x_direction)&&(thistime-steps2timeout>50)) // ����� ������� �����������, ������� 100 �� � ��������
				{
					x_limitation=0;
				}
				else	// ����������� ���������� �����������, ������ �������������, ��� ��� ���� ���������
				{
					steps2timeout=thistime;
					// ��� ����������� ������������� �� ������ ������� � 
					if((X<0)&&(x_limitation>0)) X=0;
					if((X>0)&&(x_limitation<0)) X=0;
				}
			}
		} // �������� � ��� ����


		if(0==MHSettings::mode3axe) // �� �������� �� ��� � �� ������ 3?
		{
			position=MHVector::NewValues(dx,0); // �������� �� ��� Y �� �������

			if(0<=position) // -2=���� ����������� �� ������������� ���������, -1= ����������� �� ����������
			{
				// �� ��� x �������� ������ 2 �����������: 2 � 6
				if(2==position) x_direction=1;
				else x_direction=-1;
				
				position_mem=position; // ����� �� ��������� ������ �� �������?
			}
			else // ��������� ����������� �� x
			{
				x_direction=last_x_direction;
			}

			// ������ ������� ������ ��� ����� �������� ����, ���� ���� ���� ���-�� ������ �����
			// �� ���� -1!=position_mem
			if(-1!=position_mem)
			{
				last_time=timeGetTime();
				SetTimer(MHhwnd,1,MHSettings::timeout_after_move,NULL);
			}
		}
		else // ��� X �������� � ������ 4
		{
			// ������� ����������� �� ��� x
			if(X<-3*MHSettings::deadx/2) // ����� �� ������� �����
			{
				x_direction=-1;
				X=-3*MHSettings::deadx/2;
			}
			else if(X<-MHSettings::deadx/2) // � �������� ������, ����������� -1
			{
				x_direction=-1;
			}
			else if(X>3*MHSettings::deadx/2) // ����� �� ������� ������
			{
				x_direction=1;
				X=3*MHSettings::deadx/2;
			}
			else if(X>MHSettings::deadx/2) // � �������� ������, ����������� 1
			{
				x_direction=1;
			}
			else x_direction=0; // � �������� ������� ����

			if((true==MHSettings::flag_2moves)&&(0!=x_direction)) x_limitation=x_direction;
		}

		if(1==MHSettings::mode3axe) // �� �������� �� ��� Y �� ������ 3?
		{
			position=MHVector::NewValues(0,dy); // �������� �� ��� X �� �������

			if(0<=position) // -2=���� ����������� �� ������������� ���������, -1= ����������� �� ����������
			{
				// �� ��� y �������� ������ 2 �����������: 0 � 4
				// ����� �������� �� �����!!!
				if(0==position) y_direction=-1;
				else y_direction=1;
				
				position_mem=position; // ����� �� ��������� ������ �� �������?
			}
			else // ��������� ����������� �� x
			{
				y_direction=last_y_direction;
			}

			// ������ ������� ������ ��� ����� �������� ����, ���� ���� ���� ���-�� ������ �����
			// �� ���� -1!=position_mem
			if(-1!=position_mem)
			{
				last_time=timeGetTime();
				SetTimer(MHhwnd,1,MHSettings::timeout_after_move,NULL);
			}
		}
		else
		{
			// ������� ����������� �� ��� y
			if(Y<-3*MHSettings::deady/2) // ����� �� ������� �����
			{
				y_direction=-1;
				Y=-3*MHSettings::deady/2;
			}
			else if(Y<-MHSettings::deady/2) // � �������� ������, ����������� -1
			{
				y_direction=-1;
			}
			else if(Y>3*MHSettings::deady/2) // ����� �� ������� ������
			{
				y_direction=1;
				Y=3*MHSettings::deady/2;
			}
			else if(Y>MHSettings::deady/2) // � �������� ������, ����������� 1
			{
				y_direction=1;
			}
			else y_direction=0; // � �������� ������� ����
		}
/*
		// � ����� � ��� ��������? (��� X �� ������ �������� � ������ 3)
		if((true==MHSettings::flag_2moves)&&(0!=MHSettings::mode3axe))
		{
			if((1==last_x_direction)&&(1!=x_direction)) last_right=timeGetTime(); // ���� �� ������� ��������� � ��� �����
			if((-1==last_x_direction)&&(-1!=x_direction)) last_left=timeGetTime(); // ���� �� ������ ��������� � ��� �����

			// ����� �� ������ ����� ���������? ������ �� 100 �� � ���������� ��������?
			if((-1==x_direction)&&(-1!=last_x_direction)) // ���� ���, ��� � ����� � ��� ��� �������� 100 ��
			{
				X=0;
				x_direction=0;
				last_right=timeGetTime();
			}
		}
*/		
		if((last_y_direction!=y_direction)||(last_x_direction!=x_direction)) // ������� ������ ����������
		{
			position=button_pressed[x_direction+1][y_direction+1];
			if(-1==position) 
			{
				MHKeypad::Reset(); // ��������� ������� ������
				// ������-�� Reset �� �������� �����������
				//InvalidateRect(MHhwnd,NULL,TRUE);
			}
			else MHKeypad::Press(position,true);
		}
	}

	// ��� ��������� ����������� ��� ���� ������ ������
	InvalidateRect(MHhwnd,NULL,TRUE);

	if(!initialized) initialized=true;

	last_x=_x;
	last_y=_y;
	if(last_x<0) last_x=0;
	if(last_y<0) last_y=0;
	if(last_x>=screen_x) last_x=screen_x-1;
	if(last_y>=screen_y) last_y=screen_y-1;
	last_x_direction=x_direction;
	last_y_direction=y_direction;


#ifdef _DEBUG
	// �������� ���������� �������� ������� ���������
	//sprintf(debug_buf,"dx: %d  dy: %d\n", dx,dy);
	//OutputDebugString(debug_buf);
#endif
}

bool MHookHandler4::OnRDown()
{
	rbutton_pressed=true;
	MHKeypad::Reset(); // ��������� ������� ������
	X=0;Y=0;
	last_x_direction=0;
	last_y_direction=0;
	// ������-�� Reset �� �������� �����������
	InvalidateRect(MHhwnd,NULL,TRUE);
	return true; // ���������� ������ ����
}

bool MHookHandler4::OnRUp()
{
	rbutton_pressed=false;
	// �������� ����� ������ ��������
	MHVector::Reset();
	X=0;Y=0;
	last_x_direction=0;
	last_y_direction=0;
	//initialized=false;
	return true; // ���������� ������ ����
}

void MHookHandler4::OnDraw(HDC hdc, LONG window_size)
{
	RECT rect;
	LONG quad_x,quad_y;

	// ���-�����, �������-������
	if(1!=MHSettings::mode3axe) // �� �������� �� ��� Y �� ������ 3?
	{
		quad_x=0;
		//quad_y=Y*window_size/2/(MHSettings::deady*2); // ��������� 
		quad_y=Y*window_size/MHSettings::deady/3; 
		rect.left=(LONG)(window_size/2+quad_x-10);
		rect.top=(LONG)(window_size/2+quad_y-10);
		rect.right=rect.left+20;
		rect.bottom=rect.top+20;
		FillRect(hdc,&rect,yellow_brush);
	}

	// �������
	if(0!=MHSettings::mode3axe) // �� �������� �� ��� X �� ������ 3?
	{
		quad_y=0;
		quad_x=X*window_size/MHSettings::deadx/3; 
		rect.left=(LONG)(window_size/2+quad_x-10);
		rect.top=(LONG)(window_size/2+quad_y-10);
		rect.right=rect.left+20;
		rect.bottom=rect.top+20;
		FillRect(hdc,&rect,green_brush);
	}
	
}

void MHookHandler4::OnTimer()
{
	int position;

	if(-1==position_mem) return; // ������� �� ������, ������ ������

	// ���������, � �� �������� �� ���, ���������� � ������� ���������?
	// ��� ����� ��������, ������������� �� ������� ������ �����
	DWORD time_now=timeGetTime();
	if(time_now-last_time<MHSettings::timeout_after_move) return;

	// �������������, ����� ��������� �������!
	// !!! ����� �� ���, ��� � ������ 3 !!! ��������� ������ ������ �� ����� ��� !!!
	//MHKeypad::Press(position_mem,false);
	if(0==MHSettings::mode3axe) // �� �������� �� ��� � �� ������ 3?
	{
		// ������ x_direction=0;
		if(0!=last_x_direction) // ����������� �� ��� X ����������
		{
			last_x_direction=0;
			position=button_pressed[last_x_direction+1][last_y_direction+1];
			if(-1==position) 
			{
				MHKeypad::Reset(); // ��������� ������� ������
				// ������-�� Reset �� �������� �����������
				//InvalidateRect(MHhwnd,NULL,TRUE);
			}
			else MHKeypad::Press(position,true);
		}
	}

	if(1==MHSettings::mode3axe) // � ����� �������� ��� Y �� ������ 3?
	{
		// ������ y_direction=0;
		if(0!=last_y_direction) // ����������� �� ��� Y ����������
		{
			last_y_direction=0;
			position=button_pressed[last_x_direction+1][last_y_direction+1];
			if(-1==position) 
			{
				MHKeypad::Reset(); // ��������� ������� ������
				// ������-�� Reset �� �������� �����������
				//InvalidateRect(MHhwnd,NULL,TRUE);
			}
			else MHKeypad::Press(position,true);
		}
	}

	MHVector::Reset(); // ��� ��� �����������, ����� � ��� �� ����������� ���� �� ����� �������
	// ������-�� Reset �� �������� �����������
	InvalidateRect(MHhwnd,NULL,TRUE);

	position_mem=-1;
	KillTimer(MHhwnd,1);
}