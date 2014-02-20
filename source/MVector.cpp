#include <Windows.h>
#include <math.h>
#include "MVector.h"
#include "Settings.h"

extern LONG quad_x, quad_y; // Координаты квадратика в окне

#define MH_WINDOW_SIZE 200

static double PI = 3.14159265359;

int MHVector::x=0, MHVector::y=0, MHVector::vector_position=-1;

//===========================================================================================================
// Сюда сообщаем о новом движении мыши
// Если оно в направлении нажатой клавиши - игнорируем, сбрасываем
// Если оно в другом направлении - ждём, когда вектор вырастет до предела переключения
// Возвращает новый номер позиции 
// или -1, если позиция не изменилась
// или -2, если мышь подвинули недостаточно
// В переменные quad_dx и quad_y возвращаем вектор, который соответствует накопленному движению мыши (нормализованный)
//===========================================================================================================
int MHVector::NewValues(LONG dx, LONG dy)
{
	int angle, angle2, new_position;

	// 1. Ищем, в какую сторону повернута мышь
	// 1.1. Сначала добавляем к существующему вектору изменение
	x+=dx; y+=dy;
		
	// 1.2. Теперь смотрим, вырос ли вектор за предел лимита
	//double s=sqrt((double)(x*x+y*y));
	if(sqrt((double)(x*x+y*y))<(double)MHSettings::GetMouseSensitivity()) return -2; //мышь подвинули недостаточно

	// вернём нормализованный (до 100) вектор движения мыши
	quad_x=(LONG)(MH_WINDOW_SIZE/2*x/sqrt((double)(x*x+y*y)));
	quad_y=(LONG)(MH_WINDOW_SIZE/2*y/sqrt((double)(x*x+y*y)));
	

	// 1.3 Ищем угол наклона вектора
	angle=(int)(atan2((double)y,(double)x)*180.0/PI);
	// Вводим новую проверку! Остаток от предыдущего движения игнорируем, если он не попадает в тот же румб,
	// что и новое движение !!
	angle2=(int)(atan2((double)dy,(double)dx)*180.0/PI);
	if(angle2/45!=angle/45)
	{
		// обрезаем на фиг
		x=dx;y=dy;
		quad_x=(LONG)(MH_WINDOW_SIZE/2*x/sqrt((double)(x*x+y*y)));
		quad_y=(LONG)(MH_WINDOW_SIZE/2*y/sqrt((double)(x*x+y*y)));
		return -2;
	}

	// 1.4 Находим, какому румбу это соответствует
	if(4==MHSettings::GetNumPositions())
	{
		// Начало отсчёта будет -135 градусов и пойдёт по часовой стрелке (левая граница нулевой кнопки)
		angle=angle+135;
		if(angle<0) angle+=360;
		new_position=angle/90; // один из четырёх румбов
		if(new_position>3) vector_position=3; // Вдруг ошибки округления? Не стал много думать
	}
	else // восемь румбов
	{
		// Начало отсчёта будет -112 (вообщето, с половиной, но забьём) градусов и по часовой стрелке (левая граница нулевой кнопки)
		angle=angle+112;
		if(angle<0) angle+=360;
		new_position=angle/45; // один из восьми румбов
		if(new_position>7) vector_position=7; // Вдруг ошибки округления? Не стал много думать
	}

	//  Тут надо сбросить вектор
	// Сбрасываем вектор, независимо от того, сменили направление или нет
	x=0;y=0;

	// 1.5 А нажимаем ли на новую кнопку?
	if(vector_position==new_position) 	return -1;
	else
	{
		vector_position=new_position;
		return vector_position;
	}
}
