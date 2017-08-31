#pragma once
#include "lyt_res_struct.h"
#include "Pane.h"

class FontManager;

class Layout
{
protected:
	u16 mWidth;
	u16 mHeight;
	Pane* mRootPane;
public:
	enum LayoutScreen
	{
		LAYOUT_SCREEN_MAIN,
		LAYOUT_SCREEN_SUB
	};

	Layout(lyt_res_t* layoutResource, LayoutScreen screen, const NNSG2dCellDataBank* cells, FontManager* fontManager, u32 &freeObjVramOffset);

	void Render()
	{
		mRootPane->Render();
	}

	Pane* FindPaneByName(const char* name)
	{
		return mRootPane->FindPaneByName(name);
	}
};
