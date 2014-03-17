// Здесь хранятся все настройки
#ifndef __MH_SETTINGS
#define __MH_SETTINGS

#include "HookHandler.h"

class MHSettings
{
public:
	static int SettingsDialogue(HWND hwnd);

	// Число позиций на кейпаде (4 или 8)
	static int GetNumPositions(){return num_positions;}
	static void SetNumPositions(int _num_positions){num_positions=_num_positions;}
	// Чувствительность мыши
	static int GetMouseSensitivity(){return mouse_sensitivity;}
	static void SetMouseSensitivity(int _mouse_sensitivity){mouse_sensitivity=_mouse_sensitivity;}
	static MHookHandler *hh;
	static int GetPosition() {if(hh) return hh->GetPosition(); else return -1;}

	static DWORD time_between_pushes;
	static DWORD timeout_after_move;
	static LONG minimal_mouse_speed, timeout_mouse_switch, timeout_mouse_click, deadx, deady;
	static bool flag_enable_speed_button;
	static bool flag_2moves, flag_2moves_mode1, flag_change_direction_ontheway, flag_right_mb_iskey, flag_alt2,flag_no_move_right_mb,
		flag_mode5autoclick, flag_right_mb_doubleclick,
		flag_left_mb_push_twice, flag_right_mb_push_twice;
	static int mode,mode3axe;
	static int circle_scale_factor;
	static int OpenMHookConfig(HWND hwnd,char *default_filename=NULL);
	static int SaveMHookConfig(HWND hwnd);
	static void FillDialogue(HWND hdwnd);
	static void AfterLoad(HWND hdwnd);
	static void BeforeSaveOrStart(HWND hdwnd);
protected:
	static int num_positions, mouse_sensitivity;

};

#endif