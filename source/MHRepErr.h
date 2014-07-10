// Выдвет сообщение о (системных и несистемных) ошибках
#ifndef __MH_REPERR
#define __MH_REPERR

#include "WIDEFILE.h"

void MHReportError(TCHAR *SourceFile, TCHAR *FuncName, int LineNumber);
void MHReportError(TCHAR *Error, HWND hwnd=NULL); // Для НЕсистемных ошибок (перегружена)

#endif