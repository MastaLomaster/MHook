// Волшебные окна, нажимающие клавиши
#ifndef __MH_MAGICWND
#define __MH_MAGICWND

#define NUM_MAGIC_WINDOWS 2

class MagicWindow
{
public:
	HWND MWhwnd; // Хендл окна
	bool active; // Показывается ли?
	TCHAR mw_name[256]; // Надпись в окне
	int mw_color; // цвет окна
	LONG x,y,width,height; // координаты и размеры
	bool button_or_switch; // true = кнопка, false=выключатель
	int mouse_or_eytracker; // 1 - мышь, 2 - айтрекер, 3 - и то, и сё
	int button_index;
	bool pressed;
	bool f_inside_window;
	TRACKMOUSEEVENT tme;

	static void Init();
	static void ShowEditable();
	static void ShowRuntime();
	static void Hide();

	static MagicWindow magic_wnd[NUM_MAGIC_WINDOWS];
	
	void Press(); // нажимает или отпускает клавишу

	static bool editmode;
protected:
	static bool initialized;

};

#endif