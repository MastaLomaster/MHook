// Рисует положение (джойстика) из битмапа
#ifndef __MH_BITMAP
#define __MH_BITMAP

class MHBitmap
{
public:
	static void Init(HWND hwnd);
	static void Halt();
	static void OnDraw(HDC hdc,int position);
protected:
	static HDC hdc4,hdc8;
	static HBITMAP hbm4, hbm8;
};

#endif
