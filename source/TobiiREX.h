// Взаимодействие с прибором Tobii REX
#ifndef __BKB_TOBIIREX
#define __BKB_TOBIIREX

class BKBTobiiREX
{
public:
	static int Init(HWND hwnd); // Инициализация работы с устройством
	static int Halt(HWND hwnd); // Завершение работы с устройством
protected:
	static volatile long initialized;
};

#endif