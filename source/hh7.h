// Пустышка, ничего кроме окон. .CPP нет
#ifndef __MH_HOOKHANDLER7
#define __MH_HOOKHANDLER7

#include "HookHandler.h"

class MHookHandler7:public MHookHandler
{
public:
	virtual int OnMouseMove(LONG _x, LONG _y){return 0;}
	virtual bool OnRDown(){return false;} // НЕ подавляйте правый клик
	virtual bool OnRUp(){return false;} // НЕ подавляйте правый клик
};

#endif