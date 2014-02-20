// Обработчик хука, режим 3
#ifndef __MH_HOOKHANDLER3
#define __MH_HOOKHANDLER3

#include "HookHandler.h"

class MHookHandler3:public MHookHandler
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