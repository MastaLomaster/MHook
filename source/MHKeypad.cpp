#include <Windows.h>
#include "MHKeypad.h"
#include "Settings.h"
#include "MHRepErr.h"

#ifdef _DEBUG
#include <stdio.h>
static char debug_buf[4096];
#endif

extern HWND		MHhwnd;

int MHKeypad::keypad_position=-1;
// ���������� �������
WORD MHKeypad::scancode[15]={0xE048, 0xE04D, 0xE050, 0xE04B, 0x3B, 0x3B};

// ��� ������ ������
#ifdef _DEBUG
static int button_pressed[15]={0};
#endif

// ������� ����������� 8 ����������� �������� ���������
int key8[8][4]=
{
	{1,0,0,0}, // 0-� ����������� = ������� ������
	{1,1,0,0}, // 1-� ����������� = ������ 0 � 1
	{0,1,0,0}, // 2
	{0,1,1,0}, // 3
	{0,0,1,0}, // 4
	{0,0,1,1}, // 5
	{0,0,0,1}, // 6
	{1,0,0,1} // 7
};


void MHKeypad::Init(WORD _scancode0, WORD _scancode1, WORD _scancode2, WORD _scancode3, WORD _scancode4, WORD _scancode5,
	WORD _scancode6, WORD _scancode7, WORD _scancode8, WORD _scancode9, WORD _scancode10,
	WORD _scancode11, WORD _scancode12, WORD _scancode13, WORD _scancode14)
{
	if(-1!=keypad_position) Reset(); // ����� ������������������ ��������� ������

	scancode[0]=_scancode0;
	scancode[1]=_scancode1;
	scancode[2]=_scancode2;
	scancode[3]=_scancode3;
	scancode[4]=_scancode4;
	scancode[5]=_scancode5;

	scancode[6]=_scancode6;
	scancode[7]=_scancode7;
	scancode[8]=_scancode8;
	scancode[9]=_scancode9;
	scancode[10]=_scancode10;

	scancode[11]=_scancode11;
	scancode[12]=_scancode12;
	scancode[13]=_scancode13;
	scancode[14]=_scancode14;
}


// ���� ��������-�������� ���� �� 4 ������
void MHKeypad::Press4(int position, bool down, int shift)
{
#ifdef _DEBUG
	//if((position<0)||(position>3)) 
	if((position<0)||(position>10)) // ������ ������ � 5, � 10 (�� ������ ������ ����) 
 		MHReportError("�������� �������� � Press4");

	if(down&&(1==button_pressed[position+shift])) 
		MHReportError("��������� �������");

	if(!down&&(0==button_pressed[position+shift])) 
		MHReportError("��������� ����������");
	if(down) button_pressed[position+shift]=1; else button_pressed[position+shift]=0;
#endif

	// ����-������ 0xFFFF ������������
	if(0xFFFF==scancode[position+shift]) return;

	INPUT input={0};
	input.type=INPUT_KEYBOARD;
	input.ki.dwFlags = KEYEVENTF_SCANCODE;
	if(false==down)
	{
		input.ki.dwFlags|=KEYEVENTF_KEYUP;
	}

	if(scancode[position+shift]>0xFF) // ���� ����-��� �� ���� ������, ��� ������ - E0
	{
		input.ki.dwFlags|=KEYEVENTF_EXTENDEDKEY;
	}

	input.ki.wScan=scancode[position+shift];
	SendInput(1,&input,sizeof(INPUT));

#ifdef _DEBUG
	sprintf(debug_buf,"%d %d %d %d %d %d  %d %d %d %d  %d  %d %d %d %d\n",
		button_pressed[0],button_pressed[1],button_pressed[2],button_pressed[3],button_pressed[4],
		button_pressed[5],
		button_pressed[6],button_pressed[7],button_pressed[8],button_pressed[9],button_pressed[10],
		button_pressed[11],button_pressed[12],button_pressed[13],button_pressed[14]);
	OutputDebugString(debug_buf);
#endif
}


// �������� ������, �� �������� ���������� �������, ������������ �� 8 �������
// ������ keypad_position
// ����s���������� �����
void MHKeypad::Press(int position, bool down, int shift)
{
	// ������� ������ �������� �� ��������, � ������� - �� ��������
	if(down && (position==keypad_position))
	{
#ifdef _DEBUG
		//MHReportError("������ ��� ������� ������");
		OutputDebugString("������ ��� ������� ������");
#endif
		return;
	}

	if(!down && (position!=keypad_position))
	{
#ifdef _DEBUG
		//MHReportError("������ ��������� ������");
		OutputDebugString("������ ��������� ������");
#endif
		return;
	}

	if(-1==position) 
	{
#ifdef _DEBUG
//		MHReportError("������ ������ -1");
		OutputDebugString("������ ������ -1");
#endif
		return;
	}
	
		
	if(4==MHSettings::GetNumPositions())
	{
		if(true==down)
		{
			Reset(shift); // ��� ������� ��������� ��� ���������. � ��� ���������� �� ������ �����.
			keypad_position=position;
			//OutputDebugString("������\n");
		}
		else 
		{
			//OutputDebugString("���������\n");
			keypad_position=-1;
		}
		Press4(position,down,shift);
	}
	else // 8 �������, ��������� �� ���� ������, �������� �� ���� ������
	{
		if(true==down)
		{			
			
			for(int i=0;i<4;i++)
			{
				if(-1!=keypad_position) // ���������� ������� � ����� ����������� �� 4 ��������
				{
					if(key8[keypad_position][i]==key8[position][i]) continue; // ��� ������ �� ������

					if(1==key8[keypad_position][i]) Press4(i,false); // ���������
					else Press4(i,true); // ��������
				}
				else // �������� ������, ������ ��������
				{
					if(1==key8[position][i]) Press4(i,true);
				}
			}
			keypad_position=position;

			//OutputDebugString("������8\n");
		}
		else 
		{
			//OutputDebugString("���������8\n");
			Reset(); // ��� ������� ��������� ��� ���������. � ��� ���������� �� ������ �����.
			keypad_position=-1;
		}
	} // 8 �����������

	// ���������� ��������� �� ������
	InvalidateRect(MHhwnd,NULL,TRUE);
	
}

//====================================================================================
// �������� ��� ������� ������
//====================================================================================
void MHKeypad::Reset(int shift)
{
	if(4==MHSettings::GetNumPositions())
	{
		// �������� ���� ������
		if(-1!=keypad_position) Press4(keypad_position, false, shift);
	}
	else // ����� 8 ����������� ��������
	{
		switch(keypad_position)
		{
			// �������� ���� ������
		case 0:
		case 2:
		case 4:
		case 6:

			Press4(keypad_position/2, false);
			break;

			// �������� ��� ������
		case 1: // ������� � ������
			Press4(0, false);
			Press4(1, false);
			break;

		case 3: // ������ � ������
			Press4(1, false);
			Press4(2, false);
			break;
	
		case 5: // ������ � ������
			Press4(2, false);
			Press4(3, false);
			break;

		case 7: // ������� � ������
			Press4(0, false);
			Press4(3, false);
			break;
		}
		
	}
	keypad_position=-1;
}

//=========================================================================
// ��� 8 ������ ������ (8 ������)
//=========================================================================
void MHKeypad::Press8(int position, bool down)
{
	if(true==down)
	{
		// ������� �������� ������
		if(keypad_position!=-1)
		{
			if(keypad_position>3) Press4(keypad_position-4,false,6);
			else Press4(keypad_position,false,0);
		}
		keypad_position=position;
		//OutputDebugString("������\n");
	}
	else 
	{
		//OutputDebugString("���������\n");
		keypad_position=-1;
	}
	if(position>3) 	Press4(position-4,down,6);
	else Press4(position,down,0);

	// ���������� ��������� �� ������
	InvalidateRect(MHhwnd,NULL,TRUE);
}
