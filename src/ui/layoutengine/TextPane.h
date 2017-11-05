#pragma once
#include <wstring.h>
#include "Pane.h"
#include "Layout.h"

using namespace std;

class TextPane : public Pane
{
protected:
	const NNSG2dFont* mFont;
	u8 mPaletteIndex;
	text_pane_flags_t mTextFlags;
	u8 mCharacterSpacing;
	u8 mLineSpacing;
	wchar_t* mText;

	bool mInvalidated;
	NNSG2dCharCanvas mCanvas;
	NNSG2dTextCanvas mTextCanvas;
	u8* mTextCharacterData;
	int mTextCharacterDataLength;
	NNSG2dCellData* mTextCell;
	Layout::LayoutScreen mScreen;
	u32 mObjVramOffset;

	void RenderContent();

	void SetupCanvas(u32 &freeObjVramOffset)
	{
		int areaWidth = (mWidth + 7) / 8;
		int areaHeight = (mHeight + 7) / 8;
		//we can save a lot of objs by doing this
		if ((areaWidth % 4) == 3)
			areaWidth++;
		if ((areaHeight % 4) == 3)
			areaHeight++;
		mTextCharacterDataLength = areaWidth * areaHeight * 32;
		mTextCharacterData = new u8[mTextCharacterDataLength];
		mObjVramOffset = freeObjVramOffset;
		freeObjVramOffset += mTextCharacterDataLength;
		NNS_G2dCharCanvasInitForOBJ1D(&mCanvas, mTextCharacterData, areaWidth, areaHeight, NNS_G2D_CHARA_COLORMODE_16);
		NNS_G2dTextCanvasInit(&mTextCanvas, &mCanvas, mFont, mCharacterSpacing, mLineSpacing);
		mTextCell = (NNSG2dCellData*)new u8[NNS_G2dCharCanvasCalcCellDataSize1D(&mCanvas, false)];
		NNS_G2dCharCanvasMakeCell1D(
			mTextCell, &mCanvas, 0, 0, 0, GX_OAM_MODE_NORMAL, false, GX_OAM_EFFECT_NONE, GX_OAM_COLORMODE_16, mObjVramOffset / 32, mPaletteIndex >> 4, NNS_G2D_OBJVRAMMODE_32K, false);
	}

public:
	/*TextPane(Pane* parent, const char* name, int x, int y, int width, int height, const NNSG2dFont* font, u8 paletteIndex, const wchar_t* text)
		: Pane(parent, name, x, y, width, height), mFont(font), mPaletteIndex(paletteIndex), mCharacterSpacing(0), mLineSpacing(1), mInvalidated(true)
	{
		mTextFlags.textHAlignment = TEXT_PANE_TEXT_HALIGNMENT_CENTER;
		mTextFlags.textVAlignment = TEXT_PANE_TEXT_VALIGNMENT_CENTER;
		mText = new wchar_t[wcslen(text) + 2];
		wcscpy(mText, text);
		SetupCanvas();
	}*/

	TextPane(Pane* parent, lyt_res_txt1_t* txt1Res, const char* name, const NNSG2dFont* font, const wchar_t* text, Layout::LayoutScreen screen, u32 &freeObjVramOffset)
		: Pane(parent, txt1Res, name), mFont(font), mPaletteIndex(txt1Res->paletteidx), mTextFlags(txt1Res->textflags), mCharacterSpacing(txt1Res->characterspacing),
		mLineSpacing(txt1Res->linespacing), mScreen(screen), mInvalidated(true)
	{
		mText = new wchar_t[wcslen(text) + 2];
		wcscpy(mText, text);
		SetupCanvas(freeObjVramOffset);
	}

	virtual ~TextPane()
	{
		if (mText)
			delete mText;
		delete mTextCharacterData;
		delete mTextCell;
	}

	void SetText(const wchar_t* text)
	{
		if (mText)
			delete mText;
		mText = new wchar_t[wcslen(text) + 2];
		wcscpy(mText, text);
		mInvalidated = true;
	}
};