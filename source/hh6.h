// ���������� ����, ����� 3
#ifndef __MH_HOOKHANDLER6
#define __MH_HOOKHANDLER6

#include "HookHandler.h"

class MHookHandler6:public MHookHandler
{
public:
	virtual void OnMouseMove(LONG _x, LONG _y);
	virtual void OnMouseScroll(LONG _x, LONG _y); // ������������ �������, ��� ��� ������� �����������
	virtual bool OnRDown();
	virtual bool OnRUp();
	virtual void OnTimer();
	virtual void Halt();
protected:
	DWORD last_time;
};

#endif