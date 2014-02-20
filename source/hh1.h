// Обработчик хука, режим 1
#ifndef __MH_HOOKHANDLER1
#define __MH_HOOKHANDLER1

#include "HookHandler.h"

class MHookHandler1:public MHookHandler
{
public:
	MHookHandler1():flag_opposite_direction(false){};
	virtual void OnMouseMove(LONG _x, LONG _y);
	virtual void OnRDown();
	virtual void OnRUp();
	virtual void OnDraw(HDC hdc, LONG window_size); // для отладки
	virtual void OnTimer(); // Нужен, когда нажата правая кнопка мыши, включающя режим 3
	virtual void Halt();
protected:
	bool flag_opposite_direction;
	DWORD opposite_time;
	DWORD last_time; // это для таймера 3-го режима
};

#endif