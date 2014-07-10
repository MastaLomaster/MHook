// Обработчик хука, виртуальный класс
#ifndef __MH_HOOKHANDLER
#define __MH_HOOKHANDLER

#include "MHKeypad.h"

class MHookHandler
{
public:
	MHookHandler():rbutton_pressed(false),initialized(false),position_mem(-1),mouse_path_squared(0){};
	virtual void OnMouseMove(LONG _x, LONG _y)=0;
	virtual void OnMouseScroll(LONG _x, LONG _y);
	virtual bool OnRDown()=0; // возвращает, нужно ли подавлять правый клик
	virtual bool OnRUp()=0;
	virtual void OnLDown();
	virtual void OnLUp();
	virtual int GetPosition() { return MHKeypad::GetPosition(); } // по умолчанию возвращаем нажатую клавишу
	virtual void OnTimer(){};
	virtual void OnDraw(HDC hdc, LONG window_size){}; // Нужна в 4 режиме для рисования квадратиков
	virtual void Halt(){};
	void HaltGeneral(){	rbutton_pressed=false; initialized=false; position_mem=-1; mouse_path_squared=0; }
	void Deinitialize(){initialized=false;} // Нужно, когда мы временно не обрабатываем перемещения мыши (пауза, скролл)

	void TopLeftCornerTimer(); // Тут левая кнопка мыши преображается
	void OnFastMove(LONG _dx, LONG _dy); // Определяет, был ли рывок мыши
	
protected:
	bool rbutton_pressed, initialized;
	LONG dx,dy,last_x,last_y;
	int position_mem;
	DWORD last_button5_time; // для нажатия на 5 кнопку при быстром движении мыши
	LONG mouse_path_squared;
};

#endif