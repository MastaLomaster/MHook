#include <Windows.h>
#include <math.h>
#include "MVector.h"
#include "Settings.h"

extern LONG quad_x, quad_y; // ���������� ���������� � ����

#define MH_WINDOW_SIZE 200

static double PI = 3.14159265359;

int MHVector::x=0, MHVector::y=0, MHVector::vector_position=-1;

//===========================================================================================================
// ���� �������� � ����� �������� ����
// ���� ��� � ����������� ������� ������� - ����������, ����������
// ���� ��� � ������ ����������� - ���, ����� ������ �������� �� ������� ������������
// ���������� ����� ����� ������� 
// ��� -1, ���� ������� �� ����������
// ��� -2, ���� ���� ��������� ������������
// � ���������� quad_dx � quad_y ���������� ������, ������� ������������� ������������ �������� ���� (���������������)
//===========================================================================================================
int MHVector::NewValues(LONG dx, LONG dy)
{
	int angle, angle2, new_position;

	// 1. ����, � ����� ������� ��������� ����
	// 1.1. ������� ��������� � ������������� ������� ���������
	x+=dx; y+=dy;
		
	// 1.2. ������ �������, ����� �� ������ �� ������ ������
	//double s=sqrt((double)(x*x+y*y));
	if(sqrt((double)(x*x+y*y))<(double)MHSettings::GetMouseSensitivity()) return -2; //���� ��������� ������������

	// ����� ��������������� (�� 100) ������ �������� ����
	quad_x=(LONG)(MH_WINDOW_SIZE/2*x/sqrt((double)(x*x+y*y)));
	quad_y=(LONG)(MH_WINDOW_SIZE/2*y/sqrt((double)(x*x+y*y)));
	

	// 1.3 ���� ���� ������� �������
	angle=(int)(atan2((double)y,(double)x)*180.0/PI);
	// ������ ����� ��������! ������� �� ����������� �������� ����������, ���� �� �� �������� � ��� �� ����,
	// ��� � ����� �������� !!
	angle2=(int)(atan2((double)dy,(double)dx)*180.0/PI);
	if(angle2/45!=angle/45)
	{
		// �������� �� ���
		x=dx;y=dy;
		quad_x=(LONG)(MH_WINDOW_SIZE/2*x/sqrt((double)(x*x+y*y)));
		quad_y=(LONG)(MH_WINDOW_SIZE/2*y/sqrt((double)(x*x+y*y)));
		return -2;
	}

	// 1.4 �������, ������ ����� ��� �������������
	if(4==MHSettings::GetNumPositions())
	{
		// ������ ������� ����� -135 �������� � ����� �� ������� ������� (����� ������� ������� ������)
		angle=angle+135;
		if(angle<0) angle+=360;
		new_position=angle/90; // ���� �� ������ ������
		if(new_position>3) vector_position=3; // ����� ������ ����������? �� ���� ����� ������
	}
	else // ������ ������
	{
		// ������ ������� ����� -112 (��������, � ���������, �� ������) �������� � �� ������� ������� (����� ������� ������� ������)
		angle=angle+112;
		if(angle<0) angle+=360;
		new_position=angle/45; // ���� �� ������ ������
		if(new_position>7) vector_position=7; // ����� ������ ����������? �� ���� ����� ������
	}

	//  ��� ���� �������� ������
	// ���������� ������, ���������� �� ����, ������� ����������� ��� ���
	x=0;y=0;

	// 1.5 � �������� �� �� ����� ������?
	if(vector_position==new_position) 	return -1;
	else
	{
		vector_position=new_position;
		return vector_position;
	}
}
