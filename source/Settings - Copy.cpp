#include <windows.h>
#include "Settings.h"
#include "MHKeypad.h"
#include "MVector.h"
#include "resource.h"
#include "hh1.h"
#include "hh1a.h"
#include "hh2.h"
#include "hh3.h"
#include "hh4.h"


extern HINSTANCE	MHInst;
extern HWND		MHhwnd;
extern HHOOK handle;
extern	bool flag_left_button_key;
extern	int top_position; 


int MHSettings::num_positions=4;
int MHSettings::mouse_sensitivity=1;
DWORD MHSettings::time_between_pushes=100; // 100 ����������� ����� ��������� �� �������
DWORD MHSettings::timeout_after_move=100; // 100 ����������� ����� ���������� �������� � �������� ����
LONG MHSettings::minimal_mouse_speed=900; //(������� ����� �������� �� 1/10 �������)
LONG MHSettings::timeout_mouse_switch=1500; // ������� ������� �� ������������
LONG MHSettings::deadx=100, MHSettings::deady=100; 
bool MHSettings::flag_enable_speed_button=false;
bool MHSettings::flag_2moves=false;
//bool MHSettings::flag_2moves_mode1=false;
bool MHSettings::flag_2moves_mode1=true;
bool MHSettings::flag_change_direction_ontheway=false;
bool MHSettings::flag_right_mb_iskey=false;
//bool MHSettings::flag_alt2=false; // ��� �������������� ���������
bool MHSettings::flag_alt2=true; // ��� �������������� ���������

int MHSettings::mode=1;
int MHSettings::mode3axe=0;

MHookHandler *MHSettings::hh=NULL;

MHookHandler1 hh1;
MHookHandler1a hh1a;
MHookHandler2 hh2;
MHookHandler3 hh3;
MHookHandler4 hh4;

//=== ������� ��� ���������� � ������� ===/
typedef struct
{
	char *stroka;
	int value;
} MHIntChar;

typedef struct
{
	char *stroka;
	WORD value;
} MHWORDChar;


// ���������������� ����
#define MH_NUM_SENSITIVITY 6
static MHIntChar dlg_sensitivity[MH_NUM_SENSITIVITY]={{"1",1},{"5",5},{"10",10},{"25",25},{"50",50},{"100",100}}; 
static int dlg_current_sensitivity=2;

/*
// ����� ������� ��������
#define MH_NUM_SCANCODES 16
static MHWORDChar dlg_scancodes[MH_NUM_SCANCODES]=
{
	{"<������>",0xFFFF},
	{"�����",0xE048},{"������",0xE04D},{"����",0xE050},{"�����",0xE04B},
	{"W",0x11},{"D",0x20},{"S",0x1F},{"A",0x1E},
	{"Z",0x2C},{"X",0x2D},{"C",0x2E},
	{"������",0x39},{"F1",0x3B},{"TAB",0x0F},
	{"Esc",0x01}
};*/ 

#define MH_NUM_SCANCODES 103
// ����� ��� PrtScr,Pause
static MHWORDChar dlg_scancodes[MH_NUM_SCANCODES]=
{
	{"<������>",0xFFFF}, // 0
	{"�����",0xE048},{"������",0xE04D},{"����",0xE050},{"�����",0xE04B}, // 1-4
	{"A",0x1E},{"B",0x30},{"C",0x2E},{"D",0x20},{"E",0x12}, // 5-9
	{"F",0x21},{"G",0x22},{"H",0x23},{"I",0x17},{"J",0x24}, // 10-14
	{"K",0x25},{"L",0x26},{"M",0x32},{"N",0x31},{"O",0x18}, // 15-19
	{"P",0x19},{"Q",0x10},{"R",0x13},{"S",0x1F},{"T",0x14}, // 20-24
	{"U",0x16},{"V",0x2F},{"W",0x11},{"X",0x2D},{"Y",0x15}, // 25-29
	{"Z",0x2C},{"0",0x0B},{"1",0x02},{"2",0x03},{"3",0x04}, // 30-34
	{"4",0x05},{"5",0x06},{"6",0x07},{"7",0x08},{"8",0x09}, // 35-39
	{"9",0x0A},{"~",0x29},{"-",0x0C},{"=",0x0D},{"\\",0x2B}, // 40-44
	{"[",0x1A},{"]",0x1B},{";",0x27},{"'",0x28},{",",0x33}, //45-49
	{".",0x34},{"/",0x35},{"Backspace",0x0E},{"������",0x39},{"TAB",0x0F}, // 50-54
	{"Caps Lock",0x3A},{"����� Shift",0x2A},{"����� Ctrl",0x1D},{"����� Alt",0x38},{"����� Win",0xE05B}, // 55-59
	{"������ Shift",0x36},{"������ Ctrl",0xE01D},{"������ Alt",0xE038},{"������ WIN",0xE05C},{"Menu",0xE05D}, // 60-64
	{"Enter",0x1C},{"Esc",0x01},{"F1",0x3B},{"F2",0x3C},{"F3",0x3D}, // 65-69
	{"F4",0x3E},{"F5",0x3F},{"F6",0x40},{"F7",0x41},{"(F8 - ���������) ",0xFFFF}, // 70-74
	{"F9",0x43},{"F10",0x44},{"F11",0x57},{"F12",0x58},{"Scroll Lock",0x46}, // 75-79
	{"Insert",0xE052},{"(Delete - ���������)",0xE053},{"Home",0xE047},{"End",0xE04F},{"PgUp",0xE049}, // 80-84
	{"PgDn",0xE051},{"Num Lock",0x45},{"Num /",0xE035},{"Num *",0x37},{"Num -",0x4A}, // 85-89
	{"Num +",0x4E},{"Num Enter",0xE01C},{"(Num . - ���������)",0xFFFF},{"Num 0",0x52},{"Num 1",0x4F}, // 90-94
	{"Num 2",0x50},{"Num 3",0x51},{"Num 4",0x4B},{"Num 5",0x4C},{"Num 6",0x4D}, // 95-99
	{"Num 7",0x47},{"Num 8",0x48},{"Num 9",0x49} // 100-102

}; 


//static int dlg_current_scancodes[11]={0,1,2,3,12,11,4,5,6,7,11};
//static int dlg_current_scancodes[15]={1,2,3,4,67,53,27,8,23,5,0,9,10,11,12};
static int dlg_current_scancodes[15]={27,8,23,5,67,53,  24,66,12,17, 0, 6,9,0,56};

// ������� ����� ��������
#define MH_NUM_TIMEOUT 9
static MHIntChar dlg_timeout[MH_NUM_TIMEOUT]={{"50 ��",50},{"75 ��",75},{"100 ��",100},{"125 ��",125},{"150 ��",150},
{"200 ��",200},{"250 ��",250},{"0,5 �������",500},{"1 �������",1000},}; 
static int dlg_current_timeout=0;

// ������� �������� �������� ����
#define MH_NUM_SPEED 7
static MHIntChar dlg_speed[MH_NUM_SPEED]={{"100",100},{"200",400},{"300",900},{"400",1600},{"500",2500},{"700",4900},{"1000",10000}}; 
static int dlg_current_speed=3;


// ������� ������������ ����� ������ ����
#define MH_NUM_SWITCH_TIMEOUT 5
static MHIntChar dlg_switch_timeout[MH_NUM_SWITCH_TIMEOUT]={{"0,5",500},{"1",1000},{"1,5",1000},{"2",2000},{"3",3000}}; 
static int dlg_current_switch_timeout=3;

// ���������� �������� �������� ����
#define MH_DEAD_ZONES 3
static MHIntChar dlg_deadzones[MH_DEAD_ZONES]={{"50",50},{"100",100},{"200",200}}; 
static int dlg_current_deadzone_x=1,dlg_current_deadzone_y=1;

// ���� �� ���� � ������ 4 ����� �������� � ������ 3
#define MH_NUM_MODE3AXE 3
static MHIntChar dlg_mode3axe[MH_NUM_MODE3AXE]={{"�� ����",-1},{"�� ��� ������-�����",0},{"�� ��� �����-����",1}}; 
static int dlg_current_mode3axe=0;

// static int res; // Selection result
//===================================================================
// ������ ��������
//===================================================================
static bool wasd_shown=true; // �������� �� ������ WSAD?
static BOOL CALLBACK DlgSettingsWndProc(HWND hdwnd,
						   UINT uMsg,
						   WPARAM wparam,
						   LPARAM lparam )
{
if (uMsg==WM_COMMAND)
	{
	switch (LOWORD(wparam))
		{
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

		case IDC_BUTTON_LOAD: // ������ ���� 
			MHSettings::OpenMHookConfig(hdwnd);
			return 1;

		case IDC_BUTTON_SAVE: // ��������� ���� 
			MHSettings::SaveMHookConfig(hdwnd);
			return 1;

		case IDCANCEL: // �� ��������� 
			EndDialog(hdwnd,2);
			return 1;

		case IDOK: 	//������!
			// 1. ����������������
			dlg_current_sensitivity=SendDlgItemMessage(hdwnd,IDC_SENSITIVITY, CB_GETCURSEL, 0, 0L);
			MHSettings::SetMouseSensitivity(dlg_sensitivity[dlg_current_sensitivity].value);

			// 2. ������
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
			

			// 2.1. ̸����� ����
			dlg_current_deadzone_x=SendDlgItemMessage(hdwnd,IDC_DEADX, CB_GETCURSEL, 0, 0L);
			MHSettings::deadx=dlg_deadzones[dlg_current_deadzone_x].value; 
			dlg_current_deadzone_y=SendDlgItemMessage(hdwnd,IDC_DEADY, CB_GETCURSEL, 0, 0L);
			MHSettings::deady=dlg_deadzones[dlg_current_deadzone_y].value; 

			// 2.2 ����� 3 ��� ����� �� ���� � ������ 4
			dlg_current_mode3axe=SendDlgItemMessage(hdwnd,IDC_MODE3AXE, CB_GETCURSEL, 0, 0L);
			MHSettings::mode3axe=dlg_mode3axe[dlg_current_mode3axe].value; 

			// 2.5. ����������� �������� ���� ��� ������� �� ����� ������
			dlg_current_speed=SendDlgItemMessage(hdwnd,IDC_SPEED, CB_GETCURSEL, 0, 0L);
			MHSettings::minimal_mouse_speed=dlg_speed[dlg_current_speed].value;

			// 2.6. ����� �����������
			MHSettings::SetNumPositions(4*(1+SendDlgItemMessage(hdwnd,IDC_DIRECTIONS, CB_GETCURSEL, 0, 0L)));

			// 3. �����-������
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
			if(BST_CHECKED==SendDlgItemMessage(hdwnd,IDC_RADIO4,BM_GETCHECK, 0, 0)) 
			{
				MHSettings::mode=4;
				MHSettings::hh=&hh4;
				// �����!!! � 4 ������ ������������� ��������� 8 �������!!!
				MHSettings::SetNumPositions(8);
			}
			//else

			// 3.2. - ������ �� ������������

			// 4. �������
			dlg_current_timeout=SendDlgItemMessage(hdwnd,IDC_TIMEOUT, CB_GETCURSEL, 0, 0L);
			MHSettings::timeout_after_move=dlg_timeout[dlg_current_timeout].value;

			// 4.5. ������� ������������ ������ ����� ������ ����
			dlg_current_switch_timeout=SendDlgItemMessage(hdwnd,IDC_SWITCH_TIMEOUT, CB_GETCURSEL, 0, 0L);
			MHSettings::timeout_mouse_switch=dlg_switch_timeout[dlg_current_switch_timeout].value;

			// 6. ��������� �� ������� ����� ������ ��� ������� �������� �����
			if(BST_CHECKED==SendDlgItemMessage(hdwnd,IDC_FAST_PUSH,BM_GETCHECK, 0, 0))
				MHSettings::flag_enable_speed_button=true;
			else MHSettings::flag_enable_speed_button=false;

			// 7. ������������ �� � 4 ������ �������� � 2 ���� 
			if(BST_CHECKED==SendDlgItemMessage(hdwnd,IDC_CHECK_2MOVES,BM_GETCHECK, 0, 0))
				MHSettings::flag_2moves=true;
			else MHSettings::flag_2moves=false;

			// 8. ������������ �� � 1 ������ �������� � 2 ���� 
			if(BST_CHECKED==SendDlgItemMessage(hdwnd,IDC_CHECK_2MOVES_MODE1,BM_GETCHECK, 0, 0))
				MHSettings::flag_2moves_mode1=true;
			else MHSettings::flag_2moves_mode1=false;

			// 9. ����� �� ������ ����������� �������� �� ���� 
			if(BST_CHECKED==SendDlgItemMessage(hdwnd,IDC_CHECK_CHANGE_DIRECTION_ONTHEWAY,BM_GETCHECK, 0, 0))
				MHSettings::flag_change_direction_ontheway=true;
			else MHSettings::flag_change_direction_ontheway=false;
			
			// 10. ������ ������ ���� ������ �������� ��������� ���� ����, ��� ������� 
			if(BST_CHECKED==SendDlgItemMessage(hdwnd,IDC_CHECK_RIGHT_MB_ISKEY,BM_GETCHECK, 0, 0))
				MHSettings::flag_right_mb_iskey=true;
			else MHSettings::flag_right_mb_iskey=false;

			// 10. ��� �������������� ���������
			if(BST_CHECKED==SendDlgItemMessage(hdwnd,IDC_CHECK_2ALT,BM_GETCHECK, 0, 0))
				MHSettings::flag_alt2=true;
			else MHSettings::flag_alt2=false;

			
			// (WORD _scancode0, WORD _scancode1, WORD _scancode2, WORD _scancode3);
		/*	if(BST_CHECKED==SendDlgItemMessage(hdwnd,IDC_CHECK_EDEMPOKNOPKE,BM_GETCHECK, 0, 0))
				flag_edem_po_knopke=true;
*/
			EndDialog(hdwnd,0);
			return 1;
		} // switch WM_COMMAND

	
	}// if WM_COMMAND 

	if (uMsg==WM_INITDIALOG)
	{
		int i;

		SetWindowPos(hdwnd,NULL,50,50,0,0,SWP_NOSIZE);

		// ��������� ���������� ������ � �������� ����������!
		// 1. ����������������
		for(i=0;i<MH_NUM_SENSITIVITY;i++)
		{
			SendDlgItemMessage(hdwnd,IDC_SENSITIVITY, CB_ADDSTRING, 0, (LPARAM)(dlg_sensitivity[i].stroka));
		}
		SendDlgItemMessage(hdwnd,IDC_SENSITIVITY, CB_SETCURSEL, dlg_current_sensitivity, 0L);
		
		// 2. �������
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
		

		// 2.1. ̸����� ����
		for(i=0;i<MH_DEAD_ZONES;i++)
		{
			SendDlgItemMessage(hdwnd,IDC_DEADX, CB_ADDSTRING, 0, (LPARAM)(dlg_deadzones[i].stroka));
			SendDlgItemMessage(hdwnd,IDC_DEADY, CB_ADDSTRING, 0, (LPARAM)(dlg_deadzones[i].stroka));
		}
		SendDlgItemMessage(hdwnd,IDC_DEADX, CB_SETCURSEL, dlg_current_deadzone_x, 0L);
		SendDlgItemMessage(hdwnd,IDC_DEADY, CB_SETCURSEL, dlg_current_deadzone_y, 0L);

		// 2.2 ����� 3 ��� ����� �� ���� � ������ 4
		for(i=0;i<MH_NUM_MODE3AXE;i++)
		{
			SendDlgItemMessage(hdwnd,IDC_MODE3AXE, CB_ADDSTRING, 0, (LPARAM)(dlg_mode3axe[i].stroka));
		}
		SendDlgItemMessage(hdwnd,IDC_MODE3AXE, CB_SETCURSEL, dlg_current_mode3axe, 0L);

		// 2.5. ����������� �������� ����
		for(i=0;i<MH_NUM_SPEED;i++)
		{
			SendDlgItemMessage(hdwnd,IDC_SPEED, CB_ADDSTRING, 0, (LPARAM)(dlg_speed[i].stroka));
		}
		SendDlgItemMessage(hdwnd,IDC_SPEED, CB_SETCURSEL, dlg_current_speed, 0L);

		// 2.6. ����� �����������
		SendDlgItemMessage(hdwnd,IDC_DIRECTIONS, CB_ADDSTRING, 0, (LPARAM)"4 �����������");
		SendDlgItemMessage(hdwnd,IDC_DIRECTIONS, CB_ADDSTRING, 0, (LPARAM)"8 �����������");
		SendDlgItemMessage(hdwnd,IDC_DIRECTIONS, CB_SETCURSEL, MHSettings::GetNumPositions()/5, 0L);


		// 3. �����-������
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
		}
		
		// 3.2. ����� 2: ��������� ����� �������� ��� ������� ������ ������ ���� (
		// (������ �� ������������)
		SendDlgItemMessage(hdwnd, IDC_M2_CHECK1, BM_SETCHECK, BST_CHECKED, 0);
		
		// 4. �������
		for(i=0;i<MH_NUM_TIMEOUT;i++)
		{
			SendDlgItemMessage(hdwnd,IDC_TIMEOUT, CB_ADDSTRING, 0, (LPARAM)(dlg_timeout[i].stroka));
		}
		SendDlgItemMessage(hdwnd,IDC_TIMEOUT, CB_SETCURSEL, dlg_current_timeout, 0L);
		
		// 4.5. ������� ������������ ����� ������ ����
		for(i=0;i<MH_NUM_SWITCH_TIMEOUT;i++)
		{
			SendDlgItemMessage(hdwnd,IDC_SWITCH_TIMEOUT, CB_ADDSTRING, 0, (LPARAM)(dlg_switch_timeout[i].stroka));
		}
		SendDlgItemMessage(hdwnd,IDC_SWITCH_TIMEOUT, CB_SETCURSEL, dlg_current_switch_timeout, 0L);
		
		// 6. ��������� �� ������� ����� ������ ��� ������� �������� �����
		if(MHSettings::flag_enable_speed_button) SendDlgItemMessage(hdwnd, IDC_FAST_PUSH, BM_SETCHECK, BST_CHECKED, 0);
		else SendDlgItemMessage(hdwnd, IDC_FAST_PUSH, BM_SETCHECK, BST_UNCHECKED, 0);
		
		// 7. ������������ �� � 4 ������ �������� � 2 ����
		if(MHSettings::flag_2moves) SendDlgItemMessage(hdwnd, IDC_CHECK_2MOVES, BM_SETCHECK, BST_CHECKED, 0);
		else SendDlgItemMessage(hdwnd, IDC_CHECK_2MOVES, BM_SETCHECK, BST_UNCHECKED, 0);

		// 8. ������������ �� � 1 ������ �������� � 2 ���� 
		if(MHSettings::flag_2moves_mode1) SendDlgItemMessage(hdwnd, IDC_CHECK_2MOVES_MODE1, BM_SETCHECK, BST_CHECKED, 0);
		else SendDlgItemMessage(hdwnd, IDC_CHECK_2MOVES_MODE1, BM_SETCHECK, BST_UNCHECKED, 0);

		// 9. ����� �� ������ ����������� �������� �� ���� 
		if(MHSettings::flag_change_direction_ontheway) SendDlgItemMessage(hdwnd, IDC_CHECK_CHANGE_DIRECTION_ONTHEWAY, BM_SETCHECK, BST_CHECKED, 0);
		else SendDlgItemMessage(hdwnd, IDC_CHECK_CHANGE_DIRECTION_ONTHEWAY, BM_SETCHECK, BST_UNCHECKED, 0);

		// 10. ������ ������ ���� ������ �������� ��������� ���� ����, ��� ������� 
		if(MHSettings::flag_right_mb_iskey) SendDlgItemMessage(hdwnd, IDC_CHECK_RIGHT_MB_ISKEY, BM_SETCHECK, BST_CHECKED, 0);
		else SendDlgItemMessage(hdwnd, IDC_CHECK_RIGHT_MB_ISKEY, BM_SETCHECK, BST_UNCHECKED, 0);

		// 10. ��� �������������� ���������
		if(MHSettings::flag_alt2) SendDlgItemMessage(hdwnd, IDC_CHECK_2ALT, BM_SETCHECK, BST_CHECKED, 0);
		else SendDlgItemMessage(hdwnd, IDC_CHECK_2ALT, BM_SETCHECK, BST_UNCHECKED, 0);

		return 1; // ��, ����� ����� ���� ����
	}
  
return 0;
}

// ��� ������� ���������� � HookProc
LRESULT  CALLBACK HookProc(int disabled,WPARAM wParam,LPARAM lParam);

BOOL MHSettings::SettingsDialogue(HWND hwnd)
{
	bool restart_hook=false;
	BOOL return_code;

	if(MHSettings::hh)
	{
		// 1. ������� ���������� ���������� ���
		restart_hook=true;
		UnhookWindowsHookEx(handle);

		// 2. ����� ��� �������
		KillTimer(hwnd,1);
		KillTimer(hwnd,2);
		
		// 3. ���������� MVector � MHKeypad � �� ��� ���
		MHSettings::hh->Halt();
		MHSettings::hh->HaltGeneral();

		MHVector::Reset();
		MHKeypad::Reset(); // !!! � ����� ���� ���� � ������??? ����� �� ��������� � ���-��������!!! 

		// ������-�� Reset �� �������� �����������
		InvalidateRect(MHhwnd,NULL,TRUE);

		// 4. ������� HookHandler ��������
		MHSettings::hh=NULL;
		
	}
	return_code=DialogBox(MHInst,MAKEINTRESOURCE(IDD_DIALOG_SETTINGS),hwnd,(DLGPROC)DlgSettingsWndProc);
	if((!return_code)&&(restart_hook))
	{
		// ������������������� ��� ������� (���� �� ����, ��� ��� ����� �������������������)
		// ���������� ��������, ��������������� ��� � ���������� �����������
		flag_left_button_key=false;
		top_position=-1; 
		handle = SetWindowsHookEx(WH_MOUSE_LL, 
									HookProc, 
                                 GetModuleHandle(NULL), 
                                 NULL);
	}

	return return_code;
}

//=======================================================================================================
//  ��������� ������������
//=======================================================================================================
static char *filter_MHOOK="����� MHOOK\0*.MHOOK\0\0";
static char tfilename[1258];
static char tfiletitle[1258]={"default.MHOOK"};

typedef enum {save_empty,save_int,save_bool,save_DWORD,save_LONG} T_save_type;

typedef struct
{
	char *name; // �������� � ����� ������������
	T_save_type save_type;
	void *pointer;
	T_save_type check_type;
	void *check_pointer;
} T_save_struct;

#define NUM_SAVE_LINES 1
static T_save_struct save_struct[NUM_SAVE_LINES]=
{
	{""}
}

int MHSettings::OpenMHookConfig(HWND hwnd)
{
	// ������� ������
	OPENFILENAME ofn=
	{
		sizeof(OPENFILENAME),
		hwnd,
		NULL, // � ������ ���������� ������ ������������
		filter_MHOOK,
		NULL,
		0, // �� ���������� custom filter
		0, // -"-
		tfilename,
		256,
		tfiletitle,
		256,
		NULL,
		"������� ���� ������������ MHOOK",
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY ,
		0,
		0,
		"MHOOK",
		0,0,0
	};

	// ������ ������� ����� �����
	if(0==GetOpenFileName(&ofn))
	{
		return 1;
	}

	return 0;
}

///

int MHSettings::SaveMHookConfig(HWND hwnd)
{
	// ������� ������� ������
	OPENFILENAME ofn=
	{
		sizeof(OPENFILENAME),
		hwnd,
		NULL, // � ������ ���������� ������ ������������
		filter_MHOOK,
		NULL,
		0, // �� ���������� custom filter
		0, // -"-
		tfilename,
		256,
		tfiletitle,
		256,
		NULL,
		"��������� ���� ������������ MHOOK",
		OFN_OVERWRITEPROMPT,
		0,
		0,
		"MHOOK",
		0,0,0
	};

	// ������ ������� ����� �����
	if(0==GetSaveFileName(&ofn))
	{
		return 1;
	}

	return 0;
}