// ���������� ����, ����� 5
#ifndef __MH_HOOKHANDLER5
#define __MH_HOOKHANDLER5

#include "HookHandler.h"

class MHookHandler5:public MHookHandler
{
public:
	virtual void OnMouseMove(LONG _x, LONG _y);
	virtual void OnRDown();
	virtual void OnRUp();
	virtual void OnTimer();
	virtual void Halt();
protected:
	DWORD last_time;
};

#endif