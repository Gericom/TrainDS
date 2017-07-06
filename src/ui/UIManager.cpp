#include <nitro.h>
#include <nnsys/g2d.h>
#include <nnsys/fnd.h>
#include "core.h"
#include "menu\Menu.h"
#include "components/UIComponent.h"
#include "UIManager.h"

UIManager::UIManager(UIManagerScreen screen)
	: mScreen(screen), mOnPenDownFunc(NULL), mOnPenMoveFunc(NULL), mOnPenUpFunc(NULL)
{
	NNS_FND_INIT_LIST(&mUIComponentList, UIComponent, mLink);
	MI_CpuClear8(&mLastTouchState, sizeof(mLastTouchState));
	NNS_G2dGetNewOamManagerInstanceAsFastTransferMode(&mOamManager, 0, 128, (mScreen == UI_MANAGER_SCREEN_MAIN ? NNS_G2D_OAMTYPE_MAIN : NNS_G2D_OAMTYPE_SUB));
}

void UIManager::AddUIComponent(UIComponent* uiComponent)
{
	NNS_FndAppendListObject(&mUIComponentList, uiComponent);
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
		UIComponent* uiComponent = NULL;
		while ((uiComponent = (UIComponent*)NNS_FndGetNextListObject(&mUIComponentList, uiComponent)) != NULL)
		{
			if (uiComponent->IsInside(x, y))
			{
				uiComponent->OnPenDown(x, y);
				break;
			}
		}
		if (uiComponent == NULL && mOnPenDownFunc != NULL) //no slice has done anything, let's pass the event through
			mOnPenDownFunc(mCallbackArg, x, y);
	}
	else if (mLastTouchState.touch && disp_point.touch && (mLastTouchState.x != x || mLastTouchState.y != y))//PenMove
	{
		UIComponent* uiComponent = NULL;
		while ((uiComponent = (UIComponent*)NNS_FndGetNextListObject(&mUIComponentList, uiComponent)) != NULL)
		{
			if (uiComponent->IsInside(x, y))
			{
				uiComponent->OnPenMove(x, y);
				break;
			}
		}
		if (uiComponent == NULL && mOnPenMoveFunc != NULL) //no slice has done anything, let's pass the event through
			mOnPenMoveFunc(mCallbackArg, x, y);
	}
	else if (mLastTouchState.touch && !disp_point.touch)//PenUp
	{
		UIComponent* uiComponent = NULL;
		while ((uiComponent = (UIComponent*)NNS_FndGetNextListObject(&mUIComponentList, uiComponent)) != NULL)
		{
			if (uiComponent->IsInside(x, y))
			{
				uiComponent->OnPenUp(x, y);
				break;
			}
		}
		if (uiComponent == NULL && mOnPenUpFunc != NULL) //no slice has done anything, let's pass the event through
			mOnPenUpFunc(mCallbackArg, x, y);
	}
	if (!raw_point.validity)
		mLastTouchState = disp_point;
	else
		mLastTouchState.touch = disp_point.touch;
}

void UIManager::Render()
{
	int numOam = 0;
	UIComponent* uiComponent = NULL;
	while ((uiComponent = (UIComponent*)NNS_FndGetNextListObject(&mUIComponentList, uiComponent)) != NULL)
	{
		uiComponent->Render(mOamBuffer, numOam);
	}
	NNS_G2dEntryOamManagerOam(&mOamManager, mOamBuffer, numOam);
}

void UIManager::ProcessVBlank()
{
	NNS_G2dApplyOamManagerToHW(&mOamManager);
	NNS_G2dResetOamManagerBuffer(&mOamManager);
}