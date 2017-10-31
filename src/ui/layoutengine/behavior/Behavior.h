#pragma once

class Behavior
{
	friend class Pane;
protected:
	Pane* _pane;

	virtual bool OnPenDownCapture(fx32 px, fx32 py) { return false; }
	virtual void OnPenDownBubble(fx32 px, fx32 py) = 0;
	virtual bool OnPenMoveCapture(fx32 px, fx32 py) { return false; }
	virtual void OnPenMoveBubble(fx32 px, fx32 py) = 0;
	virtual bool OnPenUpCapture(fx32 px, fx32 py) { return false; }
	virtual void OnPenUpBubble(fx32 px, fx32 py) = 0;
public:
	Pane* GetPane() const { return _pane; }
	virtual void SetPane(Pane* pane) { _pane = pane; }
};