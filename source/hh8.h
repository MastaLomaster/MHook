// Вот он, Battlefield!
#ifndef __MH_HOOKHANDLER8
#define __MH_HOOKHANDLER8

#include "HookHandler.h"

class MHookHandler8:public MHookHandler
{
public:
	virtual int OnMouseMove(LONG _x, LONG _y);
	virtual void OnTimer();
	virtual void Halt();
	virtual void OnDraw(HDC hdc, LONG window_size);

	virtual bool OnRDown(){return false;} // НЕ подавляйте правый клик
	virtual bool OnRUp(){return false;} // НЕ подавляйте правый клик
};

#endif