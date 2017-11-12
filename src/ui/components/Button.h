#pragma once

#include "common.h"


#include "UIComponent.h"

class Button : public UIComponent
{
public:
	enum ButtonState
	{
		BUTTON_STATE_NORMAL,
		BUTTON_STATE_PUSHED
	};

private:
	const NNSG2dCellData* mNormalState;
	const NNSG2dCellData* mPushedState;

	ButtonState mState;

	bool mWasDown;

public:
	Button(int x, int y, int width, int height, const NNSG2dCellData* normal, const NNSG2dCellData* pushed)
		: UIComponent(x, y, width, height), mNormalState(normal), mPushedState(pushed), mState(BUTTON_STATE_NORMAL), mWasDown(false)
	{ }

	void Render(GXOamAttr* pOAMBuffer, int &oamCount)
	{
		NNSG2dFVec2 trans = { mX * FX32_ONE, mY * FX32_ONE };
		if (mState == BUTTON_STATE_NORMAL)
			oamCount += NNS_G2dMakeCellToOams(&pOAMBuffer[oamCount], 128 - oamCount, mNormalState, NULL, &trans, -1, FALSE);
		else if(mState == BUTTON_STATE_PUSHED)
			oamCount += NNS_G2dMakeCellToOams(&pOAMBuffer[oamCount], 128 - oamCount, mPushedState, NULL, &trans, -1, FALSE);
	}
	
	void OnPenDown(int x, int y)
	{
		mState = BUTTON_STATE_PUSHED;
		mWasDown = true;
	}

	void OnPenMove(int x, int y)
	{
		if (mWasDown)
		{

		}
	}

	void OnPenUp(int x, int y)
	{
		if (mWasDown)
		{
			ButtonState old = mState;
			mState = BUTTON_STATE_NORMAL;
			mWasDown = false;
			if (mOnClickHandler)
				mOnClickHandler(mOnClickHandlerArg);
		}
	}
};