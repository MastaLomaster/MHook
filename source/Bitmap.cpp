#include <Windows.h>
#include "resource.h"
#include "Bitmap.h"
#include "Settings.h"

extern HINSTANCE	MHInst;

HDC MHBitmap::hdc4=0, MHBitmap::hdc8=0;
HBITMAP MHBitmap::hbm4=0, MHBitmap::hbm8=0;

void MHBitmap::Init(HWND hwnd)
{
	HDC hdc=GetDC(hwnd);

	hdc4=CreateCompatibleDC(hdc);
	hdc8=CreateCompatibleDC(hdc);

	hbm4=LoadBitmap(MHInst,MAKEINTRESOURCE(IDB_BITMAP4));
	hbm8=LoadBitmap(MHInst,MAKEINTRESOURCE(IDB_BITMAP8));

	SelectObject(hdc4,hbm4);
	SelectObject(hdc8,hbm8);
}

void MHBitmap::Halt()
{
	DeleteDC(hdc8);
	DeleteDC(hdc4);

	if(hbm4) DeleteObject(hbm4);
	if(hbm8) DeleteObject(hbm8);
}

void MHBitmap::OnDraw(HDC hdc,int position)
{
	switch(MHSettings::GetNumPositions())
	{
	case 4:
		// Сначала рисует незакрашенный кружочек
		BitBlt(hdc,0,0,200,200,hdc4,0,0,SRCCOPY);

		switch(position)
		{
		case 0: // стрелка вверх
			BitBlt(hdc,0,0,200,100,hdc4,200,0,SRCCOPY);
			break;
		case 1: // стрелка вправо
			BitBlt(hdc,100,0,100,200,hdc4,100,199,SRCCOPY);
			break;
		case 2: // стрелка вниз
			BitBlt(hdc,0,100,200,100,hdc4,200,100,SRCCOPY);
			break;
		case 3: // стрелка влево
			BitBlt(hdc,0,0,100,200,hdc4,0,199,SRCCOPY);
			break;
		} // switch position
		break;
	case 8:
		// Сначала рисует незакрашенный кружочек
		BitBlt(hdc,0,0,200,200,hdc8,0,0,SRCCOPY);

		switch(position)
		{
		case 0: // стрелка вверх
			BitBlt(hdc,0,0,200,100,hdc8,200,0,SRCCOPY);
			break;
		case 2: // стрелка вправо
			BitBlt(hdc,100,0,100,200,hdc8,100,200,SRCCOPY);
			break;
		case 4: // стрелка вниз
			BitBlt(hdc,0,100,200,100,hdc8,200,100,SRCCOPY);
			break;
		case 6: // стрелка влево
			BitBlt(hdc,0,0,100,200,hdc8,0,200,SRCCOPY);
			break;

			// Теперь промежуточные
		case 1: // стрелка вверх-вправо
			BitBlt(hdc,100,0,100,100,hdc8,300,201,SRCCOPY);
			break;

		case 3: // стрелка вниз-вправо
			BitBlt(hdc,100,100,100,100,hdc8,300,301,SRCCOPY);
			break;

		case 5: // стрелка вниз-влево
			BitBlt(hdc,0,100,100,100,hdc8,200,301,SRCCOPY);
			break;

		case 7: // стрелка вверх-вправо
			BitBlt(hdc,0,0,100,100,hdc8,200,201,SRCCOPY);
			break;
		} // switch position
		break;
	}
}