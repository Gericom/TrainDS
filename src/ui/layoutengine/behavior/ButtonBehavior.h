#pragma once
#include "Behavior.h"

class ButtonBehavior : public Behavior
{
public:
	enum ButtonState
	{
		BUTTON_STATE_NORMAL,
		BUTTON_STATE_DOWN,
		BUTTON_STATE_SELECTED
	};
	typedef void(*OnActivateFunc)(void* arg);
protected:
	const NNSG2dCellData* _cellNormal;
	const NNSG2dCellData* _cellDown;

	ButtonState _buttonState;

	void* _onActivateArg;
	OnActivateFunc _onActivateFunc;

	void OnPenDownBubble(fx32 px, fx32 py);
	void OnPenMoveBubble(fx32 px, fx32 py);
	void OnPenUpBubble(fx32 px, fx32 py);

	void SetState(ButtonState state);
public:
	ButtonBehavior(const NNSG2dCellData* cellNormal, const NNSG2dCellData* cellDown, bool selected = false)
		: _cellNormal(cellNormal), _cellDown(cellDown), _onActivateFunc(NULL)
	{ 
		SetSelected(selected);
	}

	void SetSelected(bool selected)
	{
		SetState(selected ? BUTTON_STATE_SELECTED : BUTTON_STATE_NORMAL);
	}

	void SetPane(Pane* pane)
	{
		Behavior::SetPane(pane);
		//make sure the pane gets the right state
		SetState(_buttonState);
	}

	void SetOnActivateHandler(OnActivateFunc onActivateFunc, void* arg)
	{
		_onActivateFunc = onActivateFunc;
		_onActivateArg = arg;
	}

	void Activate()
	{
		SetSelected(true);
		if (_onActivateFunc)
			_onActivateFunc(_onActivateArg);
	}
};
