#include <windows.h>
#include <stdio.h>
#include "Settings.h"
#include "MHKeypad.h"
#include "MHRepErr.h"
#include "MVector.h"
#include "resource.h"
#include "hh1.h"
#include "hh1a.h"
#include "hh2.h"
#include "hh3.h"
#include "hh4.h"
#include "hh5.h"
#include "hh6.h"
#include "hh7.h"
#include "MagicWindow.h"

static char char_buf[4096];
static TCHAR tchar_buf[4096];

void ResetEytrackerBuffer(); // Определена в OnGazeData

extern HINSTANCE	MHInst;
extern HWND		MHhwnd;
extern HHOOK handle;
extern	bool flag_left_button_key;
extern bool flag_left_button_waits;
extern bool flag_right_button_waits;
extern	int top_position; 

static TCHAR *filter_MHOOK=L"файлы MHOOK\0*.MHOOK\0\0";
static TCHAR tfilename[1258];
//static char tfiletitle[1258]={"default.MHOOK"};
static TCHAR tfiletitle[1258];


int MHSettings::num_positions=4;
int MHSettings::mouse_sensitivity=1;
DWORD MHSettings::time_between_pushes=100; // 100 миллисекунд между нажатиями на клавиши
DWORD MHSettings::timeout_after_move=100; // 100 миллисекунд после последнего известия о движении мыши
LONG MHSettings::minimal_mouse_speed=900; //(квадрат числа пикселов за 1/10 секунды)
LONG MHSettings::timeout_mouse_switch=1500; // полторы секунды на переключение
LONG MHSettings::timeout_mouse_click=143; // 1/7 секунды держим клавишу нажатой
LONG MHSettings::deadx=100, MHSettings::deady=100; 
bool MHSettings::flag_enable_speed_button=false;
bool MHSettings::flag_2moves=false;
//bool MHSettings::flag_2moves_mode1=false;
bool MHSettings::flag_2moves_mode1=true;
bool MHSettings::flag_change_direction_ontheway=false;
bool MHSettings::flag_right_mb_iskey=false;
//bool MHSettings::flag_alt2=false; // Две альтернативные раскладки
bool MHSettings::flag_alt2=true; // Две альтернативные раскладки
bool MHSettings::flag_no_move_right_mb=false; // Флаг запрещает двигать мышь, когда нажата правая кнопка
//bool MHSettings::flag_no_move_right_mb=true; // Флаг запрещает двигать мышь, когда нажата правая кнопка
bool MHSettings::flag_mode5autoclick=false;
bool MHSettings::flag_right_mb_doubleclick=false; // Стоп эмуляции по двойному щелчку
bool MHSettings::flag_left_mb_push_twice=false; // нажимать клавишу также при отпускании ЛК мыши
bool MHSettings::flag_right_mb_push_twice=false; // нажимать клавишу также при отпускании ПК мыши
int MHSettings::circle_scale_factor=0;
bool MHSettings::flag_downall=false; // вниз и вбок = просто вниз (1 режим)
bool MHSettings::flag_skip_fast=false; // Быстрое движение мыши игнорируется
bool MHSettings::flag_up_immediately=false; // Только что нажатая кнопка должна быть отжата (1 режим)

int MHSettings::mode=1;
int MHSettings::mode3axe=0;

MHookHandler *MHSettings::hh=NULL;

MHookHandler1 hh1;
MHookHandler1a hh1a;
MHookHandler2 hh2;
MHookHandler3 hh3;
MHookHandler4 hh4;
MHookHandler5 hh5;
MHookHandler6 hh6;
MHookHandler7 hh7;

//=== Массивы для параметров в диалоге ===/



// Чувствительность мыши
#define MH_NUM_SENSITIVITY 6
static MHIntChar dlg_sensitivity[MH_NUM_SENSITIVITY]={{L"1",1},{L"5",5},{L"10",10},{L"25",25},{L"50",50},{L"100",100}}; 
static int dlg_current_sensitivity=2;


// Здесь нет PrtScr,Pause
MHWORDChar dlg_scancodes[MH_NUM_SCANCODES_EXTRA]=
{
	{L"<ничего>",0xFFFF}, // 0
	{L"вверх",0xE048},{L"вправо",0xE04D},{L"вниз",0xE050},{L"влево",0xE04B}, // 1-4
	{L"A",0x1E},{L"B",0x30},{L"C",0x2E},{L"D",0x20},{L"E",0x12}, // 5-9
	{L"F",0x21},{L"G",0x22},{L"H",0x23},{L"I",0x17},{L"J",0x24}, // 10-14
	{L"K",0x25},{L"L",0x26},{L"M",0x32},{L"N",0x31},{L"O",0x18}, // 15-19
	{L"P",0x19},{L"Q",0x10},{L"R",0x13},{L"S",0x1F},{L"T",0x14}, // 20-24
	{L"U",0x16},{L"V",0x2F},{L"W",0x11},{L"X",0x2D},{L"Y",0x15}, // 25-29
	{L"Z",0x2C},{L"0",0x0B},{L"1",0x02},{L"2",0x03},{L"3",0x04}, // 30-34
	{L"4",0x05},{L"5",0x06},{L"6",0x07},{L"7",0x08},{L"8",0x09}, // 35-39
	{L"9",0x0A},{L"~",0x29},{L"-",0x0C},{L"=",0x0D},{L"\\",0x2B}, // 40-44
	{L"[",0x1A},{L"]",0x1B},{L";",0x27},{L"'",0x28},{L",",0x33}, //45-49
	{L".",0x34},{L"/",0x35},{L"Backspace",0x0E},{L"пробел",0x39},{L"TAB",0x0F}, // 50-54
	{L"Caps Lock",0x3A},{L"Левый Shift",0x2A},{L"Левый Ctrl",0x1D},{L"Левый Alt",0x38},{L"Левый Win",0xE05B}, // 55-59
	{L"Правый Shift",0x36},{L"Правый Ctrl",0xE01D},{L"Правый Alt",0xE038},{L"Правый WIN",0xE05C},{L"Menu",0xE05D}, // 60-64
	{L"Enter",0x1C},{L"Esc",0x01},{L"F1",0x3B},{L"F2",0x3C},{L"F3",0x3D}, // 65-69
	{L"F4",0x3E},{L"F5",0x3F},{L"F6",0x40},{L"F7",0x41},{L"(F8 - запрещена) ",0xFFFF}, // 70-74
	{L"F9",0x43},{L"F10",0x44},{L"F11",0x57},{L"F12",0x58},{L"Scroll Lock",0x46}, // 75-79
	{L"Insert",0xE052},{L"(Delete - запрещена)",0xE053},{L"Home",0xE047},{L"End",0xE04F},{L"PgUp",0xE049}, // 80-84
	{L"PgDn",0xE051},{L"Num Lock",0x45},{L"Num /",0xE035},{L"Num *",0x37},{L"Num -",0x4A}, // 85-89
	{L"Num +",0x4E},{L"Num Enter",0xE01C},{L"(Num . - запрещена)",0xFFFF},{L"Num 0",0x52},{L"Num 1",0x4F}, // 90-94
	{L"Num 2",0x50},{L"Num 3",0x51},{L"Num 4",0x4B},{L"Num 5",0x4C},{L"Num 6",0x4D}, // 95-99
	{L"Num 7",0x47},{L"Num 8",0x48},{L"Num 9",0x49}, // 100-102
	// А теперь - дополнения для супер-окон!
	{L"ЛКМ+F12",0xE101},{L"Мышь влево",0xE102},{L"Мышь вправо",0xE103},{L"Скролл туда",0xE104},{L"Скролл сюда",0xE105}

}; 


//static int dlg_current_scancodes[11]={0,1,2,3,12,11,4,5,6,7,11};
//static int dlg_current_scancodes[15]={1,2,3,4,67,53,27,8,23,5,0,9,10,11,12};
static int dlg_current_scancodes[15]={27,8,23,5,67,53,  24,66,12,17, 0, 6,9,0,56};

// Таймаут после движения
#define MH_NUM_TIMEOUT 9
static MHIntChar dlg_timeout[MH_NUM_TIMEOUT]={{L"50 мс",50},{L"75 мс",75},{L"100 мс",100},{L"125 мс",125},{L"150 мс",150},
{L"200 мс",200},{L"250 мс",250},{L"0,5 секунды",500},{L"1 секунда",1000},}; 
static int dlg_current_timeout=0;

// Быстрая скорость движения мыши
#define MH_NUM_SPEED 7
static MHIntChar dlg_speed[MH_NUM_SPEED]={{L"100",100},{L"200",400},{L"300",900},{L"400",1600},{L"500",2500},{L"700",4900},{L"1000",10000}}; 
static int dlg_current_speed=3;

// Нужно для правильного сохранения и чтения конфигурации
#define MH_NUM_DIRECTIONS 2
static MHIntChar dlg_dirs[MH_NUM_DIRECTIONS]={{L"4",4},{L"8",8}};
static int dlg_current_direction=0;


// Таймаут переключения левой кнопки мыши
#define MH_NUM_SWITCH_TIMEOUT 6
static MHIntChar dlg_switch_timeout[MH_NUM_SWITCH_TIMEOUT]={{L"0,1",100},{L"0,5",500},{L"1",1000},{L"1,5",1000},{L"2",2000},{L"3",3000}}; 
static int dlg_current_switch_timeout=0;

// Количество пикселов в мертвой зоне
#define MH_DEAD_ZONES 4
static MHIntChar dlg_deadzones[MH_DEAD_ZONES]={{L"50",50},{L"100",100},{L"200",200},{L"25",25}}; 
static int dlg_current_deadzone_x=1,dlg_current_deadzone_y=1;

// Одна из осей в режиме 4 может работать в режиме 3
#define MH_NUM_MODE3AXE 3
static MHIntChar dlg_mode3axe[MH_NUM_MODE3AXE]={{L"не надо",-1},{L"на оси вправо-влево",0},{L"на оси вверх-вниз",1}}; 
static int dlg_current_mode3axe=0;

// В режиме 5 выбор колесом
#define MH_NUM_CIRCLE_SCALES 3
static MHIntChar dlg_circlescales[MH_NUM_CIRCLE_SCALES]={{L"не использовать",0},{L"50 пикселов",50},{L"100 пикселов",100}}; 
static int dlg_current_circlescale=0;

// static int res; // Selection result
// Прототип диалога номер два
BOOL CALLBACK DlgSettings2WndProc(HWND hdwnd,
						   UINT uMsg,
						   WPARAM wparam,
						   LPARAM lparam );

//===================================================================
// Диалог настроек
//===================================================================
static bool wasd_shown=true; // Показаны ли кнопки WSAD?

static BOOL CALLBACK DlgSettingsWndProc(HWND hdwnd,
						   UINT uMsg,
						   WPARAM wparam,
						   LPARAM lparam )
{
	
	switch(uMsg)
	{
	case WM_COMMAND:
		switch (LOWORD(wparam))
			{
			case IDC_BUTTON_DOPLNITELNO:
				MagicWindow::ShowEditable();
				DialogBox(MHInst,MAKEINTRESOURCE(IDD_DIALOG_SETTINGS2),hdwnd,(DLGPROC)DlgSettings2WndProc);
				MagicWindow::Hide();
				return 1;

			case IDC_BUTTON_WASD:
				if(wasd_shown)
				{
					wasd_shown=false;
					SendDlgItemMessage(hdwnd,IDC_UP, CB_SETCURSEL, 1, 0L);
					SendDlgItemMessage(hdwnd,IDC_RIGHT, CB_SETCURSEL, 2, 0L);
					SendDlgItemMessage(hdwnd,IDC_DOWN, CB_SETCURSEL, 3, 0L);
					SendDlgItemMessage(hdwnd,IDC_LEFT, CB_SETCURSEL, 4, 0L);
				}
				else
				{
					wasd_shown=true;
					SendDlgItemMessage(hdwnd,IDC_UP, CB_SETCURSEL, 27, 0L);
					SendDlgItemMessage(hdwnd,IDC_RIGHT, CB_SETCURSEL, 8, 0L);
					SendDlgItemMessage(hdwnd,IDC_DOWN, CB_SETCURSEL, 23, 0L);
					SendDlgItemMessage(hdwnd,IDC_LEFT, CB_SETCURSEL, 5, 0L);
				}
				return 1;

			case IDC_BUTTON_LOAD: // Грузим файл 
				MHSettings::OpenMHookConfig(hdwnd);
				MHSettings::AfterLoad(hdwnd);
				return 1;

			case IDC_BUTTON_SAVE: // Сохраняем файл 
				MHSettings::BeforeSaveOrStart(hdwnd); // Текущие поля диалога копирует в переменные
				MHSettings::SaveMHookConfig(hdwnd);
				return 1;

			case IDCANCEL: // Не случилось 
				EndDialog(hdwnd,2);
				return 1;
	
			case IDOK: 	//Хорошо!
				// 1. Чувствительность
				MHSettings::BeforeSaveOrStart(hdwnd);
				ResetEytrackerBuffer();
				EndDialog(hdwnd,0);
				return 1;
			} // switch WM_COMMAND
		break; // if WM_COMMAND 

	case WM_INITDIALOG:
		//SetWindowPos(hdwnd,NULL,50,50,0,0,SWP_NOSIZE);
		//SetWindowPos(hdwnd,HWND_TOPMOST,50,50,0,0,SWP_NOSIZE | SWP_NOREDRAW);
		SetWindowPos(hdwnd,HWND_TOP,50,50,0,0,SWP_NOSIZE | SWP_NOREDRAW);
		// Здесь не работает. 
		//SetWindowLong(hdwnd,GWL_STYLE,GetWindowLong(hdwnd,GWL_STYLE) | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);
			
		MHSettings::FillDialogue(hdwnd); // Заполняет списки
		MHSettings::AfterLoad(hdwnd); // Показываем текущие значения

		return 1; // Да, ставь фокус куда надо
		break;


	} // switch uMsg

 
return 0;
}

// Эта функция определена в HookProc
LRESULT  CALLBACK HookProc(int disabled,WPARAM wParam,LPARAM lParam);

//====================================================================================
// Заполнить выпадающие списки диалога возможными значениями
//====================================================================================
void MHSettings::FillDialogue(HWND hdwnd)
{
	int i;

	// Имя файла показать в диалоге
	SendDlgItemMessage(hdwnd,IDC_EDIT1, WM_SETTEXT, 0L, (LPARAM)tfiletitle);

	// Заполнить выпадающие списки с текущими значениями!
		// 1. Чувствительность
		for(i=0;i<MH_NUM_SENSITIVITY;i++)
		{
			SendDlgItemMessage(hdwnd,IDC_SENSITIVITY, CB_ADDSTRING, 0, (LPARAM)(dlg_sensitivity[i].stroka));
		}
		//SendDlgItemMessage(hdwnd,IDC_SENSITIVITY, CB_SETCURSEL, dlg_current_sensitivity, 0L);
		
		// 2. Клавиши
		for(i=0;i<MH_NUM_SCANCODES;i++)
		{
			SendDlgItemMessage(hdwnd,IDC_UP, CB_ADDSTRING, 0, (LPARAM)(dlg_scancodes[i].stroka));
			SendDlgItemMessage(hdwnd,IDC_RIGHT, CB_ADDSTRING, 0, (LPARAM)(dlg_scancodes[i].stroka));
			SendDlgItemMessage(hdwnd,IDC_DOWN, CB_ADDSTRING, 0, (LPARAM)(dlg_scancodes[i].stroka));
			SendDlgItemMessage(hdwnd,IDC_LEFT, CB_ADDSTRING, 0, (LPARAM)(dlg_scancodes[i].stroka));
			SendDlgItemMessage(hdwnd,IDC_BUTTON5, CB_ADDSTRING, 0, (LPARAM)(dlg_scancodes[i].stroka));
			SendDlgItemMessage(hdwnd,IDC_BUTTON6, CB_ADDSTRING, 0, (LPARAM)(dlg_scancodes[i].stroka));
			SendDlgItemMessage(hdwnd,IDC_UP2, CB_ADDSTRING, 0, (LPARAM)(dlg_scancodes[i].stroka));
			SendDlgItemMessage(hdwnd,IDC_RIGHT2, CB_ADDSTRING, 0, (LPARAM)(dlg_scancodes[i].stroka));
			SendDlgItemMessage(hdwnd,IDC_DOWN2, CB_ADDSTRING, 0, (LPARAM)(dlg_scancodes[i].stroka));
			SendDlgItemMessage(hdwnd,IDC_LEFT2, CB_ADDSTRING, 0, (LPARAM)(dlg_scancodes[i].stroka));
			SendDlgItemMessage(hdwnd,IDC_BUTTON7, CB_ADDSTRING, 0, (LPARAM)(dlg_scancodes[i].stroka));
			SendDlgItemMessage(hdwnd,IDC_UP3, CB_ADDSTRING, 0, (LPARAM)(dlg_scancodes[i].stroka));
			SendDlgItemMessage(hdwnd,IDC_RIGHT3, CB_ADDSTRING, 0, (LPARAM)(dlg_scancodes[i].stroka));
			SendDlgItemMessage(hdwnd,IDC_DOWN3, CB_ADDSTRING, 0, (LPARAM)(dlg_scancodes[i].stroka));
			SendDlgItemMessage(hdwnd,IDC_LEFT3, CB_ADDSTRING, 0, (LPARAM)(dlg_scancodes[i].stroka));
		}


		// 2.1. Мёртвые зоны
		for(i=0;i<MH_DEAD_ZONES;i++)
		{
			SendDlgItemMessage(hdwnd,IDC_DEADX, CB_ADDSTRING, 0, (LPARAM)(dlg_deadzones[i].stroka));
			SendDlgItemMessage(hdwnd,IDC_DEADY, CB_ADDSTRING, 0, (LPARAM)(dlg_deadzones[i].stroka));
		}


		// 2.2 Режим 3 для одной из осей в режиме 4
		for(i=0;i<MH_NUM_MODE3AXE;i++)
		{
			SendDlgItemMessage(hdwnd,IDC_MODE3AXE, CB_ADDSTRING, 0, (LPARAM)(dlg_mode3axe[i].stroka));
		}


		// 2.5. Минимальная скорость мыши
		for(i=0;i<MH_NUM_SPEED;i++)
		{
			SendDlgItemMessage(hdwnd,IDC_SPEED, CB_ADDSTRING, 0, (LPARAM)(dlg_speed[i].stroka));
		}


		// 2.6. Число направлений
		for(i=0;i<MH_NUM_DIRECTIONS;i++)
		{
			SendDlgItemMessage(hdwnd,IDC_DIRECTIONS, CB_ADDSTRING, 0, (LPARAM)(dlg_dirs[i].stroka));
		}


		// 3. Радио-кнопка

		// 4. Таймаут
		for(i=0;i<MH_NUM_TIMEOUT;i++)
		{
			SendDlgItemMessage(hdwnd,IDC_TIMEOUT, CB_ADDSTRING, 0, (LPARAM)(dlg_timeout[i].stroka));
		}

		
		// 4.5. Таймаут переключения левой кнопки мыши
		for(i=0;i<MH_NUM_SWITCH_TIMEOUT;i++)
		{
			SendDlgItemMessage(hdwnd,IDC_SWITCH_TIMEOUT, CB_ADDSTRING, 0, (LPARAM)(dlg_switch_timeout[i].stroka));
		}

		// 4.6. Чувствительность мыши при прокрутке колеса
		for(i=0;i<MH_NUM_CIRCLE_SCALES;i++)
		{
			SendDlgItemMessage(hdwnd,IDC_CIRCLE_SCALES, CB_ADDSTRING, 0, (LPARAM)(dlg_circlescales[i].stroka));
		}

}

//=======================================================================================
// Актуализировать в полях диалога загруженнные значения переменных
//=======================================================================================
void MHSettings::AfterLoad(HWND hdwnd)
{
//	int i;

	// Заполнить выпадающие списки с текущими значениями!
		// 1. Чувствительность
		SendDlgItemMessage(hdwnd,IDC_SENSITIVITY, CB_SETCURSEL, dlg_current_sensitivity, 0L);
		
		// 2. Клавиши
		SendDlgItemMessage(hdwnd,IDC_UP, CB_SETCURSEL, dlg_current_scancodes[0], 0L);
		SendDlgItemMessage(hdwnd,IDC_RIGHT, CB_SETCURSEL, dlg_current_scancodes[1], 0L);
		SendDlgItemMessage(hdwnd,IDC_DOWN, CB_SETCURSEL, dlg_current_scancodes[2], 0L);
		SendDlgItemMessage(hdwnd,IDC_LEFT, CB_SETCURSEL, dlg_current_scancodes[3], 0L);
		SendDlgItemMessage(hdwnd,IDC_BUTTON5, CB_SETCURSEL, dlg_current_scancodes[4], 0L);
		SendDlgItemMessage(hdwnd,IDC_BUTTON6, CB_SETCURSEL, dlg_current_scancodes[5], 0L);
		SendDlgItemMessage(hdwnd,IDC_UP2, CB_SETCURSEL, dlg_current_scancodes[6], 0L);
		SendDlgItemMessage(hdwnd,IDC_RIGHT2, CB_SETCURSEL, dlg_current_scancodes[7], 0L);
		SendDlgItemMessage(hdwnd,IDC_DOWN2, CB_SETCURSEL, dlg_current_scancodes[8], 0L);
		SendDlgItemMessage(hdwnd,IDC_LEFT2, CB_SETCURSEL, dlg_current_scancodes[9], 0L);
		SendDlgItemMessage(hdwnd,IDC_BUTTON7, CB_SETCURSEL, dlg_current_scancodes[10], 0L);
		SendDlgItemMessage(hdwnd,IDC_UP3, CB_SETCURSEL, dlg_current_scancodes[11], 0L);
		SendDlgItemMessage(hdwnd,IDC_RIGHT3, CB_SETCURSEL, dlg_current_scancodes[12], 0L);
		SendDlgItemMessage(hdwnd,IDC_DOWN3, CB_SETCURSEL, dlg_current_scancodes[13], 0L);
		SendDlgItemMessage(hdwnd,IDC_LEFT3, CB_SETCURSEL, dlg_current_scancodes[14], 0L);
		

		// 2.1. Мёртвые зоны
		SendDlgItemMessage(hdwnd,IDC_DEADX, CB_SETCURSEL, dlg_current_deadzone_x, 0L);
		SendDlgItemMessage(hdwnd,IDC_DEADY, CB_SETCURSEL, dlg_current_deadzone_y, 0L);

		// 2.2 Режим 3 для одной из осей в режиме 4
		SendDlgItemMessage(hdwnd,IDC_MODE3AXE, CB_SETCURSEL, dlg_current_mode3axe, 0L);

		// 2.5. Минимальная скорость мыши
		SendDlgItemMessage(hdwnd,IDC_SPEED, CB_SETCURSEL, dlg_current_speed, 0L);

		// 2.6. Число направлений
		// SendDlgItemMessage(hdwnd,IDC_DIRECTIONS, CB_SETCURSEL, MHSettings::GetNumPositions()/5, 0L);
		SendDlgItemMessage(hdwnd,IDC_DIRECTIONS, CB_SETCURSEL, dlg_current_direction, 0L);

		// 3. Радио-кнопка. Сначала все отпускаем (вот фигня-то!)
		SendDlgItemMessage(hdwnd, IDC_RADIO1, BM_SETCHECK, BST_UNCHECKED, 0);
		SendDlgItemMessage(hdwnd, IDC_RADIO2, BM_SETCHECK, BST_UNCHECKED, 0);
		SendDlgItemMessage(hdwnd, IDC_RADIO3, BM_SETCHECK, BST_UNCHECKED, 0);
		SendDlgItemMessage(hdwnd, IDC_RADIO4, BM_SETCHECK, BST_UNCHECKED, 0);
		SendDlgItemMessage(hdwnd, IDC_RADIO5, BM_SETCHECK, BST_UNCHECKED, 0);
		SendDlgItemMessage(hdwnd, IDC_RADIO6, BM_SETCHECK, BST_UNCHECKED, 0);
		SendDlgItemMessage(hdwnd, IDC_RADIO7, BM_SETCHECK, BST_UNCHECKED, 0);

		switch(MHSettings::mode)
		{
		case 1:
			SendDlgItemMessage(hdwnd, IDC_RADIO1, BM_SETCHECK, BST_CHECKED, 0);
			break;

		case 2:
			SendDlgItemMessage(hdwnd, IDC_RADIO2, BM_SETCHECK, BST_CHECKED, 0);
			break;

		case 3:
			SendDlgItemMessage(hdwnd, IDC_RADIO3, BM_SETCHECK, BST_CHECKED, 0);
			break;
			
		case 4:
			SendDlgItemMessage(hdwnd, IDC_RADIO4, BM_SETCHECK, BST_CHECKED, 0);
			break;

		case 5:
			SendDlgItemMessage(hdwnd, IDC_RADIO5, BM_SETCHECK, BST_CHECKED, 0);
			break;

		case 6:
			SendDlgItemMessage(hdwnd, IDC_RADIO6, BM_SETCHECK, BST_CHECKED, 0);
			break;
		
		case 7:
			SendDlgItemMessage(hdwnd, IDC_RADIO7, BM_SETCHECK, BST_CHECKED, 0);
			break;
		}
		
		// 3.2. Режим 2: Разрешать смену движения при нажатой правой кнопке мыши (
		// (кнопка не используется)
		SendDlgItemMessage(hdwnd, IDC_M2_CHECK1, BM_SETCHECK, BST_CHECKED, 0);
		
		// 4. Таймаут
		SendDlgItemMessage(hdwnd,IDC_TIMEOUT, CB_SETCURSEL, dlg_current_timeout, 0L);
		
		// 4.5. Таймаут переключения левой кнопки мыши
		SendDlgItemMessage(hdwnd,IDC_SWITCH_TIMEOUT, CB_SETCURSEL, dlg_current_switch_timeout, 0L);
		
		// 4.6. Чувствительность мыши при прокрутке колеса
		SendDlgItemMessage(hdwnd,IDC_CIRCLE_SCALES, CB_SETCURSEL, dlg_current_circlescale, 0L);


		// 6. Разрешить ли нажатие пятой кнопки при быстром движении мышью
		if(MHSettings::flag_enable_speed_button) SendDlgItemMessage(hdwnd, IDC_FAST_PUSH, BM_SETCHECK, BST_CHECKED, 0);
		else SendDlgItemMessage(hdwnd, IDC_FAST_PUSH, BM_SETCHECK, BST_UNCHECKED, 0);
		
		// 7. Использовать ли в 4 режиме движение в 2 шага
		if(MHSettings::flag_2moves) SendDlgItemMessage(hdwnd, IDC_CHECK_2MOVES, BM_SETCHECK, BST_CHECKED, 0);
		else SendDlgItemMessage(hdwnd, IDC_CHECK_2MOVES, BM_SETCHECK, BST_UNCHECKED, 0);

		// 8. Использовать ли в 1 режиме движение в 2 шага 
		if(MHSettings::flag_2moves_mode1) SendDlgItemMessage(hdwnd, IDC_CHECK_2MOVES_MODE1, BM_SETCHECK, BST_CHECKED, 0);
		else SendDlgItemMessage(hdwnd, IDC_CHECK_2MOVES_MODE1, BM_SETCHECK, BST_UNCHECKED, 0);

		// 9. можно ли менять направление движения на ходу 
		if(MHSettings::flag_change_direction_ontheway) SendDlgItemMessage(hdwnd, IDC_CHECK_CHANGE_DIRECTION_ONTHEWAY, BM_SETCHECK, BST_CHECKED, 0);
		else SendDlgItemMessage(hdwnd, IDC_CHECK_CHANGE_DIRECTION_ONTHEWAY, BM_SETCHECK, BST_UNCHECKED, 0);

		// 10. правая кнопка мыши вместо обычного поведения ведёт себя, как клавиша 
		if(MHSettings::flag_right_mb_iskey) SendDlgItemMessage(hdwnd, IDC_CHECK_RIGHT_MB_ISKEY, BM_SETCHECK, BST_CHECKED, 0);
		else SendDlgItemMessage(hdwnd, IDC_CHECK_RIGHT_MB_ISKEY, BM_SETCHECK, BST_UNCHECKED, 0);

		// 11. две альтернативные раскладки
		if(MHSettings::flag_alt2) SendDlgItemMessage(hdwnd, IDC_CHECK_2ALT, BM_SETCHECK, BST_CHECKED, 0);
		else SendDlgItemMessage(hdwnd, IDC_CHECK_2ALT, BM_SETCHECK, BST_UNCHECKED, 0);

		// 12. автоклик в режиме 5
		if(MHSettings::flag_mode5autoclick) SendDlgItemMessage(hdwnd, IDC_CHECK_AUTOCLICK, BM_SETCHECK, BST_CHECKED, 0);
		else SendDlgItemMessage(hdwnd, IDC_CHECK_AUTOCLICK, BM_SETCHECK, BST_UNCHECKED, 0);
		
		// 13. пауза по двойному щелчку
		if(MHSettings::flag_right_mb_doubleclick) SendDlgItemMessage(hdwnd, IDC_CHECK_RIGHT_DBLCLK, BM_SETCHECK, BST_CHECKED, 0);
		else SendDlgItemMessage(hdwnd, IDC_CHECK_RIGHT_DBLCLK, BM_SETCHECK, BST_UNCHECKED, 0);

		// 14. // нажимать клавишу также при отпускании ЛК мыши
		if(MHSettings::flag_left_mb_push_twice) SendDlgItemMessage(hdwnd, IDC_CHECK_LEFT_PUSH_TWICE, BM_SETCHECK, BST_CHECKED, 0);
		else SendDlgItemMessage(hdwnd, IDC_CHECK_LEFT_PUSH_TWICE, BM_SETCHECK, BST_UNCHECKED, 0);

		// 15. // нажимать клавишу также при отпускании ЛК мыши
		if(MHSettings::flag_right_mb_push_twice) SendDlgItemMessage(hdwnd, IDC_CHECK_RIGHT_PUSH_TWICE, BM_SETCHECK, BST_CHECKED, 0);
		else SendDlgItemMessage(hdwnd, IDC_CHECK_RIGHT_PUSH_TWICE, BM_SETCHECK, BST_UNCHECKED, 0);

		// 16. вниз+вбок = простол вниз (режим 1)
		if(MHSettings::flag_downall) SendDlgItemMessage(hdwnd, IDC_CHECK_DOWNALL, BM_SETCHECK, BST_CHECKED, 0);
		else SendDlgItemMessage(hdwnd, IDC_CHECK_DOWNALL, BM_SETCHECK, BST_UNCHECKED, 0);

		// 17. вниз+вбок сразу отпускать (режим 1)
		if(MHSettings::flag_up_immediately) SendDlgItemMessage(hdwnd, IDC_CHECK_UP_IMMEDIATELY, BM_SETCHECK, BST_CHECKED, 0);
		else SendDlgItemMessage(hdwnd, IDC_CHECK_UP_IMMEDIATELY, BM_SETCHECK, BST_UNCHECKED, 0);

		// 18. игнорировать быстрое движение (режим 3)
		if(MHSettings::flag_skip_fast) SendDlgItemMessage(hdwnd, IDC_CHECK_SKIP_FAST, BM_SETCHECK, BST_CHECKED, 0);
		else SendDlgItemMessage(hdwnd, IDC_CHECK_SKIP_FAST, BM_SETCHECK, BST_UNCHECKED, 0);

		//flag_left_mb_push_twice
		//IDC_CHECK_RIGHT_DBLCLK
}


extern bool flag_stop_emulation;
BOOL MHSettings::SettingsDialogue(HWND hwnd)
{
	bool restart_hook=false;
	BOOL return_code;

	if(MHSettings::hh)
	{
		// 0. Убрать окна
		MagicWindow::Hide();

		// 1. сначала остановить работающий хук
		restart_hook=true;
		UnhookWindowsHookEx(handle);

		// 2. Убить все таймеры
		KillTimer(hwnd,1);
		KillTimer(hwnd,2);
		KillTimer(hwnd,3);
		KillTimer(hwnd,4);
		KillTimer(hwnd,5);
		
		// 3. Сбрасываем MVector и MHKeypad и чё там ещё
		MHSettings::hh->Halt();
		MHSettings::hh->HaltGeneral();

		MHVector::Reset();
		MHKeypad::Reset(); // !!! А вдруг надо было с шифтом??? Тогда он сбросился в хук-хендлере!!! 

		// Почему-то Reset не включает перерисовку
		InvalidateRect(MHhwnd,NULL,TRUE);

		// 4. Текущий HookHandler обнуляем
		MHSettings::hh=NULL;
		flag_stop_emulation=false;
		flag_left_button_waits=false;
		flag_right_button_waits=false;
	}

	return_code=DialogBox(MHInst,MAKEINTRESOURCE(IDD_DIALOG_SETTINGS),hwnd,(DLGPROC)DlgSettingsWndProc);
	
	if((!return_code)&&(restart_hook))
	{
		// Проинициализировать хук хендлер (пока не знаю, что его нужно проинициализировать)
		// Продолжаем работать, восстанавливаем хук с начальными параметрами
		flag_left_button_key=false;
		top_position=-1; 
		handle = SetWindowsHookEx(WH_MOUSE_LL, 
									HookProc, 
                                 GetModuleHandle(NULL), 
                                 NULL);
	}

	if(!return_code)
	{
		MagicWindow::ShowRuntime(); // Здесь же взводится пятый таймер
	}

	return return_code;
}

//=======================================================================================================
//  Считывать конфигурацию
//=======================================================================================================


typedef enum {save_empty,save_int,save_bool,save_WORD, save_MagicWindows} T_save_type;

typedef struct
{
	char *name; // Название в файле конфигурации
	T_save_type save_type;
	void *pointer;
	T_save_type check_type;
	void *check_pointer;
	int max_index;
} T_save_struct;

#define NUM_SAVE_LINES 39
static T_save_struct save_struct[NUM_SAVE_LINES]=
{
	{"Sensitivity",save_int,&dlg_current_sensitivity,save_int,&dlg_sensitivity, MH_NUM_SENSITIVITY},

	{"Button0",save_int,&(dlg_current_scancodes[0]),save_WORD,&dlg_scancodes, MH_NUM_SCANCODES},
	{"Button1",save_int,&(dlg_current_scancodes[1]),save_WORD,&dlg_scancodes, MH_NUM_SCANCODES},
	{"Button2",save_int,&(dlg_current_scancodes[2]),save_WORD,&dlg_scancodes, MH_NUM_SCANCODES},
	{"Button3",save_int,&(dlg_current_scancodes[3]),save_WORD,&dlg_scancodes, MH_NUM_SCANCODES},
	{"Button4",save_int,&(dlg_current_scancodes[4]),save_WORD,&dlg_scancodes, MH_NUM_SCANCODES},
	{"Button5",save_int,&(dlg_current_scancodes[5]),save_WORD,&dlg_scancodes, MH_NUM_SCANCODES},

	{"Button6",save_int,&(dlg_current_scancodes[6]),save_WORD,&dlg_scancodes, MH_NUM_SCANCODES},
	{"Button7",save_int,&(dlg_current_scancodes[7]),save_WORD,&dlg_scancodes, MH_NUM_SCANCODES},
	{"Button8",save_int,&(dlg_current_scancodes[8]),save_WORD,&dlg_scancodes, MH_NUM_SCANCODES},
	{"Button9",save_int,&(dlg_current_scancodes[9]),save_WORD,&dlg_scancodes, MH_NUM_SCANCODES},
	{"Button10",save_int,&(dlg_current_scancodes[10]),save_WORD,&dlg_scancodes, MH_NUM_SCANCODES},

	{"Button11",save_int,&(dlg_current_scancodes[11]),save_WORD,&dlg_scancodes, MH_NUM_SCANCODES},
	{"Button12",save_int,&(dlg_current_scancodes[12]),save_WORD,&dlg_scancodes, MH_NUM_SCANCODES},
	{"Button13",save_int,&(dlg_current_scancodes[13]),save_WORD,&dlg_scancodes, MH_NUM_SCANCODES},
	{"Button14",save_int,&(dlg_current_scancodes[14]),save_WORD,&dlg_scancodes, MH_NUM_SCANCODES}, //16

	{"DeadzoneX",save_int,&dlg_current_deadzone_x,save_int,&dlg_deadzones, MH_DEAD_ZONES},
	{"DeadzoneY",save_int,&dlg_current_deadzone_y,save_int,&dlg_deadzones, MH_DEAD_ZONES}, //18
	
	// 2.2 Режим 3 для одной из осей в режиме 4
	{"Mode3Axe",save_int,&dlg_current_mode3axe,save_int,&dlg_mode3axe, MH_DEAD_ZONES},
	{"FastSpeed",save_int,&dlg_current_speed,save_int,&dlg_speed, MH_NUM_SPEED},
	{"Directions",save_int,&dlg_current_direction,save_int,&dlg_dirs, MH_NUM_DIRECTIONS},
	{"Mode",save_int,&MHSettings::mode,save_empty,NULL, 8}, // Количество режимов (на самом деле нулевого нет, то есть 7)

	// 4. Таймаут
	{"TimeoutMove",save_int,&dlg_current_timeout,save_int,dlg_timeout,MH_NUM_TIMEOUT}, //23
	// 4.5. Таймаут переключения левой кнопки мыши
	{"TimeoutSwitchLeftMB",save_int,&dlg_current_switch_timeout,save_int,dlg_switch_timeout,MH_NUM_SWITCH_TIMEOUT},
	// 6. Разрешить ли нажатие пятой кнопки при быстром движении мышью  и далее
	{"FastPush", save_bool, &MHSettings::flag_enable_speed_button,save_empty,0,0}, // 25
	{"2Moves", save_bool, &MHSettings::flag_2moves,save_empty,0,0},
	{"2MovesMode1", save_bool, &MHSettings::flag_2moves_mode1,save_empty,0,0},
	{"ChangeDirOnTheWay", save_bool, &MHSettings::flag_change_direction_ontheway,save_empty,0,0},
	{"RightMBisKey", save_bool, &MHSettings::flag_right_mb_iskey,save_empty,0,0},
	{"Alt2", save_bool, &MHSettings::flag_alt2,save_empty,0,0}, // 30
	{"Autoclick", save_bool, &MHSettings::flag_mode5autoclick,save_empty,0,0},
	{"CircleScale", save_int, &dlg_current_circlescale,save_int,dlg_circlescales,MH_NUM_CIRCLE_SCALES},
	{"RightMBDoubleClick", save_bool, &MHSettings::flag_right_mb_doubleclick,save_empty,0,0},

	{"LeftMBPushTwice", save_bool, &MHSettings::flag_left_mb_push_twice,save_empty,0,0}, // 34
	{"RightMBPushTwice", save_bool, &MHSettings::flag_right_mb_push_twice,save_empty,0,0},
	{"DownAll", save_bool, &MHSettings::flag_downall,save_empty,0,0},
	{"SkipFast", save_bool, &MHSettings::flag_skip_fast,save_empty,0,0},
	{"UpImmediately", save_bool, &MHSettings::flag_up_immediately,save_empty,0,0},

	{"MagicWindows", save_MagicWindows, 0,save_empty,0,0}//39 - сохраняет ВСЕ MagicWindows одним махом
};

int MHSettings::OpenMHookConfig(HWND hwnd, TCHAR *default_filename)
{
	if(NULL==default_filename)
	{
		// выводим диалог
		OPENFILENAME ofn=
		{
			sizeof(OPENFILENAME),
			hwnd,
			NULL, // в данном конкретном случае игнорируется
			filter_MHOOK,
			NULL,
			0, // Не используем custom filter
			0, // -"-
			tfilename,
			256,
			tfiletitle,
			256,
			NULL,
			L"Открыть файл конфигурации MHOOK",
			OFN_FILEMUSTEXIST | OFN_HIDEREADONLY ,
			0,
			0,
			L"MHOOK",
			0,0,0
		};

		// Диалог запроса имени файла
		if(0==GetOpenFileName(&ofn))
		{
			return 1;
		}
			
		// Имя файла показать в диалоге
		SendDlgItemMessage(hwnd,IDC_EDIT1, WM_SETTEXT, 0L, (LPARAM)tfiletitle);
	}
	else // Имя файла получено в качестве параметра функции
	{
		wcscpy_s(tfilename,default_filename);
	}

	FILE *fin=NULL;
	_wfopen_s(&fin,tfilename,L"r");
	if(NULL==fin)
	{
		wcscpy_s(tchar_buf,L"Не могу открыть файл: '");
		wcsncat_s(tchar_buf,tfilename,1000);
		wcsncat_s(tchar_buf,L"'",2);
		MHReportError(tchar_buf);
		return (-1);
	}

	int num_succeeded=0;
	int i;
	T_save_struct ss;
	bool found;

	// Сюда считываются числа
	int int_arg1, int_arg2;
	WORD WORD_arg;

	// Читаем все строки одну за другой
	while(1==fscanf_s(fin,"%s",char_buf, _countof(char_buf)))
	{
		found=false;
		for(i=0;i<NUM_SAVE_LINES;i++) // Перебираем все возможные параметры
		{
			if(0==strncmp(char_buf,save_struct[i].name,sizeof(char_buf)-1))
			{
				ss=save_struct[i];
				
				fgets(char_buf,sizeof(char_buf)-1,fin); // Остаток строки загоняем в буфер
				switch(ss.save_type)
				{
				case save_int:
					// Кроме индекса нужно считать ещё и значение одного из... N типов
					switch(ss.check_type)
					{
					case save_int:
						if(2!=sscanf_s(char_buf,"%d %d",&int_arg1,&int_arg2)) goto load_error;
						if((int_arg1<0)||(int_arg1>=ss.max_index)) goto load_error;
						// Проверка, что по указанному индексу лежит правлильное значение
						if( ((MHIntChar *)(ss.check_pointer) + int_arg1)->value != int_arg2 ) goto load_error; // индекс не соответствует значению
						*((int *)ss.pointer)=int_arg1; // Всё правильно, прописываем
						break;

					case save_WORD:
						if(2!=sscanf_s(char_buf,"%d %hx",&int_arg1,&WORD_arg)) goto load_error;
						if((int_arg1<0)||(int_arg1>=ss.max_index)) goto load_error;
						// Проверка, что по указанному индексу лежит правлильное значение
						if( ((MHWORDChar *)(ss.check_pointer) + int_arg1)->value != WORD_arg ) goto load_error; // индекс не соответствует значению
						//*((WORD *)ss.pointer)=WORD_arg; // Всё правильно, прописываем
						*((int *)ss.pointer)=int_arg1;
						break;

					case save_empty: // Берём не из списка значений, а прямо
						if(1!=sscanf_s(char_buf,"%d",&int_arg1)) goto load_error;
						if((int_arg1<0)||(int_arg1>=ss.max_index)) goto load_error;
						*((int *)ss.pointer)=int_arg1;
						break;

					default:
						goto load_error; // Не умеем обрабатывать
					} // switch check_type
					break;

				case save_bool:
					// Используем временную переменную типа int
					if(1!=sscanf_s(char_buf,"%d",&int_arg1)) goto load_error;
					*((bool *)ss.pointer)=int_arg1;
					break;

				case save_MagicWindows:
					// Для проверки, что количество окон совпадает
					if(1!=sscanf_s(char_buf,"%d",&int_arg1)) goto load_error;
					if(NUM_MAGIC_WINDOWS<int_arg1) goto load_error;
					if(Load2(fin,int_arg1)) goto load_error;
					break;

				default:
					goto load_error; // Не умеем обрабатывать
				}

				num_succeeded++; // Количество успешно считанных параметров
				found=true;
				break; // Не нужно больше сравнивать, выходим из цикла
			} // если найдена строка
			
		} // for
		if(!found) 
			goto load_error; // наткнулись на неизвестную строку
	}

#ifdef _DEBUG
	swprintf_s(tchar_buf,L"Файл конфигурации прочитан без ошибок.\r\nЧисло считанных параметров: %d", num_succeeded);
	MHReportError(tchar_buf,hwnd);
#endif
	fclose(fin);
	return 0;


load_error:
	swprintf_s(tchar_buf,L"Файл конфигурации прочитан с ошибками.\r\nВозможно, он от другой версии программы.\r\nОднако, число успешно считанных параметров: %d\r\n(Рекомендую сохранить конфигурацию заново)", num_succeeded);
	MHReportError(tchar_buf,hwnd);
	fclose(fin);
	return -1;
}

///

int MHSettings::SaveMHookConfig(HWND hwnd)
{
	// Сначала выводим диалог
	OPENFILENAME ofn=
	{
		sizeof(OPENFILENAME),
		hwnd,
		NULL, // в данном конкретном случае игнорируется
		filter_MHOOK,
		NULL,
		0, // Не используем custom filter
		0, // -"-
		tfilename,
		256,
		tfiletitle,
		256,
		NULL,
		L"Сохранить файл конфигурации MHOOK",
		OFN_OVERWRITEPROMPT,
		0,
		0,
		L"MHOOK",
		0,0,0
	};

	// Диалог запроса имени файла
	if(0==GetSaveFileName(&ofn))
	{
		return 1;
	}

	// Имя файла показать в диалоге
	SendDlgItemMessage(hwnd,IDC_EDIT1, WM_SETTEXT, 0L, (LPARAM)tfiletitle);

	FILE *fout=NULL;
	_wfopen_s(&fout,tfilename,L"w+");
	if(NULL==fout)
	{
		wcscpy_s(tchar_buf,L"Не могу создать файл: '");
		wcsncat_s(tchar_buf,tfilename,1000);
		wcsncat_s(tchar_buf,L"'",2);
		MHReportError(tchar_buf);
		return (-1);
	}

	T_save_struct ss;
	for(int i=0;i<NUM_SAVE_LINES;i++)
	{
		ss=save_struct[i];
		// Сохраняем Имя вне зависимости от типа
		fprintf(fout,"%s ",ss.name);
		
		// Основное значение (индекс)
		switch(ss.save_type)
		{
		case save_int:
			fprintf(fout,"%d ",*((int *)ss.pointer));
			break;

		case save_bool:
			fprintf(fout,"%d ",*((bool *)ss.pointer));
			break;

		case save_MagicWindows:
			Save2(fout);
			break;
		}

		// Проверочное значение
		switch(ss.check_type)
		{
		case save_int:
			fprintf(fout,"%d ",((MHIntChar *)(ss.check_pointer) + *((int *)ss.pointer))->value);
			break;

		case save_WORD:
			fprintf(fout,"0x%hX ",((MHWORDChar *)(ss.check_pointer) + *((int *)ss.pointer))->value);
			break;
		}


		// Перевод строки 
		fprintf(fout,"\n");
	}

	fclose(fout);
	return 0;
}

//===============================================================================================
// Копирует из полей диалога в реальные переменные
//===============================================================================================
void MHSettings::BeforeSaveOrStart(HWND hdwnd)
{
			dlg_current_sensitivity=SendDlgItemMessage(hdwnd,IDC_SENSITIVITY, CB_GETCURSEL, 0, 0L);
			MHSettings::SetMouseSensitivity(dlg_sensitivity[dlg_current_sensitivity].value);

			// 2. Кнопки
			dlg_current_scancodes[0]=SendDlgItemMessage(hdwnd,IDC_UP, CB_GETCURSEL, 0, 0L);
			dlg_current_scancodes[1]=SendDlgItemMessage(hdwnd,IDC_RIGHT, CB_GETCURSEL, 0, 0L);
			dlg_current_scancodes[2]=SendDlgItemMessage(hdwnd,IDC_DOWN, CB_GETCURSEL, 0, 0L);
			dlg_current_scancodes[3]=SendDlgItemMessage(hdwnd,IDC_LEFT, CB_GETCURSEL, 0, 0L);
			dlg_current_scancodes[4]=SendDlgItemMessage(hdwnd,IDC_BUTTON5, CB_GETCURSEL, 0, 0L);
			dlg_current_scancodes[5]=SendDlgItemMessage(hdwnd,IDC_BUTTON6, CB_GETCURSEL, 0, 0L);
			dlg_current_scancodes[6]=SendDlgItemMessage(hdwnd,IDC_UP2, CB_GETCURSEL, 0, 0L);
			dlg_current_scancodes[7]=SendDlgItemMessage(hdwnd,IDC_RIGHT2, CB_GETCURSEL, 0, 0L);
			dlg_current_scancodes[8]=SendDlgItemMessage(hdwnd,IDC_DOWN2, CB_GETCURSEL, 0, 0L);
			dlg_current_scancodes[9]=SendDlgItemMessage(hdwnd,IDC_LEFT2, CB_GETCURSEL, 0, 0L);
			dlg_current_scancodes[10]=SendDlgItemMessage(hdwnd,IDC_BUTTON7, CB_GETCURSEL, 0, 0L);
			dlg_current_scancodes[11]=SendDlgItemMessage(hdwnd,IDC_UP3, CB_GETCURSEL, 0, 0L);
			dlg_current_scancodes[12]=SendDlgItemMessage(hdwnd,IDC_RIGHT3, CB_GETCURSEL, 0, 0L);
			dlg_current_scancodes[13]=SendDlgItemMessage(hdwnd,IDC_DOWN3, CB_GETCURSEL, 0, 0L);
			dlg_current_scancodes[14]=SendDlgItemMessage(hdwnd,IDC_LEFT3, CB_GETCURSEL, 0, 0L);
						
			MHKeypad::Init(dlg_scancodes[dlg_current_scancodes[0]].value,
				dlg_scancodes[dlg_current_scancodes[1]].value,
				dlg_scancodes[dlg_current_scancodes[2]].value,
				dlg_scancodes[dlg_current_scancodes[3]].value,
				dlg_scancodes[dlg_current_scancodes[4]].value,
				dlg_scancodes[dlg_current_scancodes[5]].value,
				dlg_scancodes[dlg_current_scancodes[6]].value,
				dlg_scancodes[dlg_current_scancodes[7]].value,
				dlg_scancodes[dlg_current_scancodes[8]].value,
				dlg_scancodes[dlg_current_scancodes[9]].value,
				dlg_scancodes[dlg_current_scancodes[10]].value,
				dlg_scancodes[dlg_current_scancodes[11]].value,
				dlg_scancodes[dlg_current_scancodes[12]].value,
				dlg_scancodes[dlg_current_scancodes[13]].value,
				dlg_scancodes[dlg_current_scancodes[14]].value);
			

			// 2.1. Мёртвые зоны
			dlg_current_deadzone_x=SendDlgItemMessage(hdwnd,IDC_DEADX, CB_GETCURSEL, 0, 0L);
			MHSettings::deadx=dlg_deadzones[dlg_current_deadzone_x].value; 
			dlg_current_deadzone_y=SendDlgItemMessage(hdwnd,IDC_DEADY, CB_GETCURSEL, 0, 0L);
			MHSettings::deady=dlg_deadzones[dlg_current_deadzone_y].value; 

			// 2.2 Режим 3 для одной из осей в режиме 4
			dlg_current_mode3axe=SendDlgItemMessage(hdwnd,IDC_MODE3AXE, CB_GETCURSEL, 0, 0L);
			MHSettings::mode3axe=dlg_mode3axe[dlg_current_mode3axe].value; 

			// 2.5. Минимальная скорость мыши для нажатия на пятую кнопку
			dlg_current_speed=SendDlgItemMessage(hdwnd,IDC_SPEED, CB_GETCURSEL, 0, 0L);
			MHSettings::minimal_mouse_speed=dlg_speed[dlg_current_speed].value;

			// 2.6. Число направлений
			dlg_current_direction=SendDlgItemMessage(hdwnd,IDC_DIRECTIONS, CB_GETCURSEL, 0, 0L);
			MHSettings::SetNumPositions(dlg_dirs[dlg_current_direction].value);

			// 3. Радио-кнопка
			MHSettings::flag_no_move_right_mb=false; // Во всех режимах, кроме пятого
			if(BST_CHECKED==SendDlgItemMessage(hdwnd,IDC_RADIO1,BM_GETCHECK, 0, 0))
			{ 
				MHSettings::mode=1;
				MHSettings::hh=&hh1;
			}
			else if(BST_CHECKED==SendDlgItemMessage(hdwnd,IDC_RADIO2,BM_GETCHECK, 0, 0)) 
			{
				MHSettings::mode=2;
				MHSettings::hh=&hh2;
			}
			else if(BST_CHECKED==SendDlgItemMessage(hdwnd,IDC_RADIO3,BM_GETCHECK, 0, 0)) 
			{
				MHSettings::mode=3;
				MHSettings::hh=&hh3;
			}
			else if(BST_CHECKED==SendDlgItemMessage(hdwnd,IDC_RADIO4,BM_GETCHECK, 0, 0)) 
			{
				MHSettings::mode=4;
				MHSettings::hh=&hh4;
				// Важно!!! В 4 режиме принудительно выставить 8 позиций!!!
				MHSettings::SetNumPositions(8); dlg_current_direction=1;
			}
			else if(BST_CHECKED==SendDlgItemMessage(hdwnd,IDC_RADIO5,BM_GETCHECK, 0, 0)) 
			{
				MHSettings::mode=5;
				MHSettings::hh=&hh5;
				// Важно!!! В 5 режиме принудительно запрещается движение курсора при правом нажатии!!
				MHSettings::flag_no_move_right_mb=true;
				MHSettings::SetNumPositions(8); dlg_current_direction=1; // и принудительно выставить 8 позиций!!!
			}
			else if(BST_CHECKED==SendDlgItemMessage(hdwnd,IDC_RADIO6,BM_GETCHECK, 0, 0)) 
			{
				MHSettings::mode=6;
				MHSettings::hh=&hh6;
				// Важно!!! В 6 режиме принудительно выставить 4 позиций!!!
				MHSettings::SetNumPositions(4); dlg_current_direction=0;
			}
			else if(BST_CHECKED==SendDlgItemMessage(hdwnd,IDC_RADIO7,BM_GETCHECK, 0, 0)) 
			{
				MHSettings::mode=7;
				MHSettings::hh=&hh7;
			}

			//else

			// 3.2. - кнопка не используется

			// 4. Таймаут
			dlg_current_timeout=SendDlgItemMessage(hdwnd,IDC_TIMEOUT, CB_GETCURSEL, 0, 0L);
			MHSettings::timeout_after_move=dlg_timeout[dlg_current_timeout].value;

			// 4.5. Таймаут переключения режима левой кнопки мыши
			dlg_current_switch_timeout=SendDlgItemMessage(hdwnd,IDC_SWITCH_TIMEOUT, CB_GETCURSEL, 0, 0L);
			MHSettings::timeout_mouse_switch=dlg_switch_timeout[dlg_current_switch_timeout].value;

			// 4.6.
			dlg_current_circlescale=SendDlgItemMessage(hdwnd,IDC_CIRCLE_SCALES, CB_GETCURSEL, 0, 0L);
			MHSettings::circle_scale_factor=dlg_circlescales[dlg_current_circlescale].value;
			

			// 6. Разрешить ли нажатие пятой кнопки при быстром движении мышью
			if(BST_CHECKED==SendDlgItemMessage(hdwnd,IDC_FAST_PUSH,BM_GETCHECK, 0, 0))
				MHSettings::flag_enable_speed_button=true;
			else MHSettings::flag_enable_speed_button=false;

			// 7. Использовать ли в 4 режиме движение в 2 шага 
			if(BST_CHECKED==SendDlgItemMessage(hdwnd,IDC_CHECK_2MOVES,BM_GETCHECK, 0, 0))
				MHSettings::flag_2moves=true;
			else MHSettings::flag_2moves=false;

			// 8. Использовать ли в 1 режиме движение в 2 шага 
			if(BST_CHECKED==SendDlgItemMessage(hdwnd,IDC_CHECK_2MOVES_MODE1,BM_GETCHECK, 0, 0))
				MHSettings::flag_2moves_mode1=true;
			else MHSettings::flag_2moves_mode1=false;

			// 9. можно ли менять направление движения на ходу 
			if(BST_CHECKED==SendDlgItemMessage(hdwnd,IDC_CHECK_CHANGE_DIRECTION_ONTHEWAY,BM_GETCHECK, 0, 0))
				MHSettings::flag_change_direction_ontheway=true;
			else MHSettings::flag_change_direction_ontheway=false;
			
			// 10. правая кнопка мыши вместо обычного поведения ведёт себя, как клавиша 
			if(BST_CHECKED==SendDlgItemMessage(hdwnd,IDC_CHECK_RIGHT_MB_ISKEY,BM_GETCHECK, 0, 0))
				MHSettings::flag_right_mb_iskey=true;
			else MHSettings::flag_right_mb_iskey=false;

			// 11. две альтернативные раскладки
			if(BST_CHECKED==SendDlgItemMessage(hdwnd,IDC_CHECK_2ALT,BM_GETCHECK, 0, 0))
				MHSettings::flag_alt2=true;
			else MHSettings::flag_alt2=false;

			// 12. автоклик в режиме 5
			if(BST_CHECKED==SendDlgItemMessage(hdwnd,IDC_CHECK_AUTOCLICK,BM_GETCHECK, 0, 0))
				MHSettings::flag_mode5autoclick=true;
			else MHSettings::flag_mode5autoclick=false;

			// 13. Стоп эмуляции по двойному щелчку
			if(BST_CHECKED==SendDlgItemMessage(hdwnd,IDC_CHECK_RIGHT_DBLCLK,BM_GETCHECK, 0, 0))
				MHSettings::flag_right_mb_doubleclick=true;
			else MHSettings::flag_right_mb_doubleclick=false;

			// 14. нажимать клавишу также при отпускании ЛК мыши
			if(BST_CHECKED==SendDlgItemMessage(hdwnd,IDC_CHECK_LEFT_PUSH_TWICE,BM_GETCHECK, 0, 0))
				MHSettings::flag_left_mb_push_twice=true;
			else MHSettings::flag_left_mb_push_twice=false;

			// 15. нажимать клавишу также при отпускании ЛК мыши
			if(BST_CHECKED==SendDlgItemMessage(hdwnd,IDC_CHECK_RIGHT_PUSH_TWICE,BM_GETCHECK, 0, 0))
				MHSettings::flag_right_mb_push_twice=true;
			else MHSettings::flag_right_mb_push_twice=false;

			// 16. вниз+вбок = просто вниз (режим 1)
			if(BST_CHECKED==SendDlgItemMessage(hdwnd,IDC_CHECK_DOWNALL,BM_GETCHECK, 0, 0))
				MHSettings::flag_downall=true;
			else MHSettings::flag_downall=false;

			// 17. вниз+вбок сразу отпускать (режим 1)
			if(BST_CHECKED==SendDlgItemMessage(hdwnd,IDC_CHECK_UP_IMMEDIATELY,BM_GETCHECK, 0, 0))
				MHSettings::flag_up_immediately=true;
			else MHSettings::flag_up_immediately=false;


			// 18. игнорировать быстрое движение (режим 3)
			if(BST_CHECKED==SendDlgItemMessage(hdwnd,IDC_CHECK_SKIP_FAST,BM_GETCHECK, 0, 0))
				MHSettings::flag_skip_fast=true;
			else MHSettings::flag_skip_fast=false;
}