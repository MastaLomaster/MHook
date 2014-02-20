// Отслеживает вектор мыши
#ifndef __MH_MVECTOR
#define __MH_MVECTOR

class MHVector
{
public:
	static void Reset(){vector_position=-1;x=0;y=0;};
	static int NewValues(LONG dx, LONG dy);
protected:
	static int x,y,vector_position;
};

#endif