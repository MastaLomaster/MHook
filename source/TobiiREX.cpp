#include <Windows.h>
#include <process.h>
#include "MHRepErr.h"
#include "TobiiREX.h"

// Заголовочные файлы из Tobii Gaze SDK
#include "tobiigaze_error_codes.h"
#include "tobiigaze.h"
#include "tobiigaze_discovery.h"

// Для динамической подгрузки библиотек
typedef  TOBIIGAZE_API void (TOBIIGAZE_CALL *type_tobiigaze_get_connected_eye_tracker)(char *url, uint32_t url_size, tobiigaze_error_code *error_code);
typedef  TOBIIGAZE_API void (TOBIIGAZE_CALL *type_tobiigaze_run_event_loop_on_internal_thread)(tobiigaze_eye_tracker *eye_tracker, tobiigaze_async_callback callback, void *user_data);

typedef TOBIIGAZE_API tobiigaze_eye_tracker* (TOBIIGAZE_CALL *type_tobiigaze_create)(const char *url, tobiigaze_error_code *error_code);
typedef TOBIIGAZE_API void (TOBIIGAZE_CALL *type_tobiigaze_connect)(tobiigaze_eye_tracker *eye_tracker, tobiigaze_error_code *error_code);
typedef TOBIIGAZE_API void (TOBIIGAZE_CALL *type_tobiigaze_start_tracking)(tobiigaze_eye_tracker *eye_tracker, tobiigaze_gaze_listener gaze_callback, tobiigaze_error_code *error_code, void *user_data);


typedef TOBIIGAZE_API void (TOBIIGAZE_CALL *type_tobiigaze_stop_tracking)(tobiigaze_eye_tracker *eye_tracker, tobiigaze_error_code *error_code);
typedef TOBIIGAZE_API void (TOBIIGAZE_CALL *type_tobiigaze_disconnect)(tobiigaze_eye_tracker *eye_tracker);
typedef TOBIIGAZE_API void (TOBIIGAZE_CALL *type_tobiigaze_break_event_loop)(tobiigaze_eye_tracker *eye_tracker);
typedef TOBIIGAZE_API void (TOBIIGAZE_CALL *type_tobiigaze_destroy)(tobiigaze_eye_tracker *eye_tracker);

typedef TOBIIGAZE_API const char* (TOBIIGAZE_CALL *type_tobiigaze_get_error_message)(tobiigaze_error_code error_code);

typedef TOBIIGAZE_API void (TOBIIGAZE_CALL *type_tobiigaze_run_event_loop)(tobiigaze_eye_tracker *eye_tracker, tobiigaze_error_code *error_code);

HMODULE TobiiConfigDLL=0, TobiiCoreDLL=0;

// указатели на фунуции из DLL
type_tobiigaze_get_connected_eye_tracker fp_tobiigaze_get_connected_eye_tracker;
type_tobiigaze_run_event_loop_on_internal_thread fp_tobiigaze_run_event_loop_on_internal_thread;

type_tobiigaze_create fp_tobiigaze_create;
type_tobiigaze_connect fp_tobiigaze_connect;
type_tobiigaze_start_tracking fp_tobiigaze_start_tracking;
type_tobiigaze_stop_tracking fp_tobiigaze_stop_tracking;
type_tobiigaze_disconnect fp_tobiigaze_disconnect;
type_tobiigaze_break_event_loop fp_tobiigaze_break_event_loop;
type_tobiigaze_destroy fp_tobiigaze_destroy;
type_tobiigaze_get_error_message fp_tobiigaze_get_error_message=0;
type_tobiigaze_run_event_loop fp_tobiigaze_run_event_loop;

// Всякие переменные для работы с Tobii Gaze SDK видны только локально (static)
static tobiigaze_error_code tbg_error_code;
static char url[256];
static tobiigaze_eye_tracker* eye_tracker=0;

static uintptr_t tobii_thread_handler; // Хендлер потока для Gaze SDK

void on_gaze_data(const tobiigaze_gaze_data* gazedata, void *user_data); // Определён в OnGazeData.cpp

volatile long BKBTobiiREX::initialized(0);

//===========================================================================================================
// Заглушка для Gaze SDK 4, там добавился новый параметр, мы его игнорируем
//===========================================================================================================
void on_gaze_data_SDK4(const struct tobiigaze_gaze_data *gaze_data, const struct tobiigaze_gaze_data_extensions *gaze_data_extensions, void *user_data)
{
	on_gaze_data(gaze_data, user_data);
}


//=====================================================================================
// Начало работы с устройством
//=====================================================================================
int BKBTobiiREX::Init(HWND hwnd)
{
	if(initialized) return 1; // уже инициализировали

	TobiiCoreDLL = LoadLibrary(L"TobiiGazeCore32.dll");
	if(0==TobiiCoreDLL)
	{
		MHReportError(L"Не удалось загрузить библиотеку TobiiGazeCore32.dll\r\nСкопируйте её в рабочий каталог программы",hwnd);
		return 1;
	}


	// 0. Загрузка функций из DLL
	// Пытаемся найти там нужные функции
	fp_tobiigaze_get_connected_eye_tracker=(type_tobiigaze_get_connected_eye_tracker)GetProcAddress(TobiiCoreDLL,"tobiigaze_get_connected_eye_tracker");
	fp_tobiigaze_run_event_loop_on_internal_thread=(type_tobiigaze_run_event_loop_on_internal_thread)GetProcAddress(TobiiCoreDLL,"tobiigaze_run_event_loop_on_internal_thread");

	fp_tobiigaze_create=(type_tobiigaze_create)GetProcAddress(TobiiCoreDLL,"tobiigaze_create");
	fp_tobiigaze_connect=(type_tobiigaze_connect)GetProcAddress(TobiiCoreDLL,"tobiigaze_connect");
	fp_tobiigaze_start_tracking=(type_tobiigaze_start_tracking)GetProcAddress(TobiiCoreDLL,"tobiigaze_start_tracking");
	fp_tobiigaze_stop_tracking=(type_tobiigaze_stop_tracking)GetProcAddress(TobiiCoreDLL,"tobiigaze_stop_tracking");
	fp_tobiigaze_disconnect=(type_tobiigaze_disconnect)GetProcAddress(TobiiCoreDLL,"tobiigaze_disconnect");
	fp_tobiigaze_break_event_loop=(type_tobiigaze_break_event_loop)GetProcAddress(TobiiCoreDLL,"tobiigaze_break_event_loop");
	fp_tobiigaze_destroy=(type_tobiigaze_destroy)GetProcAddress(TobiiCoreDLL,"tobiigaze_destroy");
	fp_tobiigaze_get_error_message=(type_tobiigaze_get_error_message)GetProcAddress(TobiiCoreDLL,"tobiigaze_get_error_message");
	fp_tobiigaze_run_event_loop=(type_tobiigaze_run_event_loop)GetProcAddress(TobiiCoreDLL,"tobiigaze_run_event_loop");

	if(!fp_tobiigaze_get_connected_eye_tracker||!fp_tobiigaze_run_event_loop_on_internal_thread||!fp_tobiigaze_create||
		!fp_tobiigaze_connect||!fp_tobiigaze_start_tracking||!fp_tobiigaze_stop_tracking||
		!fp_tobiigaze_disconnect||!fp_tobiigaze_break_event_loop||!fp_tobiigaze_destroy||
		!fp_tobiigaze_get_error_message||!fp_tobiigaze_run_event_loop)
	{
		MHReportError(L"Не удалось получить необходимые функции из TobiiGazeCore32.dll",hwnd);
		return 1;
	}

	// Изменение в Gaze SDK 4.0
	// 1.1.
	(*fp_tobiigaze_get_connected_eye_tracker)(url, 255, &tbg_error_code);
	if(tbg_error_code)
	{
		MHReportError(tbg_error_code, __WIDEFILE__,L"tobiigaze_get_connected_eye_tracker",__LINE__,hwnd);
		return 1;
	}
	
	// 1.2.
	eye_tracker = (*fp_tobiigaze_create)(url, &tbg_error_code);
    if(tbg_error_code)
	{
		MHReportError(tbg_error_code, __WIDEFILE__,L"tobiigaze_create",__LINE__,hwnd);
		return 1;
	}

	// 1.4. - содрано из Gaze SDK 4.0
	(*fp_tobiigaze_run_event_loop_on_internal_thread)(eye_tracker, 0, 0);

	// 1.5.
	(*fp_tobiigaze_connect)(eye_tracker, &tbg_error_code);
    if(tbg_error_code)
	{
		MHReportError(tbg_error_code, __WIDEFILE__,L"tobiigaze_connect",__LINE__,hwnd);
		return 1;
	}

	// В примере из SDK рекомендуют тут посмотреть детали устройства. Оставим на потом.
    // print_device_info(eye_tracker)

	// 1.6.
    (*fp_tobiigaze_start_tracking)(eye_tracker, &on_gaze_data_SDK4, &tbg_error_code, 0);
	if(tbg_error_code)
	{
		MHReportError(tbg_error_code, __WIDEFILE__,L"tobiigaze_start_tracking",__LINE__,hwnd);
		return 1;
	}

	initialized=1;
	return 0; // нормально отработали
}

//=====================================================================================
// Завершение работы с устройством
//=====================================================================================
int BKBTobiiREX::Halt(HWND hwnd)
{
	// Чтобы два окна не начали останавливать вместе
	if(0==InterlockedCompareExchange(&initialized,0,1))
	 return 1; // уже завершили работу

	// Содрано из Tobii Gaze SDK
	(*fp_tobiigaze_stop_tracking)(eye_tracker, &tbg_error_code);
	if(tbg_error_code)
	{
		MHReportError(tbg_error_code, __WIDEFILE__,L"tobiigaze_stop_tracking",__LINE__,hwnd);
		return 1;
	}

    (*fp_tobiigaze_disconnect)(eye_tracker);
   
    (*fp_tobiigaze_break_event_loop)(eye_tracker);
   
    (*fp_tobiigaze_destroy)(eye_tracker);

	

	// Выгружаем DLL
	if(TobiiConfigDLL) FreeLibrary(TobiiConfigDLL);
	if(TobiiCoreDLL) FreeLibrary(TobiiCoreDLL);

	return 0; // нормально отработали
}


