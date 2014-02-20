// Обработчик хука, режим 2
#ifndef __MH_HOOKHANDLER2
#define __MH_HOOKHANDLER2

#include "HookHandler.h"

class MHookHandler2:public MHookHandler
{
public:
	virtual void OnMouseMove(LONG _x, LONG _y);
	virtual void OnRDown();
	virtual void OnRUp();
	virtual int GetPosition() { return position_mem;}
};

#endif