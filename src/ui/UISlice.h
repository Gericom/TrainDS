#ifndef __UISLICE_H__
#define __UISLICE_H__
#include <nnsys/fnd.h>

class UIManager;

class UISlice
{
	friend class UIManager;
private:
	NNSFndLink mLink;
protected:
	int mX;
	int mY;
public:
	UISlice() 
		: mX(0), mY(0) 
	{ 
	
	}
	//Sets the position of the slice. Not all slices use this as their absolute position,
	//because sometimes this is only used for an animation
	void SetPosition(int x, int y)
	{
		mX = x;
		mY = y;
	}

	virtual BOOL OnPenDown(Menu* context, int x, int y) = 0;
	virtual BOOL OnPenMove(Menu* context, int x, int y) = 0;
	virtual BOOL OnPenUp(Menu* context, int x, int y) = 0;

	virtual void Render(Menu* context, NNSG2dOamManagerInstance* oamManager) = 0;
};

#endif