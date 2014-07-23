#include <Windows.h>
#include <process.h>
#include "MHRepErr.h"
#include "WM_USER_messages.h"
#include "CircleWindow.h"
#include "MagicWindow.h"

//extern HWND hwnds[2];
extern LONG screen_x, screen_y;
extern double screen_scale;

static volatile long TGD_is_processing=0; // Типа мьютекса для InterlockedCompareExchange

#define MH_EYETRACKER_BUFFER_SIZE 5
#define MH_EYETRACKER_MIN_REPEAT 3
static int eyetracker_buffer[MH_EYETRACKER_BUFFER_SIZE]={-1};
static int eposition=-1; // текущий номекр окна, на которое мы смотрим

//===========================================================================================================
// Перед началом работы заполнить буфер минус единицами
//===========================================================================================================
void ResetEytrackerBuffer()
{
	int i;
	for(i=0;i<MH_EYETRACKER_BUFFER_SIZE;i++)
		eyetracker_buffer[i]=-1;
	eposition=-1;
}

//===========================================================================================================
// Функция, которую вызывает REX, когда сообщает данные о глазах
//===========================================================================================================
void on_gaze_data(const tobiigaze_gaze_data* gazedata, void *user_data)
{
	int x,y,i;
	MagicWindow *mw;

	
	// Сбагриваем очередные данные, только если старые уже обработаны
	if(0==InterlockedCompareExchange(&TGD_is_processing,1,0))
	{
		// Переведём в координаты экрана
		x=screen_x/2.0*(gazedata->left.gaze_point_on_display_normalized.x+gazedata->right.gaze_point_on_display_normalized.x)-50/screen_scale;
		y=screen_y/2.0*(gazedata->left.gaze_point_on_display_normalized.y+gazedata->right.gaze_point_on_display_normalized.y)-50/screen_scale;

		// Двигаем окно с кружочком (если оно есть)
		if(CircleWindow::CircleHwnd) PostMessage(CircleWindow::CircleHwnd, WM_USER_MOVEWINDOW,x,y);
	
		if(MagicWindow::editmode)
		{
			TGD_is_processing=0;
			return; // В режиме редактирования только двигаем окно
			// Тут, конечно, возможно race condition, но будем считать, что неотжатая клавиша раз в сто лет рояля не играет
		}

		// Сдвигаем значения в буфере
		// Самое новое - на позиции 0
		for(i=MH_EYETRACKER_BUFFER_SIZE-1;i>0;i--)
		{
			eyetracker_buffer[i]=eyetracker_buffer[i-1];
		}
		eyetracker_buffer[0]=-1; // пока окно не найдено

		// Ищем, не попал ли взгляд в одно из окон, для того предназначенных
		for(i=0;i<NUM_MAGIC_WINDOWS;i++)
		{
			mw=&MagicWindow::magic_wnd[i];
			if((1==mw->mouse_or_eytracker)&&(mw->active)) // наш клиент
			{
				if((mw->x<=x)&&(mw->x+mw->width>=x)&&(mw->y<=y)&&(mw->y+mw->height>=y)) // попали!
				{
					eyetracker_buffer[0]=i; // В буфере запомнили наше окно
					break; // поиск завершён
				}
			}
		} // for

		// Поменялось ли текущее окно
		if(eposition!=eyetracker_buffer[0]) // кандидиат!
		{
			int num_repeat=1; // число повторений последнего окна в буфере
			for(i=1;i<MH_EYETRACKER_BUFFER_SIZE;i++)
			{
				if(eyetracker_buffer[i]==eyetracker_buffer[0]) num_repeat++;
			}
			if(num_repeat>=MH_EYETRACKER_MIN_REPEAT) // 3 из 5 последних были в окне, меняем текущую позицию
			{
				// 1. Если старая была окном - покидаем его.
				// Если это был не переключатель, то вызвать Press();
				if(eposition>=0)
				{
					if(MagicWindow::magic_wnd[eposition].button_or_switch==0) // кнопка
					{
						// Поубавляем прозрачность, если нажимаем, прозрачнеем, если отпускаем
						SetLayeredWindowAttributes(MagicWindow::magic_wnd[eposition].MWhwnd,NULL,255*50/100,LWA_ALPHA);
						MagicWindow::magic_wnd[eposition].Press();
					}
				}

				// 2. Делаем Press() новому окну, если оно >=0
				eposition=eyetracker_buffer[0]; // новый король
				if(eposition>=0)
				{
					mw=&MagicWindow::magic_wnd[eposition];
					// Далее содрано из оконной процедуры MagicWindow
					// Поубавляем прозрачность, если нажимаем, прозрачнеем, если отпускаем
					if(mw->pressed) SetLayeredWindowAttributes(mw->MWhwnd,NULL,255*50/100,LWA_ALPHA);
					else SetLayeredWindowAttributes(mw->MWhwnd,NULL,255*75/100,LWA_ALPHA);

					// При нажатии, если уже нажата другая кнопка в группе, отжать её (кроме самой себя)!
					if(!mw->pressed && (0!=mw->mw_group))
					{
						int i;
						for(i=0;i<NUM_MAGIC_WINDOWS;i++)
						{
							if((mw->button_index!=i)&&(MagicWindow::magic_wnd[i].mw_group==mw->mw_group)&&(MagicWindow::magic_wnd[i].pressed))
							{
								SetLayeredWindowAttributes(MagicWindow::magic_wnd[i].MWhwnd,NULL,255*50/100,LWA_ALPHA);
								MagicWindow::magic_wnd[i].Press();
							}
							// На active не проверяем, ибо только активная может быть pressed
						} // for i
					}

					mw->Press(); // будь то кнопка или переключатель - состояние меняется при входе всегда
				}

			} // 3 из 5 последних были в окне, меняем текущую позицию
		} // поменялось текущее окно

		// Типа мьютекса
		TGD_is_processing=0;
	}
	
}

