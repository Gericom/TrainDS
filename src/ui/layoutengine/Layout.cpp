#include <nitro.h>
#include "core.h"
#include "util.h"
#include "Pane.h"
#include "PicturePane.h"
#include "TextPane.h"
#include "FontManager.h"
#include "Layout.h"

static Pane* createPaneFromLytRes(Pane* parent, lyt_res_t* layoutResource, Layout::LayoutScreen screen, lyt_res_pan1_t* &curPane, const NNSG2dCellDataBank* cells, FontManager* fontManager, u32 &freeObjVramOffset)
{
	Pane* pane;
	switch (curPane->signature)
	{
	case LYT_RES_SIGNATURE_PAN1:
		pane = new Pane(parent, curPane, (const char*)(((u8*)layoutResource) + layoutResource->header.stringtableoffset + curPane->nameoffset));
		break;
	case LYT_RES_SIGNATURE_PIC1:
	{
		const NNSG2dCellData* cell = NULL;
		if (((lyt_res_pic1_t*)curPane)->cell != 0xFFFF)
			cell = NNS_G2dGetCellDataByIdx(cells, ((lyt_res_pic1_t*)curPane)->cell);
		pane = new PicturePane(parent, (lyt_res_pic1_t*)curPane, (const char*)(((u8*)layoutResource) + layoutResource->header.stringtableoffset + curPane->nameoffset), cell);
		break;
	}
	case LYT_RES_SIGNATURE_TXT1:
	{
		const NNSG2dFont* font = NULL;
		if (fontManager && ((lyt_res_txt1_t*)curPane)->fontid != 0xFFFF)
			font = fontManager->GetFontByName((const char*)(((u8*)layoutResource) + layoutResource->header.stringtableoffset + layoutResource->fonts.fontnameoffsets[((lyt_res_txt1_t*)curPane)->fontid]));
		pane = new TextPane(parent, (lyt_res_txt1_t*)curPane, (const char*)(((u8*)layoutResource) + layoutResource->header.stringtableoffset + curPane->nameoffset), font, (const wchar_t*)(((u8*)layoutResource) + layoutResource->header.stringtableoffset + ((lyt_res_txt1_t*)curPane)->textoffset), screen, freeObjVramOffset);
		break;
	}
	}
	int nrChildren = curPane->nrchildren;
	curPane = (lyt_res_pan1_t*)(((u8*)curPane) + curPane->blocksize);
	for (int i = 0; i < nrChildren; i++)
		pane->AddChild(createPaneFromLytRes(pane, layoutResource, screen, curPane, cells, fontManager, freeObjVramOffset));
	return pane;
}

Layout::Layout(lyt_res_t* layoutResource, LayoutScreen screen, const NNSG2dCellDataBank* cells, FontManager* fontManager, u32 &freeObjVramOffset)
{
	mWidth = layoutResource->header.width;
	mHeight = layoutResource->header.height;
	lyt_res_pan1_t* curPane = (lyt_res_pan1_t*)(((u8*)layoutResource) + layoutResource->header.rootpaneoffset);
	mRootPane = createPaneFromLytRes(NULL, layoutResource, screen, curPane, cells, fontManager, freeObjVramOffset);
}