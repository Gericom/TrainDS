#pragma once
#include "ui/layoutengine/behavior/ListAdapter.h"
#include "ui/layoutengine/TextPane.h"

class DepotPaneHolder : public PaneHolder
{
public:
	TextPane* trainNamePane;

	DepotPaneHolder(Pane* itemPane)
		: PaneHolder(itemPane)
	{ 
		trainNamePane = (TextPane*)itemPane->FindPaneByNamePrefix("train_name");
	}
};

class DepotListAdapter : public ListAdapter
{
protected:
	void OnBindPaneHolder(PaneHolder* paneHolder, int position)
	{
		DepotPaneHolder* holder = (DepotPaneHolder*)paneHolder;
		wchar_t name[14];
		swprintf(name, 14, L"Test Name %d", position);
		holder->trainNamePane->SetText(name);
	}
public:
	PaneHolder* CreatePaneHolder(Pane* itemPane)
	{
		return new DepotPaneHolder(itemPane);
	}

	void DestroyPaneHolder(PaneHolder* paneHolder)
	{
		delete paneHolder;
	}

	int GetItemCount()
	{
		return 10;
	}
};
