// ���������� ����, ����������� �����
#ifndef __MH_HOOKHANDLER
#define __MH_HOOKHANDLER

#include "MHKeypad.h"

class MHookHandler
{
public:
	MHookHandler():rbutton_pressed(false),initialized(false),position_mem(-1),mouse_path_squared(0){};
	virtual void OnMouseMove(LONG _x, LONG _y)=0;
	virtual void OnMouseScroll(LONG _x, LONG _y);
	virtual bool OnRDown()=0; // ����������, ����� �� ��������� ������ ����
	virtual bool OnRUp()=0;
	virtual void OnLDown();
	virtual void OnLUp();
	virtual int GetPosition() { return MHKeypad::GetPosition(); } // �� ��������� ���������� ������� �������
	virtual void OnTimer(){};
	virtual void OnDraw(HDC hdc, LONG window_size){}; // ����� � 4 ������ ��� ��������� �����������
	virtual void Halt(){};
	void HaltGeneral(){	rbutton_pressed=false; initialized=false; position_mem=-1; mouse_path_squared=0; }
	void Deinitialize(){initialized=false;} // �����, ����� �� �������� �� ������������ ����������� ���� (�����, ������)

	void TopLeftCornerTimer(); // ��� ����� ������ ���� �������������
	void OnFastMove(LONG _dx, LONG _dy); // ����������, ��� �� ����� ����
	
protected:
	bool rbutton_pressed, initialized;
	LONG dx,dy,last_x,last_y;
	int position_mem;
	DWORD last_button5_time; // ��� ������� �� 5 ������ ��� ������� �������� ����
	LONG mouse_path_squared;
};

#endif