#include <nitro.h>
#include <nnsys/g2d.h>
#include <nnsys/fnd.h>
#include "core.h"
#include "menu\Menu.h"
#include "UISlice.h"
#include "UIManager.h"

UIManager::UIManager(Menu* context)
	: mContext(context)
{
	NNS_FND_INIT_LIST(&mSliceList, UISlice, mLink);
	MI_CpuClear8(&mLastTouchState, sizeof(mLastTouchState));
	mOnPenDownFunc = NULL;
	mOnPenMoveFunc = NULL;
	mOnPenUpFunc = NULL;
}

void UIManager::AddSlice(UISlice* slice)
{
	NNS_FndAppendListObject(&mSliceList, slice);
	//should we call an init function here?
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
		UISlice* slice = NULL;
		while ((slice = (UISlice*)NNS_FndGetNextListObject(&mSliceList, slice)) != NULL)
		{
			if (slice->OnPenDown(mContext, x, y)) break;
		}
		if (slice == NULL && mOnPenDownFunc != NULL) //no slice has done anything, let's pass the event through
			mOnPenDownFunc(mContext, x, y);
	}
	else if (mLastTouchState.touch && disp_point.touch && (mLastTouchState.x != x || mLastTouchState.y != y))//PenMove
	{
		UISlice* slice = NULL;
		while ((slice = (UISlice*)NNS_FndGetNextListObject(&mSliceList, slice)) != NULL)
		{
			if (slice->OnPenMove(mContext, x, y)) break;
		}
		if (slice == NULL && mOnPenMoveFunc != NULL) //no slice has done anything, let's pass the event through
			mOnPenMoveFunc(mContext, x, y);
	}
	else if (mLastTouchState.touch && !disp_point.touch)//PenUp
	{
		UISlice* slice = NULL;
		while ((slice = (UISlice*)NNS_FndGetNextListObject(&mSliceList, slice)) != NULL)
		{
			if (slice->OnPenUp(mContext, x, y)) break;
		}
		if (slice == NULL && mOnPenUpFunc != NULL) //no slice has done anything, let's pass the event through
			mOnPenUpFunc(mContext, x, y);
	}
	if (!raw_point.validity)
		mLastTouchState = disp_point;
	else
		mLastTouchState.touch = disp_point.touch;
}

void UIManager::Render()
{
	UISlice* slice = NULL;
	while ((slice = (UISlice*)NNS_FndGetNextListObject(&mSliceList, slice)) != NULL)
	{
		slice->Render(mContext);//, &mOamManager);
	}
}