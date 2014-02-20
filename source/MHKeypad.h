// четыре клавиши, которые может нажимать. Возможно, две соседние
#ifndef __MH_KEYPAD
#define __MH_KEYPAD

class MHKeypad
{
public:
	static void Init(WORD _scancode0, WORD _scancode1, WORD _scancode2, WORD _scancode3, WORD _scancode4, WORD _scancode5,
		WORD _scancode6, WORD _scancode7, WORD _scancode8, WORD _scancode9, WORD _scancode10,
		WORD _scancode11, WORD _scancode12, WORD _scancode13, WORD _scancode14);
	static void Reset(int shift=0);
	static int GetPosition(){return keypad_position;};
	static void Press(int position, bool down, int shift=0);
	static void Press4(int position, bool down, int shift=0);
	static void Press8(int position, bool down); // Для 8 умений

protected:
	static int keypad_position;
	static WORD scancode[15];
};

#endif
