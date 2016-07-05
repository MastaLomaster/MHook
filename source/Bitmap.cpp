#include <Windows.h>
#include "resource.h"
#include "Bitmap.h"
#include "Settings.h"

extern HINSTANCE	MHInst;

HDC MHBitmap::hdc4=0, MHBitmap::hdc8=0;
HDC MHBitmap::hdc4red=0, MHBitmap::hdc8red=0;

HBITMAP MHBitmap::hbm4=0, MHBitmap::hbm8=0;
HBITMAP MHBitmap::hbm4red=0, MHBitmap::hbm8red=0;

extern HBRUSH red_brush;

extern bool right_button_down;
extern bool left_button_down;

void MHBitmap::Init(HWND hwnd)
{
	HDC hdc=GetDC(hwnd);

	hdc4=CreateCompatibleDC(hdc);
	hdc8=CreateCompatibleDC(hdc);
	hdc4red=CreateCompatibleDC(hdc);
	hdc8red=CreateCompatibleDC(hdc);

	hbm4=LoadBitmap(MHInst,MAKEINTRESOURCE(IDB_BITMAP4));
	hbm8=LoadBitmap(MHInst,MAKEINTRESOURCE(IDB_BITMAP8));
	hbm4red=LoadBitmap(MHInst,MAKEINTRESOURCE(IDB_BITMAP4RED));
	hbm8red=LoadBitmap(MHInst,MAKEINTRESOURCE(IDB_BITMAP8RED));


	SelectObject(hdc4,hbm4);
	SelectObject(hdc8,hbm8);
	SelectObject(hdc4red,hbm4red);
	SelectObject(hdc8red,hbm8red);
}

void MHBitmap::Halt()
{
	DeleteDC(hdc8);
	DeleteDC(hdc4);
	DeleteDC(hdc8red);
	DeleteDC(hdc4red);


	if(hbm4) DeleteObject(hbm4);
	if(hbm8) DeleteObject(hbm8);
	if(hbm4red) DeleteObject(hbm4red);
	if(hbm8red) DeleteObject(hbm8red);
}

void MHBitmap::OnDraw(HDC hdc,int position)
{
	switch(MHSettings::GetNumPositions())
	{
	case 4:
		// Сначала рисует незакрашенный кружочек
		BitBlt(hdc,0,0,200,200,hdc4,0,0,SRCCOPY);
		// Красные нажатия мыши
		if(left_button_down) BitBlt(hdc,0,0,100,100,hdc4red,0,0,SRCCOPY);
		if(right_button_down) BitBlt(hdc,100,0,100,100,hdc4red,100,0,SRCCOPY);

		switch(position)
		{
		case 0: // стрелка вверх
			BitBlt(hdc,0,0,200,100,hdc4,200,0,SRCCOPY);
			if(left_button_down) BitBlt(hdc,0,0,100,100,hdc4red,200,0,SRCCOPY);
			if(right_button_down) BitBlt(hdc,100,0,100,100,hdc4red,300,0,SRCCOPY);
			break;
		case 1: // стрелка вправо
			if(right_button_down) BitBlt(hdc,100,0,100,200,hdc4red,100,199,SRCCOPY);
			else BitBlt(hdc,100,0,100,200,hdc4,100,199,SRCCOPY);
			break;
		case 2: // стрелка вниз
			BitBlt(hdc,0,100,200,100,hdc4,200,100,SRCCOPY);
			break;
		case 3: // стрелка влево
			if(left_button_down) BitBlt(hdc,0,0,100,200,hdc4red,0,199,SRCCOPY);
			else BitBlt(hdc,0,0,100,200,hdc4,0,199,SRCCOPY);
			break;

		} // switch position
		break;
	case 8:
		// Сначала рисует незакрашенный кружочек
		BitBlt(hdc,0,0,200,200,hdc8,0,0,SRCCOPY);
		// Красные нажатия мыши
		if(left_button_down) BitBlt(hdc,0,0,100,100,hdc8red,0,0,SRCCOPY);
		if(right_button_down) BitBlt(hdc,100,0,100,100,hdc8red,100,0,SRCCOPY);

		switch(position)
		{
		case 0: // стрелка вверх
			BitBlt(hdc,0,0,200,100,hdc8,200,0,SRCCOPY);
			if(left_button_down) BitBlt(hdc,0,0,100,100,hdc8red,200,0,SRCCOPY);
			if(right_button_down) BitBlt(hdc,100,0,100,100,hdc8red,300,0,SRCCOPY);
			break;
		case 2: // стрелка вправо
			if(right_button_down) BitBlt(hdc,100,0,100,200,hdc8red,100,200,SRCCOPY);
			else BitBlt(hdc,100,0,100,200,hdc8,100,200,SRCCOPY);
			break;
		case 4: // стрелка вниз
			BitBlt(hdc,0,100,200,100,hdc8,200,100,SRCCOPY);
			break;
		case 6: // стрелка влево
			if(left_button_down) BitBlt(hdc,0,0,100,200,hdc8red,0,200,SRCCOPY);
			else BitBlt(hdc,0,0,100,200,hdc8,0,200,SRCCOPY);
			break;

			// Теперь промежуточные
		case 1: // стрелка вверх-вправо
			if(right_button_down) BitBlt(hdc,100,0,100,100,hdc8red,300,201,SRCCOPY);
			else BitBlt(hdc,100,0,100,100,hdc8,300,201,SRCCOPY);
			break;

		case 3: // стрелка вниз-вправо
			BitBlt(hdc,100,100,100,100,hdc8,300,301,SRCCOPY);
			break;

		case 5: // стрелка вниз-влево
			BitBlt(hdc,0,100,100,100,hdc8,200,301,SRCCOPY);
			break;

		case 7: // стрелка вверх-влево
			if(left_button_down) BitBlt(hdc,0,0,100,100,hdc8red,200,201,SRCCOPY);
			else BitBlt(hdc,0,0,100,100,hdc8,200,201,SRCCOPY);
			break;
		} // switch position
		break;
	}

	/*
	// 4/7/2016 добавляем квадратики нажатий на кнопки мыши
	static RECT rect1={10,170,30,190};
	static RECT rect2={170,170,190,190};
	if(left_button_down) FillRect(hdc,&rect1,red_brush);
	if(right_button_down) FillRect(hdc,&rect2,red_brush);
	*/
}