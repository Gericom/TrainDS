#include <nitro.h>
#include <nnsys/g2d.h>
#include <nnsys/fnd.h>
#include "core.h"
#include "menu\Menu.h"
#include "UISlice.h"
#include "UIManager.h"

UIManager::UIManager(Menu* context, int screen)
	: mContext(context), mScreen(screen)
{
	NNS_FND_INIT_LIST(&mSliceList, UISlice, mLink);
	MI_CpuClear8(&mLastTouchState, sizeof(mLastTouchState));
	mOnPenDownFunc = NULL;
	mOnPenMoveFunc = NULL;
	mOnPenUpFunc = NULL;
	NNS_G2dGetNewOamManagerInstanceAsFastTransferMode(&mOamManager, 0, 128, (mScreen == UIMANAGER_SCREEN_MAIN) ? NNS_G2D_OAMTYPE_MAIN : NNS_G2D_OAMTYPE_SUB);
}

void UIManager::AddSlice(UISlice* slice)
{
	NNS_FndAppendListObject(&mSliceList, slice);
	//should we call an init function here?
}

void UIManager::ProcessInput()
{
	TPData raw_point;
    TPData disp_point;
	u16 keyData = PAD_Read();
	while (TP_RequestRawSampling(&raw_point) != 0);
	TP_GetCalibratedPoint(&disp_point, &raw_point);
	if(!mLastTouchState.touch && disp_point.touch)//PenDown
	{
		UISlice* slice = NULL;
		while ((slice = (UISlice*)NNS_FndGetNextListObject(&mSliceList, slice)) != NULL)
		{
			if(slice->OnPenDown(mContext, disp_point.x, disp_point.y)) break;
		}
		if(slice == NULL && mOnPenDownFunc != NULL) //no slice has done anything, let's pass the event through
			mOnPenDownFunc(mContext, disp_point.x, disp_point.y);
	}
	else if(mLastTouchState.touch && disp_point.touch && (mLastTouchState.x != disp_point.x || mLastTouchState.y != disp_point.y))//PenMove
	{
		UISlice* slice = NULL;
		while ((slice = (UISlice*)NNS_FndGetNextListObject(&mSliceList, slice)) != NULL)
		{
			if(slice->OnPenMove(mContext, disp_point.x, disp_point.y)) break;
		}
		if(slice == NULL && mOnPenMoveFunc != NULL) //no slice has done anything, let's pass the event through
			mOnPenMoveFunc(mContext, disp_point.x, disp_point.y);
	}
	else if(mLastTouchState.touch && !disp_point.touch)//PenUp
	{
		UISlice* slice = NULL;
		while ((slice = (UISlice*)NNS_FndGetNextListObject(&mSliceList, slice)) != NULL)
		{
			if(slice->OnPenUp(mContext, disp_point.x, disp_point.y)) break;
		}
		if(slice == NULL && mOnPenUpFunc != NULL) //no slice has done anything, let's pass the event through
			mOnPenUpFunc(mContext, disp_point.x, disp_point.y);
	}
	mLastTouchState = disp_point;
}

void UIManager::Render()
{
	UISlice* slice = NULL;
	while ((slice = (UISlice*)NNS_FndGetNextListObject(&mSliceList, slice)) != NULL)
	{
		slice->Render(mContext, &mOamManager);
	}
}

void UIManager::VBlankProc()
{
	NNS_G2dApplyOamManagerToHW(&mOamManager);
    NNS_G2dResetOamManagerBuffer(&mOamManager);
}