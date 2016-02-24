#ifndef __UIMANAGER_H__
#define __UIMANAGER_H__
#include <nnsys/g2d.h>

#define UIMANAGER_SCREEN_MAIN	0
#define UIMANAGER_SCREEN_SUB	1

typedef void (*PenFunc)(Menu* context, int x, int y);

class UISlice;

class UIManager
{
private:
	Menu* mContext;
	int mScreen;

	NNSFndList mSliceList;
	TPData mLastTouchState;

	PenFunc mOnPenDownFunc;
	PenFunc mOnPenMoveFunc;
	PenFunc mOnPenUpFunc;

	NNSG2dOamManagerInstance mOamManager;
public:
	UIManager(Menu* context, int screen);

	void AddSlice(UISlice* slice);
	void RegisterPenCallbacks(PenFunc onPenDown, PenFunc onPenMove, PenFunc onPenUp)
	{
		mOnPenDownFunc = onPenDown;
		mOnPenMoveFunc = onPenMove;
		mOnPenUpFunc = onPenUp;
	}

	void ProcessInput();
	void Render();
	void VBlankProc();
};

#endif