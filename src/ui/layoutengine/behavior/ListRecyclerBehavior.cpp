#include "common.h"

#include "util.h"
#include "../Pane.h"
#include "../PicturePane.h"
#include "ListRecyclerBehavior.h"

bool ListRecyclerBehavior::OnPenDownCapture(fx32 px, fx32 py)
{
	if (!_initialized)
		return true;
	_penDown = true;
	_penDownX = px;
	_penDownY = py;
	_scrollYOld = _scrollY;
	_hasScrollStarted = false;
	return true;
}

void ListRecyclerBehavior::OnPenDownBubble(fx32 px, fx32 py)
{
	//this does nothing
}

bool ListRecyclerBehavior::OnPenMoveCapture(fx32 px, fx32 py)
{
	if (!_initialized)
		return true;
	if (!_penDown)
		return true;
	if (!_hasScrollStarted)
	{
		fx32 dx = px - _penDownX;
		fx32 dy = py - _penDownY;
		if (FX_Mul(dx, dx) + FX_Mul(dy, dy) > 7 * 7 * FX32_ONE)//(5 * 5))
		{
			if (MATH_ABS(dx) < MATH_ABS(dy))
				_hasScrollStarted = true;
			else
				_penDown = false;//wrong direction drag, so cancel it
		}
	}
	else
	{
		_scrollY = _scrollYOld + ((py - _penDownY) >> FX32_SHIFT);
		if (-_scrollY < 0)
		{
			_scrollY = 0;
			_scrollYOld = _scrollY;
			_penDownY = py;
		}
		else if (-_scrollY > _scrollYMax)
		{
			_scrollY = -_scrollYMax;
			_scrollYOld = _scrollY;
			_penDownY = py;
		}
		UpdatePanePositions();
	}
	return true;
}

void ListRecyclerBehavior::OnPenMoveBubble(fx32 px, fx32 py)
{
	//this does nothing
}

bool ListRecyclerBehavior::OnPenUpCapture(fx32 px, fx32 py)
{
	if (!_initialized)
		return true;
	if (!_penDown)
		return true;
	_penDown = false;
	if (!_hasScrollStarted)
	{
		//touch
	}
	return true;
}

void ListRecyclerBehavior::OnPenUpBubble(fx32 px, fx32 py)
{
	//this does nothing
}

void ListRecyclerBehavior::UpdatePanePositions()
{
	for (int i = 0; i < _listPaneCount; i++)
	{
		if (!_paneHolders[i]->GetIsBound())
			continue;
		int newY = _listItemBaseOffsetY + _paneHolders[i]->GetItemPosition() * _listItemHeight + _scrollY;
		if (newY + _listItemHeight < 0 || newY > _pane->GetHeight())
		{
			if (_paneHolders[i]->GetItemPosition() == _firstVisiblePane)
				_firstVisiblePane++;
			else
				_lastVisiblePane--;
			_paneHolders[i]->GetItemPane()->SetVisible(false);
			_paneHolders[i]->SetIsBound(false);
			continue;
		}
		_paneHolders[i]->GetItemPane()->SetTranslationY(newY);
	}
	int firstY = _listItemBaseOffsetY + _firstVisiblePane * _listItemHeight + _scrollY;
	while (_firstVisiblePane > 0 && firstY > 0)
	{
		SetupListItem(--_firstVisiblePane);
		firstY = _listItemBaseOffsetY + _firstVisiblePane * _listItemHeight + _scrollY;
	}
	int lastY = _listItemBaseOffsetY + _lastVisiblePane * _listItemHeight + _scrollY;
	while (_lastVisiblePane < _listAdapter->GetItemCount() - 1 && lastY + _listItemHeight < _pane->GetHeight())
	{
		SetupListItem(++_lastVisiblePane);
		lastY = _listItemBaseOffsetY + _lastVisiblePane * _listItemHeight + _scrollY;
	}
}

PaneHolder* ListRecyclerBehavior::GetUnboundPaneHolder() const
{
	for (int i = 0; i < _listPaneCount; i++)
		if (!_paneHolders[i]->GetIsBound())
			return _paneHolders[i];
	OS_Panic("Pane underrun!");
	return NULL;
}

PaneHolder* ListRecyclerBehavior::GetPaneHolderByPosition(int position) const
{
	for (int i = 0; i < _listPaneCount; i++)
		if (_paneHolders[i]->GetIsBound() && _paneHolders[i]->GetItemPosition() == position)
			return _paneHolders[i];
	return NULL;
}

void ListRecyclerBehavior::SetupListItem(int position)
{
	PaneHolder* h = GetUnboundPaneHolder();
	_listAdapter->BindPaneHolder(h, position);
	h->GetItemPane()->SetTranslationY(_listItemBaseOffsetY + position * _listItemHeight + _scrollY);
	h->GetItemPane()->SetVisible(true);
}

void ListRecyclerBehavior::Initialize()
{
	if (!_pane || !_listAdapter)
		return;
	//initialize information for proper list rendering
	NNSFndList* childList = _pane->GetChildrenList();
	Pane* firstChild = (Pane*)NNS_FndGetNextListObject(childList, NULL);
	Pane* secondChild = (Pane*)NNS_FndGetNextListObject(childList, firstChild);
	_listItemBaseOffsetY = firstChild->GetTranslationY();
	_listItemHeight = secondChild->GetTranslationY() - _listItemBaseOffsetY;
	Pane* child = secondChild;
	int count = 2;
	while ((child = (Pane*)NNS_FndGetNextListObject(childList, child)) != NULL)
		count++;
	_listPaneCount = count;
	_paneHolders = new PaneHolder*[_listPaneCount];
	//adapter stuff
	child = NULL;
	PaneHolder** holder = &_paneHolders[0];
	while ((child = (Pane*)NNS_FndGetNextListObject(childList, child)) != NULL)
		*holder++ = _listAdapter->CreatePaneHolder(child);
	//make sure all panes are hidden by default
	for (int i = 0; i < _listPaneCount; i++)
		_paneHolders[i]->GetItemPane()->SetVisible(false);
	int nrItems = _listAdapter->GetItemCount();
	int visibleItemCount = (_pane->GetHeight() + (_listItemHeight >> 1)) / _listItemHeight;
	int toInit = MATH_MIN(nrItems, visibleItemCount);
	for (int i = 0; i < toInit; i++)
		SetupListItem(i);
	_scrollYMax = _listItemBaseOffsetY + _listAdapter->GetItemCount() * _listItemHeight - _pane->GetHeight();
	_firstVisiblePane = 0;
	_lastVisiblePane = toInit - 1;
	_initialized = true;
}
