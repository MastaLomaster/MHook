#include <Windows.h>
#include <process.h>
#include "MHRepErr.h"
#include "TobiiREX.h"

static uintptr_t tobii_thread_handler; // Хендлер потока для Gaze SDK

void on_gaze_data(toit_gaze_data* gazedata, void *user_data); // Определён в OnGazeData.cpp

volatile long BKBTobiiREX::initialized(0);


typedef struct to_api to_api;
typedef struct to_engine to_engine;
typedef struct to_device to_device;
typedef struct type_tobii_gaze_point
{
    int64_t timestamp_us;
    int validity;
    float position_xy[ 2 ];
} type_tobii_gaze_point;

typedef void( *type_tobii_device_url_receiver )( char const* url, void* user_data );
typedef void ( *type_tobii_gaze_point_callback )( type_tobii_gaze_point const* gaze_point, void* user_data );


typedef __declspec(dllimport) int (__cdecl *type_tobii_api_create)(to_api **api, void*, void* );
typedef __declspec(dllimport) int (__cdecl *type_tobii_enumerate_local_device_urls)(to_api* api, type_tobii_device_url_receiver receiver, void* user_data);
typedef __declspec(dllimport) int (__cdecl *type_tobii_device_create)(to_api* api, char const* url, to_device** device );
typedef __declspec(dllimport) int (__cdecl *type_tobii_gaze_point_subscribe)(to_device* device,  type_tobii_gaze_point_callback callback, void* user_data);
typedef __declspec(dllimport) int (__cdecl *type_tobii_gaze_point_unsubscribe)(to_device *device);
typedef __declspec(dllimport) int (__cdecl *type_tobii_device_destroy)(to_device* device);
typedef __declspec(dllimport) int (__cdecl *type_tobii_api_destroy)(to_api* api );
typedef __declspec(dllimport) int (__cdecl *type_tobii_wait_for_callbacks)(to_engine* engine, int device_count, to_device* const* devices );
typedef __declspec(dllimport) int (__cdecl *type_tobii_device_process_callbacks)(to_device* device);
typedef __declspec(dllimport) const char* (__cdecl *type_tobiigaze_get_error_message)(int error_code);

// указатели на фунуции из DLL
type_tobii_api_create fp_tobii_api_create;
type_tobii_enumerate_local_device_urls fp_tobii_enumerate_local_device_urls;
type_tobii_device_create fp_tobii_device_create;
type_tobii_gaze_point_subscribe fp_tobii_gaze_point_subscribe;
type_tobii_gaze_point_unsubscribe fp_tobii_gaze_point_unsubscribe;
type_tobii_device_destroy fp_tobii_device_destroy;
type_tobii_api_destroy fp_tobii_api_destroy;
type_tobii_wait_for_callbacks fp_tobii_wait_for_callbacks;
type_tobii_device_process_callbacks fp_tobii_device_process_callbacks;
type_tobiigaze_get_error_message fp_tobiigaze_get_error_message=0;

static to_api* api=0;
static to_device* device=0;
static char URL[256]= {0};
HMODULE TobiiStreamDLL=0;

//========================================================================
// Находит первое подключенное устройство
//========================================================================
static void url_receiver( char const* url, void* user_data )
{
    char* buffer = (char*)user_data;
    if( *buffer != '\0' ) return; // уже найдено, уходим

    if( strlen( url ) < 256 )
        strcpy_s(buffer, 255, url );
}

//=========================================================
// Callback function
//=========================================================
static void gaze_point_callback( type_tobii_gaze_point const* gaze_point, void* user_data )
{
	static toit_gaze_data gd;

	if( gaze_point->validity != 1 ) return;

	gd.toit_status = 1;
	gd.timestamp=gaze_point->timestamp_us; // Используется скроллом

	gd.left.bingo.x=gaze_point->position_xy[0];
	gd.left.bingo.y=gaze_point->position_xy[1];

	gd.right.bingo.x=gd.left.bingo.x;
	gd.right.bingo.y=gd.left.bingo.y;

	on_gaze_data(&gd, NULL);
}

//=====================================================================================
// Поток, читающий очередную порцию глазных координат
//=====================================================================================
static bool volatile flag_stop_thread=false;
unsigned __stdcall TobiiStreamThread(void *p)
{
	int err;

	while(!flag_stop_thread)
	{
		err = (*fp_tobii_wait_for_callbacks)( NULL, 1, &device );
		if(err!=0 && err !=6) //TOBII_ERROR_TIMED_OUT разрешается
		{
			MHReportError(err, __WIDEFILE__,L"tobii_wait_for_callbacks",__LINE__);
			return 0;
		}
		
        err= (*fp_tobii_device_process_callbacks)( device );
        if(err) 
		{
			MHReportError(err, __WIDEFILE__,L"tobii_device_process_callbacks",__LINE__);
			return 0;
		}
	}
	return 0;
}

//=====================================================================================
// Начало работы с устройством - версия 2 для Tobii Stream Engine
//=====================================================================================
int BKBTobiiREX::Init(HWND hwnd)
{
	int err;

	if(initialized) return 1; // уже инициализировали

	TobiiStreamDLL = LoadLibrary(L"tobii_stream_engine.dll");
	if(0==TobiiStreamDLL)
	{
		MHReportError(L"Не удалось загрузить библиотеку tobii_stream_engine.dll\r\nСкопируйте её в рабочий каталог программы",hwnd);
		return 1;
	}

	// 0. Загрузка функций из DLL
	// Пытаемся найти там нужные функции
	fp_tobii_api_create=(type_tobii_api_create)GetProcAddress(TobiiStreamDLL,"tobii_api_create");
	fp_tobii_enumerate_local_device_urls=(type_tobii_enumerate_local_device_urls)GetProcAddress(TobiiStreamDLL,"tobii_enumerate_local_device_urls"); 
	fp_tobii_device_create=(type_tobii_device_create)GetProcAddress(TobiiStreamDLL,"tobii_device_create");
	fp_tobii_gaze_point_subscribe=(type_tobii_gaze_point_subscribe)GetProcAddress(TobiiStreamDLL,"tobii_gaze_point_subscribe");
	fp_tobii_gaze_point_unsubscribe=(type_tobii_gaze_point_unsubscribe)GetProcAddress(TobiiStreamDLL,"tobii_gaze_point_unsubscribe");
	fp_tobii_device_destroy=(type_tobii_device_destroy)GetProcAddress(TobiiStreamDLL, "tobii_device_destroy");
	fp_tobii_api_destroy=(type_tobii_api_destroy)GetProcAddress(TobiiStreamDLL,"tobii_api_destroy");
	fp_tobii_wait_for_callbacks=(type_tobii_wait_for_callbacks)GetProcAddress(TobiiStreamDLL,"tobii_wait_for_callbacks");
	fp_tobii_device_process_callbacks=(type_tobii_device_process_callbacks)GetProcAddress(TobiiStreamDLL,"tobii_device_process_callbacks");

	// Трюк - старый указатель на новую функцию (чтобы не переделывать BKBRepError )
	fp_tobiigaze_get_error_message=(type_tobiigaze_get_error_message)GetProcAddress(TobiiStreamDLL,"tobii_error_message");



	if(
		!fp_tobii_api_create||!fp_tobii_enumerate_local_device_urls||
		!fp_tobii_device_create||!fp_tobii_gaze_point_subscribe||
		!fp_tobii_gaze_point_unsubscribe||!fp_tobii_device_destroy||
		!fp_tobii_api_destroy||!fp_tobii_wait_for_callbacks||
		!fp_tobii_device_process_callbacks||!fp_tobiigaze_get_error_message)
	{
		MHReportError(L"Не удалось получить необходимые функции из tobii_stream_engine.dll",hwnd);
		return 1;
	}

	// Содрано из Tobii Stream SDK
	err = (*fp_tobii_api_create)( &api, NULL, NULL );
    if(err)
	{
		MHReportError(err, __WIDEFILE__,L"tobii_api_create",__LINE__,hwnd);
		return 1;
	}
	
    err = (*fp_tobii_enumerate_local_device_urls)( api, url_receiver, URL );
	if(err)
	{
		MHReportError(err, __WIDEFILE__,L"tobii_api_create",__LINE__,hwnd);
		return 1;
	}

	if('\0' == *URL)
	{
		MHReportError(L"Не удалось подключиться к устройству Tobii",hwnd);
		return 1;
	}

    err = (*fp_tobii_device_create)( api, URL, &device );
    if(err)
	{
		MHReportError(err, __WIDEFILE__,L"tobii_device_create",__LINE__,hwnd);
		return 1;
	}

	
    err = (*fp_tobii_gaze_point_subscribe)( device, gaze_point_callback, 0 );
    if(err)
	{
		MHReportError(err, __WIDEFILE__,L"tobii_gaze_point_subscribe",__LINE__,hwnd);
		return 1;
	}

	// Здесь создаём поток для опроса айтрекера
	tobii_thread_handler=_beginthreadex(NULL,0,TobiiStreamThread,NULL,0,NULL);
	if(1>tobii_thread_handler)
	{
		MHReportError(__WIDEFILE__,L"start Tobii loop thread",__LINE__,hwnd);
		return 1;
	}

	initialized=true;
	return 0; // нормально отработали
}


//=====================================================================================
// Завершение работы с устройством - версия 2 для Tobii Stream Engine
//=====================================================================================
int BKBTobiiREX::Halt(HWND hwnd)
{
	int err;

	if(!initialized) return 1; // уже завершили работу

	// Первым делом остановим поток!
	flag_stop_thread=true;
	WaitForSingleObject((HANDLE)tobii_thread_handler,INFINITE);

	// Содрано из Tobii Stream SDK
	err = (*fp_tobii_gaze_point_unsubscribe)( device );
	if(err)
	{
		MHReportError(err, __WIDEFILE__,L"tobii_gaze_point_unsubscribe",__LINE__,hwnd);
		initialized=false; // На всякий случай
		return 1;
	}

	err = (*fp_tobii_device_destroy)( device );
	if(err)
	{
		MHReportError(err, __WIDEFILE__,L"tobii_device_destroy",__LINE__,hwnd);
		initialized=false; // На всякий случай
		return 1;
	}

	err = (*fp_tobii_api_destroy)( api );
	if(err)
	{
		MHReportError(err, __WIDEFILE__,L"tobii_api_destroy",__LINE__,hwnd);
		initialized=false; // На всякий случай
		return 1;
	}
   
	initialized=false;

	// Выгружаем DLL
	if(TobiiStreamDLL) FreeLibrary(TobiiStreamDLL);

	return 0; // нормально отработали
}
