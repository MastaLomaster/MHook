// Взаимодействие c TheEyeTribe 
#ifndef __BKB_TET
#define __BKB_TET

class BKBTET
{
public:
	static int Init(HWND hwnd); // Инициализация работы с устройством
	static int Halt(HWND hwnd); // Завершение работы с устройством
protected:
	static bool initialized;
};

#endif