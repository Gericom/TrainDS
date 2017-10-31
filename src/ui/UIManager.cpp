#include <nitro.h>
#include <nnsys/g2d.h>
#include <nnsys/fnd.h>
#include "core.h"
#include "menu\Menu.h"
#include "components/UIComponent.h"
#include "layoutengine/Layout.h"
#include "UIManager.h"

UIManager::UIManager(UIManagerScreen screen, NNSG2dImageProxy* imageProxy, NNSG2dImagePaletteProxy* imagePaletteProxy)
	: mScreen(screen), mOnPenDownFunc(NULL), mOnPenMoveFunc(NULL), mOnPenUpFunc(NULL), mImageProxy(imageProxy), mImagePaletteProxy(imagePaletteProxy)
{
	NNS_FND_INIT_LIST(&mLayoutList, Layout, mLink);
	MI_CpuClear8(&mLastTouchState, sizeof(mLastTouchState));
	NNS_G2dGetNewOamManagerInstanceAsFastTransferMode(&mOamManager, 0, 128, mScreen == UI_MANAGER_SCREEN_MAIN ? NNS_G2D_OAMTYPE_MAIN : NNS_G2D_OAMTYPE_SUB);
	NNS_G2dInitRenderer(&mOamRenderer);
	NNS_G2dInitRenderSurface(&mOamRenderSurface);

	mOamRenderSurface.viewRect.posTopLeft.x = 0;
	mOamRenderSurface.viewRect.posTopLeft.y = 0;
	mOamRenderSurface.viewRect.sizeView.x = 256 * FX32_ONE;
	mOamRenderSurface.viewRect.sizeView.y = 192 * FX32_ONE;
	mOamRenderSurface.type = mScreen == UI_MANAGER_SCREEN_MAIN ? NNS_G2D_SURFACETYPE_MAIN2D : NNS_G2D_SURFACETYPE_SUB2D;

	mOamRenderSurface.pFuncOamRegister = CallBackAddOam;
	mOamRenderSurface.pFuncOamAffineRegister = CallBackAddAffine;

	NNS_G2dAddRendererTargetSurface(&mOamRenderer, &mOamRenderSurface);
	NNS_G2dSetRendererImageProxy(&mOamRenderer, mImageProxy, mImagePaletteProxy);
}

void UIManager::AddLayout(Layout* layout)
{
	NNS_FndAppendListObject(&mLayoutList, layout);
}

void UIManager::ProcessInput()
{
	TPData raw_point;// = gTPData;
	Core_GetTouchInput(&raw_point);
	TPData disp_point;
	//while (TP_RequestRawSampling(&raw_point) != 0);
	TP_GetCalibratedPoint(&disp_point, &raw_point);
	if (raw_point.validity && !mLastTouchState.touch && disp_point.touch)
		return;//this will yield invalid results!
	int x = disp_point.x;
	int y = disp_point.y;
	if (raw_point.validity)
	{
		x = mLastTouchState.x;
		y = mLastTouchState.y;
	}
	if (!mLastTouchState.touch && disp_point.touch)//PenDown
	{
		Layout* layout = NULL;
		while ((layout = (Layout*)NNS_FndGetNextListObject(&mLayoutList, layout)) != NULL)
			if (layout->OnPenDown(x * FX32_ONE, y * FX32_ONE))
				break;
		if (layout == NULL && mOnPenDownFunc != NULL) //no layout has done anything, let's pass the event through
			mOnPenDownFunc(mCallbackArg, x, y);
	}
	else if (mLastTouchState.touch && disp_point.touch && (mLastTouchState.x != x || mLastTouchState.y != y))//PenMove
	{
		Layout* layout = NULL;
		while ((layout = (Layout*)NNS_FndGetNextListObject(&mLayoutList, layout)) != NULL)
			if (layout->OnPenMove(x * FX32_ONE, y * FX32_ONE))
				break;
		if (layout == NULL && mOnPenMoveFunc != NULL) //no layout has done anything, let's pass the event through
			mOnPenMoveFunc(mCallbackArg, x, y);
	}
	else if (mLastTouchState.touch && !disp_point.touch)//PenUp
	{
		Layout* layout = NULL;
		while ((layout = (Layout*)NNS_FndGetNextListObject(&mLayoutList, layout)) != NULL)
			if (layout->OnPenUp(x * FX32_ONE, y * FX32_ONE))
				break;
		if (layout == NULL && mOnPenUpFunc != NULL) //no layout has done anything, let's pass the event through
			mOnPenUpFunc(mCallbackArg, x, y);
	}
	if (!raw_point.validity)
		mLastTouchState = disp_point;
	else
		mLastTouchState.touch = disp_point.touch;
}

static UIManager* sRenderingUIManager;

BOOL UIManager::CallBackAddOam(const GXOamAttr* pOam, u16 affineIndex, BOOL bDoubleAffine)
{
#pragma unused( bDoubleAffine )
	return NNS_G2dEntryOamManagerOamWithAffineIdx(&sRenderingUIManager->mOamManager, pOam, affineIndex);
}

u16 UIManager::CallBackAddAffine(const MtxFx22* mtx)
{
	return NNS_G2dEntryOamManagerAffine(&sRenderingUIManager->mOamManager, mtx);
}

void UIManager::Render()
{
	sRenderingUIManager = this;
	NNS_G2dBeginRendering(&mOamRenderer);
	{
		Layout* layout = NULL;
		while ((layout = (Layout*)NNS_FndGetNextListObject(&mLayoutList, layout)) != NULL)
			layout->Render();
	}
	NNS_G2dEndRendering();
}

void UIManager::ProcessVBlank()
{
	NNS_G2dApplyOamManagerToHW(&mOamManager);
	NNS_G2dResetOamManagerBuffer(&mOamManager);
}