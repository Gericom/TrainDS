#ifndef __UIMANAGER_H__
#define __UIMANAGER_H__
#include <nnsys/g2d.h>

typedef void(*PenFunc)(Menu* context, int x, int y);

class UISlice;

class UIManager
{
private:
	Menu* mContext;

	NNSFndList mSliceList;
	TPData mLastTouchState;

	PenFunc mOnPenDownFunc;
	PenFunc mOnPenMoveFunc;
	PenFunc mOnPenUpFunc;
public:
	UIManager(Menu* context);

	void AddSlice(UISlice* slice);
	void RegisterPenCallbacks(PenFunc onPenDown, PenFunc onPenMove, PenFunc onPenUp)
	{
		mOnPenDownFunc = onPenDown;
		mOnPenMoveFunc = onPenMove;
		mOnPenUpFunc = onPenUp;
	}

	void ProcessInput();
	void Render();
};

#endif