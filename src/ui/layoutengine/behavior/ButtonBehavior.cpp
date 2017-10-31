#include <nitro.h>
#include "core.h"
#include "util.h"
#include "../Pane.h"
#include "../PicturePane.h"
#include "ButtonBehavior.h"

void ButtonBehavior::OnPenDownBubble(fx32 px, fx32 py)
{
	SetState(BUTTON_STATE_DOWN);
	if (_onActivateFunc)
		_onActivateFunc(_onActivateArg);
}

void ButtonBehavior::OnPenMoveBubble(fx32 px, fx32 py)
{

}

void ButtonBehavior::OnPenUpBubble(fx32 px, fx32 py)
{
	SetState(BUTTON_STATE_NORMAL);
}

void ButtonBehavior::SetState(ButtonState state)
{
	_buttonState = state;
	if (!_pane)
		return;
	if (_buttonState == BUTTON_STATE_NORMAL)
		((PicturePane*)_pane)->SetCell(_cellNormal);
	else
		((PicturePane*)_pane)->SetCell(_cellDown);
}
