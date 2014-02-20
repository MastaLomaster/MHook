/*
*	Сообщает об ошибках времени исполнения
*/

#include <windows.h>
#include <time.h>
#include <stdio.h>
#include "MHRepErr.h"

// Сообщает о СИСТЕМНЫХ ошибках времени исполнения
void MHReportError(char *SourceFile, char *FuncName, int LineNumber)
{
	DWORD res;				// Результат функции FormatMessage
	void *BKBStringError;	// Указатель на строку для получения системной ошибки
	char BKBMessage[1024];	// Это строка, в которой формируется сообщение об ошибке
	DWORD BKBLastError=GetLastError(); // Получили код системной ошибки
	
	
	if (BKBLastError!=(DWORD)0) // Получить строку, если код не равен нулю
	{
		res=FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
						NULL, BKBLastError,
						MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
						(LPTSTR) &BKBStringError, 0, NULL );

		if(res==(DWORD)0) BKBStringError=(void *)"Сообщение об ошибке не найдено";
	}
	else
	{
		BKBStringError=(void *)"Нет системной ошибки";
	}
	
	// Сформировать строку с полным описанием ошибки
	sprintf_s(BKBMessage, sizeof(BKBMessage),
			"Module: %s\nFunction: %s\nLine number: %d\nSysErr: %d (%s)",
			SourceFile, FuncName, LineNumber,
			BKBLastError, (char *)BKBStringError);


	//Освобождаем память, которую выделила функция FormatMessage
	if (BKBLastError!=(DWORD)0)
	{
			LocalFree( BKBStringError );
	}

	//Печатаем сообщение об ошибке (если возможно, на экран)
	MessageBox(NULL,BKBMessage,"MH: сообщение об ошибке",MB_OK|MB_ICONINFORMATION );
/*
	//А также в файл 
	FILE *fout;
	fout=fopen("reperr.log","a");

	time_t mytime = time(0); // not 'long' 

	fprintf(fout,"****\n%s%s\n", ctime(&mytime), BKBMessage);
	fflush(fout);
	fclose(fout);
	*/
}

// Для НЕсистемных ошибок (перегружена)
// Реально не видел, где бы она использовалась
void MHReportError(char *Error, HWND hwnd) 
{
	//Печатаем сообщение об ошибке (если возможно, на экран)
	MessageBox(hwnd,Error,"Непорядок!",MB_OK|MB_ICONINFORMATION );
}
