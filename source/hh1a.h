// Обработчик хука, режим 1a
#ifndef __MH_HOOKHANDLER1a
#define __MH_HOOKHANDLER1a

#include "HookHandler.h"

class MHookHandler1a:public MHookHandler
{
public:
	virtual void OnMouseMove(LONG _x, LONG _y);
	virtual void OnRDown();
	virtual void OnRUp();
	virtual int GetPosition() { return position_mem;}
protected:
	DWORD last_time;
};

#endif