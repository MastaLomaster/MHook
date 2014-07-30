// Обработчик хука, режим 4
#ifndef __MH_HOOKHANDLER4
#define __MH_HOOKHANDLER4

#include "HookHandler.h"

class MHookHandler4:public MHookHandler
{
public:
	MHookHandler4():x_limitation(0){};
	virtual int OnMouseMove(LONG _x, LONG _y);
	virtual bool OnRDown();
	virtual bool OnRUp();
	virtual void OnDraw(HDC hdc, LONG window_size); // Нужна в 4 режиме для рисования квадратиков
	virtual void OnTimer(); // Нужен, если одна из осей работает по режиму 3
	virtual void Halt();
protected:
	DWORD last_time, steps2timeout;
	LONG x_limitation;
};

#endif