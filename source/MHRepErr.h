// Выдвет сообщение о (системных и несистемных) ошибках
#ifndef __MH_REPERR
#define __MH_REPERR

#define WIDEN2(x) L ## x
#define WIDEN(x) WIDEN2(x)
#define __WIDEFILE__ WIDEN(__FILE__)

// Заголовочные файлы из Tobii Gaze SDK - избавились от них
//#include "tobiigaze_error_codes.h"
//#include "tobiigaze.h"

void MHReportError(TCHAR *SourceFile, TCHAR *FuncName, int LineNumber, HWND hwnd=NULL);
void MHReportError(TCHAR *Error, HWND hwnd=NULL); // Для НЕсистемных ошибок (перегружена)
//void MHReportError(tobiigaze_error_code tbg_error_code, TCHAR *SourceFile, TCHAR *FuncName, int LineNumber, HWND hwnd=NULL); // Для ошибок Tobii Gaze SDK (перегружена)
void MHReportError(int tbg_error_code, TCHAR *SourceFile, TCHAR *FuncName, int LineNumber, HWND hwnd=NULL); // Для ошибок Tobii Gaze SDK (перегружена)

#endif