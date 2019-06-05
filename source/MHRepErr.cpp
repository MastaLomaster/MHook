/*
*	Сообщает об ошибках времени исполнения
*/

#include <windows.h>
#include <time.h>
#include <stdio.h>
#include "MHRepErr.h"

typedef __declspec(dllimport) const char* (__cdecl *type_tobiigaze_get_error_message)(int error_code);
extern type_tobiigaze_get_error_message fp_tobiigaze_get_error_message;

// Сообщает о СИСТЕМНЫХ ошибках времени исполнения
void MHReportError(TCHAR *SourceFile, TCHAR *FuncName, int LineNumber, HWND hwnd)
{
	DWORD res;				// Результат функции FormatMessage
	void *BKBStringError;	// Указатель на строку для получения системной ошибки
	TCHAR BKBMessage[1024];	// Это строка, в которой формируется сообщение об ошибке
	DWORD BKBLastError=GetLastError(); // Получили код системной ошибки
	
	
	if (BKBLastError!=(DWORD)0) // Получить строку, если код не равен нулю
	{
		res=FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
						NULL, BKBLastError,
						MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
						(LPTSTR) &BKBStringError, 0, NULL );

		if(res==(DWORD)0) BKBStringError=(void *)L"Сообщение об ошибке не найдено";
	}
	else
	{
		BKBStringError=(void *)L"Нет системной ошибки";
	}
	
	// Сформировать строку с полным описанием ошибки
	swprintf_s(BKBMessage, _countof(BKBMessage),
			L"Module: %s\r\nFunction: %s\r\nLine number: %d\r\nSysErr: %d (%s)",
			SourceFile, FuncName, LineNumber,
			BKBLastError, (TCHAR *)BKBStringError);


	//Освобождаем память, которую выделила функция FormatMessage
	if (BKBLastError!=(DWORD)0)
	{
			LocalFree( BKBStringError );
	}

	//Печатаем сообщение об ошибке (если возможно, на экран)
	MessageBox(hwnd,BKBMessage,L"MH: сообщение об ошибке",MB_OK|MB_ICONINFORMATION );

}

// Для НЕсистемных ошибок (перегружена)
// Реально не видел, где бы она использовалась
void MHReportError(TCHAR *Error, HWND hwnd) 
{
	//Печатаем сообщение об ошибке (если возможно, на экран)
	MessageBox(hwnd,Error,L"Непорядок!",MB_OK|MB_ICONINFORMATION );
}




//============================================================================================
// Для ошибок Tobii Gaze SDK (перегружена)
//============================================================================================
void MHReportError(int tbg_error_code, TCHAR *SourceFile, TCHAR *FuncName, int LineNumber, HWND hwnd)
{
	TCHAR ARCMessage[1024];	// Это строка, в которой формируется сообщение об ошибке
	TCHAR ConvertASCII2W[1024];

	if (tbg_error_code)
    {
		const TCHAR *tmp_char;
		if(fp_tobiigaze_get_error_message)
		{
			MultiByteToWideChar(CP_ACP, 0, (*fp_tobiigaze_get_error_message)(tbg_error_code), -1, ConvertASCII2W, 1023);
			tmp_char=ConvertASCII2W;
		}
		else tmp_char=L"неизвестно";

		// Сформировать строку с полным описанием ошибки
		swprintf_s(ARCMessage, _countof(ARCMessage),
			L"Module: %s\nFunction: %s\nLine number: %d\nОшибка Tobii Gaze SDK: %d (%s)",
			SourceFile, FuncName, LineNumber,
			tbg_error_code, 
			tmp_char);


		//Печатаем сообщение об ошибке (на экран)
		MessageBox(hwnd,ARCMessage,L"MHook:Gaze SDK: сообщение об ошибке",MB_OK|MB_ICONINFORMATION );
	}
}
