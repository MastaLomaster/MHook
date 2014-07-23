// Окно в виде кольца, следующего за взглядом
#ifndef __MH_CIRCLEWINDOW
#define __MH_CIRCLEWINDOW

class CircleWindow
{
public:
	static int Init();
	static void Show();
	static void Hide();
	static HWND CircleHwnd;
};

#endif