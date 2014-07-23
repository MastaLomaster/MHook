// Здесь хранятся все настройки
#ifndef __MH_SETTINGS
#define __MH_SETTINGS

#include <stdio.h>
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
		flag_left_mb_push_twice, flag_right_mb_push_twice,
		flag_downall, flag_skip_fast, flag_up_immediately;
	static int mode,mode3axe;
	static int circle_scale_factor;
	static int OpenMHookConfig(HWND hwnd,TCHAR *default_filename=NULL);
	static int SaveMHookConfig(HWND hwnd);
	static void FillDialogue(HWND hdwnd);
	static void AfterLoad(HWND hdwnd);
	static void BeforeSaveOrStart(HWND hdwnd);

	// Для второго окна диалога
	static void FillDialogue2(HWND hdwnd);
	static void BeforeSaveOrStart2(HWND hdwnd);
	static void Save2(FILE *f);
	static int Load2(FILE *f);

protected:
	static int num_positions, mouse_sensitivity;

};

typedef struct
{
	TCHAR *stroka;
	int value;
} MHIntChar;

typedef struct
{
	TCHAR *stroka;
	WORD value;
} MHWORDChar;

#define MH_NUM_SCANCODES 103

extern MHWORDChar dlg_scancodes[MH_NUM_SCANCODES];

#endif