// ���������� ����, ����� 4
#ifndef __MH_HOOKHANDLER4
#define __MH_HOOKHANDLER4

#include "HookHandler.h"

class MHookHandler4:public MHookHandler
{
public:
	MHookHandler4():x_limitation(0){};
	virtual void OnMouseMove(LONG _x, LONG _y);
	virtual bool OnRDown();
	virtual bool OnRUp();
	virtual void OnDraw(HDC hdc, LONG window_size); // ����� � 4 ������ ��� ��������� �����������
	virtual void OnTimer(); // �����, ���� ���� �� ���� �������� �� ������ 3
protected:
	DWORD last_time, steps2timeout;
	LONG x_limitation;
};

#endif