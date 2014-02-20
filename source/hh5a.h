// Обработчик хука, режим 5a
#ifndef __MH_HOOKHANDLER5a
#define __MH_HOOKHANDLER5a

#include "HookHandler.h"

class MHookHandler5a:public MHookHandler
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