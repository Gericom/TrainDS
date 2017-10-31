#pragma once
#include "lyt_res_struct.h"
#include "Pane.h"

class FontManager;

class Layout
{
	friend class UIManager;
private:
	NNSFndLink mLink;
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

	Pane* FindPaneByName(const char* name) const
	{
		return mRootPane->FindPaneByName(name);
	}

	bool OnPenDown(fx32 px, fx32 py)
	{
		MtxFx32 identity = { FX32_ONE, 0, 0, FX32_ONE, 0, 0 };
		return mRootPane->OnPenDown(&identity, px, py);
	}

	bool OnPenMove(fx32 px, fx32 py)
	{
		MtxFx32 identity = { FX32_ONE, 0, 0, FX32_ONE, 0, 0 };
		return mRootPane->OnPenMove(&identity, px, py);
	}

	bool OnPenUp(fx32 px, fx32 py)
	{
		MtxFx32 identity = { FX32_ONE, 0, 0, FX32_ONE, 0, 0 };
		return mRootPane->OnPenUp(&identity, px, py);
	}
};
