#pragma once

class Pane;

class PaneHolder
{
private:
	Pane* _itemPane;
	int _itemPosition;
	bool _isBound;
public:
	PaneHolder(Pane* itemPane)
		: _itemPane(itemPane), _itemPosition(-1), _isBound(false)
	{ }

	Pane* GetItemPane() const { return _itemPane; }

	int GetItemPosition() const { return _itemPosition; }
	void SetItemPosition(int position) { _itemPosition = position; }

	bool GetIsBound() const { return _isBound; }
	void SetIsBound(bool bound) { _isBound = bound; }
};

class ListAdapter
{
protected:
	virtual void OnBindPaneHolder(PaneHolder* paneHolder, int position) = 0;
public:
	virtual PaneHolder* CreatePaneHolder(Pane* itemPane) = 0;
	virtual void DestroyPaneHolder(PaneHolder* paneHolder) = 0;

	void BindPaneHolder(PaneHolder* paneHolder, int position)
	{
		OnBindPaneHolder(paneHolder, position);
		paneHolder->SetItemPosition(position);
		paneHolder->SetIsBound(true);
	}
	
	virtual int GetItemCount() = 0;
};