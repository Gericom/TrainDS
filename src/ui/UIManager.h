#ifndef __UIMANAGER_H__
#define __UIMANAGER_H__
#include <nnsys/g2d.h>

class Layout;
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
	NNSFndList mLayoutList;

	TPData mLastTouchState;

	PenFunc mOnPenDownFunc;
	PenFunc mOnPenMoveFunc;
	PenFunc mOnPenUpFunc;
	void* mCallbackArg;

	NNSG2dImageProxy* mImageProxy;
	NNSG2dImagePaletteProxy* mImagePaletteProxy;
	NNSG2dOamManagerInstance mOamManager;
	NNSG2dRendererInstance mOamRenderer;
	NNSG2dRenderSurface mOamRenderSurface;
	GXOamAttr mOamBuffer[128];

	UIManagerScreen mScreen;

	static BOOL CallBackAddOam(const GXOamAttr* pOam, u16 affineIndex, BOOL bDoubleAffine);
	static u16 CallBackAddAffine(const MtxFx22* mtx);
public:
	UIManager(UIManagerScreen screen, NNSG2dImageProxy* imageProxy, NNSG2dImagePaletteProxy* imagePaletteProxy);

	void AddLayout(Layout* layout);
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