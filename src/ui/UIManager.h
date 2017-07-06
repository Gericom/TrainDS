#ifndef __UIMANAGER_H__
#define __UIMANAGER_H__
#include <nnsys/g2d.h>

typedef void(*PenFunc)(void* arg, int x, int y);

class UISlice;
class UIComponent;

class UIManager
{
public:
	enum UIManagerScreen
	{
		UI_MANAGER_SCREEN_MAIN,
		UI_MANAGER_SCREEN_SUB
	};

private:
	NNSFndList mUIComponentList;

	TPData mLastTouchState;

	PenFunc mOnPenDownFunc;
	PenFunc mOnPenMoveFunc;
	PenFunc mOnPenUpFunc;
	void* mCallbackArg;

	NNSG2dOamManagerInstance mOamManager;
	GXOamAttr mOamBuffer[128];

	UIManagerScreen mScreen;
public:
	UIManager(UIManagerScreen screen);

	void AddUIComponent(UIComponent* uiComponent);
	void RegisterPenCallbacks(PenFunc onPenDown, PenFunc onPenMove, PenFunc onPenUp, void* arg)
	{
		mOnPenDownFunc = onPenDown;
		mOnPenMoveFunc = onPenMove;
		mOnPenUpFunc = onPenUp;
		mCallbackArg = arg;
	}

	void ProcessInput();
	void Render();
	void ProcessVBlank();
};

#endif