/*
*	�������� �� ������� ������� ����������
*/

#include <windows.h>
#include <time.h>
#include <stdio.h>
#include "MHRepErr.h"

// �������� � ��������� ������� ������� ����������
void MHReportError(char *SourceFile, char *FuncName, int LineNumber)
{
	DWORD res;				// ��������� ������� FormatMessage
	void *BKBStringError;	// ��������� �� ������ ��� ��������� ��������� ������
	char BKBMessage[1024];	// ��� ������, � ������� ����������� ��������� �� ������
	DWORD BKBLastError=GetLastError(); // �������� ��� ��������� ������
	
	
	if (BKBLastError!=(DWORD)0) // �������� ������, ���� ��� �� ����� ����
	{
		res=FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
						NULL, BKBLastError,
						MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
						(LPTSTR) &BKBStringError, 0, NULL );

		if(res==(DWORD)0) BKBStringError=(void *)"��������� �� ������ �� �������";
	}
	else
	{
		BKBStringError=(void *)"��� ��������� ������";
	}
	
	// ������������ ������ � ������ ��������� ������
	sprintf_s(BKBMessage, sizeof(BKBMessage),
			"Module: %s\nFunction: %s\nLine number: %d\nSysErr: %d (%s)",
			SourceFile, FuncName, LineNumber,
			BKBLastError, (char *)BKBStringError);


	//����������� ������, ������� �������� ������� FormatMessage
	if (BKBLastError!=(DWORD)0)
	{
			LocalFree( BKBStringError );
	}

	//�������� ��������� �� ������ (���� ��������, �� �����)
	MessageBox(NULL,BKBMessage,"MH: ��������� �� ������",MB_OK|MB_ICONINFORMATION );
/*
	//� ����� � ���� 
	FILE *fout;
	fout=fopen("reperr.log","a");

	time_t mytime = time(0); // not 'long' 

	fprintf(fout,"****\n%s%s\n", ctime(&mytime), BKBMessage);
	fflush(fout);
	fclose(fout);
	*/
}

// ��� ����������� ������ (�����������)
// ������� �� �����, ��� �� ��� ��������������
void MHReportError(char *Error, HWND hwnd) 
{
	//�������� ��������� �� ������ (���� ��������, �� �����)
	MessageBox(hwnd,Error,"���������!",MB_OK|MB_ICONINFORMATION );
}
