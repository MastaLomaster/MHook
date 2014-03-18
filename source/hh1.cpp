#include <Windows.h>
#include "hh1.h"
#include "MVector.h"
#include "MHKeypad.h"
#include "Settings.h"

#ifdef _DEBUG
//#include <stdio.h>
	//char debug_buf[4096];
#endif

extern HWND		MHhwnd;

extern LONG screen_x, screen_y;

static int alt2_offset=0; // Смещение для альтернативных кодировок
static int position_mem_opposite=0; // Раньше путались позиции от таймера и от противоположного направления. Теперь это разные переменные. 


//=====================================================================================================
// Функция, которая определяет, является ли новое направление противоположным к старому
// Для 8 направлений считаем, что противоположными являются три направления >90 градусов от старого
//=====================================================================================================
static bool IsOpposite(int old_direction, int new_direction)
{
	int num_directions=MHSettings::GetNumPositions();
	int opposite8;

	if((-1==old_direction)||(-1==new_direction)) return false;

	if(4==num_directions)
	{
		if((old_direction+2)%4==new_direction) return true;
	}
	else // 8 позиций
	{
		opposite8=(old_direction+4)%8;

		if(new_direction==opposite8) return true;

		if(0==opposite8) // Спец место, где семёрка переходит в 0
		{
			if((7==new_direction)||(1==new_direction)) return true;
		}
		else
		{
			if((new_direction==opposite8-1)||(new_direction==opposite8+1)) return true;
		}
	}

	return false;
}


void MHookHandler1::OnMouseMove(LONG _x, LONG _y)
{
	int position;
		
	// При нажатой правой кнопке мыши не передаём её движения в MHVector,
	// НО! продолжаем отслеживать last_x и last_y, не сбрасывая initialized! 
	// теперь по-новому: если есть прилипание к оси, то можно только противоположное
	// или то же. Поворачивать нельзя.
	if(initialized)
	{
		dx=(_x-last_x);
		dy=(_y-last_y);

		// Может, пятую кнопку можно нажать?
		if(MHSettings::flag_enable_speed_button) OnFastMove(dx,dy);

		position=MHVector::NewValues(dx,dy);

		// Если вбок и вниз = просто вбок, меняем позиции 3 и 5 на 4
		if((8==MHSettings::GetNumPositions())&&((3==position)||(5==position)))
			position=4;

		if(!rbutton_pressed) // известно последнее положение мыши, правая кнопка не нажата
		{
			// Новая опция - из конца в конец в два движения (только при 0==alt_offset)
			if((MHSettings::flag_2moves_mode1)&&(0==alt2_offset))
			{
				if(0<=position) // новое направление
				{
					// Это противоположное направление?
					if(IsOpposite(position_mem_opposite,position))
					{
						// 1. (при невыставленном флаге противоположного направления)
						// сбросим нажатую клавишу. Нажать противоположную сможем только после таймаута
						if(false==flag_opposite_direction)
						{
							// запомним время, отпустим кнопки, запомним направление (ниже)
							opposite_time=timeGetTime();
							MHKeypad::Reset(alt2_offset);
#ifdef _DEBUG
		OutputDebugString("OppDir keyup");
#endif
							flag_opposite_direction=true;
							position_mem_opposite=position;
							 
							// Почему-то Reset не включает перерисовку
							InvalidateRect(MHhwnd,NULL,TRUE);
						}
						else // при выставленном флаге противоположного направления
						{
							// 2. Как такое случилось?
							MHKeypad::Press(position,true,alt2_offset);
							flag_opposite_direction=false;
							position_mem_opposite=position;
						}
					}
					else // не противоположное (position_mem содержит всё что угодно),
					{
						if(true==flag_opposite_direction) 
						{
							// 3.
							// ждём выхода в сторону position_mem, а по дороге завернули в сторону
							// только здесь возможен поворот в сторону !!!
							// Но только если прошло время обездвиженности!!!
							if(timeGetTime()-opposite_time>100) // была пауза, можно идти в противоположном направлении
							{
								MHKeypad::Press(position,true, alt2_offset);
								flag_opposite_direction=false;
								position_mem_opposite=position;
							}
							else 
							{
								MHVector::Reset(); // Не надо больше слать -1
								opposite_time=timeGetTime(); // паузы в 50 мс неподвижности не было, перевзводим
							}
							//MHKeypad::Press(position,true);
							//flag_opposite_direction=false; // уход в сторону - сброс ожиданий
						}
						else // flag_opposite_direction=false
						{
							// не ждём выхода в сторону position_mem. 
							// Нажимаем, только если -1==position_mem (при прилипании)
							if((-1==position_mem)||(MHSettings::flag_change_direction_ontheway))
							{
								MHKeypad::Press(position,true, alt2_offset);
								position_mem_opposite=position;
							}
						}
					
					} // не противоположное направление
				}
				else if(-1==position)
				{
					// Обрабатываем, только если не довели до конца
					if(flag_opposite_direction)
					{
						if(timeGetTime()-opposite_time>100) // была пауза, можно идти в противоположном направлении
						{
							MHKeypad::Press(position_mem_opposite,true, alt2_offset);
							flag_opposite_direction=false;
						}
						else opposite_time=timeGetTime(); // паузы в 50 мс неподвижности не было, перевзводим
					}
				}
				// else осталось только -2, игнорируем
			} //flag_2moves_mode1 и 4 позиции
			else 
			{
				if(0==alt2_offset)
				{
					// Почти по-старому, как было до модификации flag_2moves_mode1
					if(0<=position) // -2=мышь ваще не двигалась, -1= направление не изменилось
					{
						MHKeypad::Press(position,true,alt2_offset);
					}
				}
				else // Если это альтернативная раскладка, то взвести таймер
				{
					// как раньше при обработке правой кнопки мыши
					if(0<=position) // -2=мышь подвинулась на недостаточное растояние, -1= направление не изменилось
					{
						MHKeypad::Press(position,true, alt2_offset); // По движению правой кнопки нажимать альтернативные клавиши из первой раскладки
						position_mem=position;
						
					}
					// Таймер взводим заново при любом движении мыши, если было хоть что-то нажато ранее
					// то есть -1!=position_mem
					if(-1!=position_mem)
					{
						last_time=timeGetTime();
						SetTimer(MHhwnd,1,MHSettings::timeout_after_move,NULL);

					}
				}
			}
		} // правая кнопка не нажата
		else // нажата правая кнопка. Внимание!!!! Здесь может быть 8 позиций, тогда движение с правой кнопкой игнорируем !!!!
		{
			if(4==MHSettings::GetNumPositions())
			{
				if(!MHSettings::flag_alt2) // Так было, пока не ввели вторую альтернативную: движения с нажатой правой вызывали нажатия c таймером
				{
					// обработка правой кнопки мыши
					if(0<=position) // -2=мышь подвинулась на недостаточное растояние, -1= направление не изменилось
					{
						//MHKeypad::Press(position,true, 6); // По движению правой кнопки нажимать альтернативные клавиши из первой раскладки
						MHKeypad::Press(position,true, alt2_offset);
						position_mem=position;
					}
					// Таймер взводим заново при любом движении мыши, если было хоть что-то нажато ранее
					// то есть -1!=position_mem
					if(-1!=position_mem)
					{
						last_time=timeGetTime();
						SetTimer(MHhwnd,1,MHSettings::timeout_after_move,NULL);
					}
				}
				else // flag_alt2 - теперь движения с нажатой правой - это выбор раскладки
				{
					switch(position)
					{
						
					case 1: // стрелка впрваво - первая альтернативная
						// Первым делом - отпустить нажатые клавиши
						MHKeypad::Reset(alt2_offset);
						position_mem=-1;
						switch(alt2_offset)
						{
						case 6: // Включена уже, выключить
							alt2_offset=0;
							break;

						case 0: // основная, поменять
						case 11: // Включена вторая, поменять
							alt2_offset=6;
							break;
						}
						break;

					case 3: // стрелка влево - выбор второй альтернативной
						// Первым делом - отпустить нажатые клавиши
						MHKeypad::Reset(alt2_offset);
						position_mem=-1;
						switch(alt2_offset)
						{
						case 11: // Включена уже, выключить
							alt2_offset=0;
							break;

						case 0: // основная, поменять
						case 6: // Включена вторая, поменять
							alt2_offset=11;
							break;
						}
						break;
						// Остальные направления (стрелки вверх и вниз) игнорируем
					}
				} // выставлен флаг alt2, меняем раскладки
			} // 4 позиции, а в 8 позициях с правой кнопкой ничего не делаем вообще
			
		} // правая кнопка нажата
	} // if initialized
		

	if(!initialized) initialized=true;

	last_x=_x;
	last_y=_y;
	if(last_x<0) last_x=0;
	if(last_y<0) last_y=0;
	if(last_x>=screen_x) last_x=screen_x-1;
	if(last_y>=screen_y) last_y=screen_y-1;

#ifdef _DEBUG
	// Покажите абсолютные значения мвшиных сообщений
	//sprintf(debug_buf,"dx: %d  dy: %d\n", dx,dy);
	//OutputDebugString(debug_buf);
#endif
}

bool MHookHandler1::OnRDown()
{
	rbutton_pressed=true;
#ifdef _DEBUG
		OutputDebugString("OnRDown keyup");
#endif
	MHKeypad::Reset(alt2_offset); // Отпускаем нажатые кнопки
	position_mem=-1;
	position_mem_opposite=-1;
	// Начинаем новый отсчет движений
	MHVector::Reset();

	// Почему-то Reset не включает перерисовку
	InvalidateRect(MHhwnd,NULL,TRUE);

	if(!MHSettings::flag_alt2) // Так было, пока не ввели вторую альтернативную
	{
		alt2_offset=6;
	}

	return true; // подавляйте правый клик
}

bool MHookHandler1::OnRUp()
{
	rbutton_pressed=false;
#ifdef _DEBUG
		OutputDebugString("OnRUP keyup");
#endif
	MHKeypad::Reset(alt2_offset); // Отпускаем нажатые кнопки (в альтернативных кодах клавиш, смещение 6)
	position_mem=-1;
	position_mem_opposite=-1;
	// Начинаем новый отсчет движений
	MHVector::Reset();
	
	if(!MHSettings::flag_alt2) // Так было, пока не ввели вторую альтернативную
	{
		alt2_offset=0;
	}
	flag_opposite_direction=false;

	// Почему-то Reset не включает перерисовку
	InvalidateRect(MHhwnd,NULL,TRUE);
	
	return true; // подавляйте правый клик
}

extern LONG quad_x,quad_y; // Координаты квадратика в окне

void MHookHandler1::OnDraw(HDC hdc, LONG window_size)
{
	RECT rect;

			// Квадратик с мышью
			//xpercent/100.0f*xsize
			//ypercent/100.0f*ysize
			rect.left=(LONG)(window_size/2+quad_x-10);
			rect.top=(LONG)(window_size/2+quad_y-10);
			rect.right=rect.left+20;
			rect.bottom=rect.top+20;
			FillRect(hdc,&rect,(HBRUSH)GetStockObject(GRAY_BRUSH));


			// В режиме двух альтернативных кодировок рисуем символы
			if(MHSettings::flag_alt2)
			{
				switch(alt2_offset)
				{
				case 0:
					TextOut(hdc,90,90,"0",1);
					break;

				case 11:
					TextOut(hdc,90,90,"<",1);
					break;

				case 6:
					TextOut(hdc,90,90,">",1);
					break;
				}
			}
}

void MHookHandler1::OnTimer()
{
	if(-1==position_mem)
	{
		KillTimer(MHhwnd,1);
		return; // Клавиша не нажата, делать нечего
	}

	// Проверяем, а не рудимент ли это, оставшийся в очереди сообщений?
	// Для этого проверим, действительнор ли истекло нужное время
	DWORD time_now=timeGetTime();
	if(time_now-last_time<MHSettings::timeout_after_move) return;

	// Действительно, время отпустить клавишу!
	KillTimer(MHhwnd,1); // Чтобы в отладчике не получать сообщения при обработке Press
#ifdef _DEBUG
		OutputDebugString("Timer keyup");
#endif
	MHKeypad::Press(position_mem,false,alt2_offset); // По движению правой кнопки нажимать альтернативные клавиши
	MHVector::Reset(); // Вот это обязательно, иначе в том же направлении мышь не нажмёт клавишу
	// Почему-то Reset не включает перерисовку
	InvalidateRect(MHhwnd,NULL,TRUE);

	position_mem=-1;
	
}

void MHookHandler1::Halt()
{
	KillTimer(MHhwnd,1);

	// отожмём клавиши
	//if(
	//if((rbutton_pressed)&&(!MHSettings::flag_alt2)) 
	MHKeypad::Reset(alt2_offset);
		
	alt2_offset=0;

	// персонально для нас (в отличие от HaltGeneral базового класса):
	flag_opposite_direction=false;
	position_mem=-1;
	position_mem_opposite=-1;
}