// Выдвет сообщение о (системных и несистемных) ошибках
#ifndef __MH_REPERR
#define __MH_REPERR

void MHReportError(char *SourceFile, char *FuncName, int LineNumber);
void MHReportError(char *Error, HWND hwnd=NULL); // Для НЕсистемных ошибок (перегружена)

#endif