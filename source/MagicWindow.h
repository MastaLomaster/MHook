// Волшебные окна, нажимающие клавиши
#ifndef __MH_MAGICWND
#define __MH_MAGICWND

#define NUM_MAGIC_WINDOWS 24

class MagicWindow
{
public:
	int myindex; // Нужно, когда общается с диалогом установок (похоже, больше не используется)
	HWND MWhwnd; // Хендл окна
	int active; // Показывается ли?
	TCHAR mw_name[256]; // Надпись в окне
	int mw_color; // цвет окна
	int x,y,width,height; // координаты и размеры
	int button_or_switch; // 0 = кнопка, 1  = выключатель
	int mouse_or_eytracker; // 0 - мышь, 1 - айтрекер
	int button_index; // Номер клавиши в массиве коавиш
	int mw_group; // группа взаимоисключающих клавиш, от 1 до 4 (или 0=нет группы)
	bool pressed;
	bool f_inside_window;
	int flag_ignore_mouse_move; // При спрятывании и появлении окна генерируется ложное mouse_move
	TRACKMOUSEEVENT tme;

	static void Init();
	static void ShowEditable();
	static void ShowRuntime();
	static void Hide();
	static void OnTimer5();

	static MagicWindow magic_wnd[NUM_MAGIC_WINDOWS];
	
	void Press(); // нажимает или отпускает клавишу
	void PressSpecial(BYTE operation); // вместо нажатия на клавишу выполняет какую-нибудь хитрую операцию

	static bool editmode;
	static RECT adjust_rect; // корректировка размеров окна с учетом заголовка и рамки
protected:
	static bool initialized;

};

#endif