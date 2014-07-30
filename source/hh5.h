// Обработчик хука, режим 5
#ifndef __MH_HOOKHANDLER5
#define __MH_HOOKHANDLER5


#include "HookHandler.h"

class MHookHandler5:public MHookHandler
{
public:
	virtual int OnMouseMove(LONG _x, LONG _y);
	virtual bool OnRDown();
	virtual bool OnRUp();
//	virtual void OnTimer();
//	virtual void Halt();
protected:
	DWORD last_time;
};
#endif