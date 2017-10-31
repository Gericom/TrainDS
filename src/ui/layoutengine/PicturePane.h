#pragma once
#include "Pane.h"

class PicturePane : public Pane
{
protected:
	const NNSG2dCellData* mCell;
public:
	PicturePane(Pane* parent, const char* name, int x, int y, int width, int height, const NNSG2dCellData* cell)
		: Pane(parent, name, x, y, width, height), mCell(cell)
	{ }

	PicturePane(Pane* parent, lyt_res_pic1_t* pic1Res, const char* name, const NNSG2dCellData* cell)
		: Pane(parent, pic1Res, name), mCell(cell)
	{ }

	void RenderContent();

	const NNSG2dCellData* GetCell() const { return mCell; }
	void SetCell(const NNSG2dCellData* cell) { mCell = cell; }
};
