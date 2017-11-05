#pragma once
#include "Behavior.h"
#include "ListAdapter.h"

class ListRecyclerBehavior : public Behavior
{
protected:
	bool _initialized;

	s16 _listItemBaseOffsetY;
	s16 _listItemHeight;
	int _listPaneCount;
	PaneHolder** _paneHolders;
	int _firstVisiblePane;
	int _lastVisiblePane;

	ListAdapter* _listAdapter;

	bool _penDown;
	fx32 _penDownX;
	fx32 _penDownY;
	int _scrollY;
	int _scrollYOld;
	int _scrollYMax;
	bool _hasScrollStarted;


	bool OnPenDownCapture(fx32 px, fx32 py);
	void OnPenDownBubble(fx32 px, fx32 py);
	bool OnPenMoveCapture(fx32 px, fx32 py);
	void OnPenMoveBubble(fx32 px, fx32 py);
	bool OnPenUpCapture(fx32 px, fx32 py);
	void OnPenUpBubble(fx32 px, fx32 py);
	void UpdatePanePositions();
	PaneHolder* GetUnboundPaneHolder() const;
	PaneHolder* GetPaneHolderByPosition(int position) const;
	void SetupListItem(int position);

	void Initialize();

public:
	ListRecyclerBehavior()
		: _initialized(false), _paneHolders(NULL), _listAdapter(NULL), _penDown(false), _scrollY(0), _hasScrollStarted(false)
	{ }

	~ListRecyclerBehavior()
	{
		if (_paneHolders)
		{
			if (_listAdapter)
			{
				for (int i = 0; i < _listPaneCount; i++)
					_listAdapter->DestroyPaneHolder(_paneHolders[i]);
			}
			delete _paneHolders;
		}
	}

	void SetPane(Pane* pane)
	{
		Behavior::SetPane(pane);
		Initialize();
	}

	ListAdapter* GetAdapter() const { return _listAdapter; }
	void SetAdapter(ListAdapter* adapter)
	{
		_listAdapter = adapter;
		Initialize();
	}
};
