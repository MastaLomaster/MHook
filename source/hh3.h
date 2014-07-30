// Обработчик хука, режим 3
#ifndef __MH_HOOKHANDLER3
#define __MH_HOOKHANDLER3

#include "HookHandler.h"

class MHookHandler3:public MHookHandler
{
public:
	virtual int OnMouseMove(LONG _x, LONG _y);
	virtual bool OnRDown();
	virtual bool OnRUp();
	virtual void OnTimer();
	virtual void Halt();
protected:
	DWORD last_time;
};

#endif