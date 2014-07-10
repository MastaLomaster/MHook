﻿/*
*	Сообщает об ошибках времени исполнения
*/

#include <windows.h>
#include <time.h>
#include <stdio.h>
#include "MHRepErr.h"

// Сообщает о СИСТЕМНЫХ ошибках времени исполнения
void MHReportError(TCHAR *SourceFile, TCHAR *FuncName, int LineNumber)
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
	MessageBox(NULL,BKBMessage,L"MH: сообщение об ошибке",MB_OK|MB_ICONINFORMATION );

}

// Для НЕсистемных ошибок (перегружена)
// Реально не видел, где бы она использовалась
void MHReportError(TCHAR *Error, HWND hwnd) 
{
	//Печатаем сообщение об ошибке (если возможно, на экран)
	MessageBox(hwnd,Error,L"Непорядок!",MB_OK|MB_ICONINFORMATION );
}
