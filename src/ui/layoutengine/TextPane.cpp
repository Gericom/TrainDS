#include <nitro.h>
#include "core.h"
#include "util.h"
#include "TextPane.h"

void TextPane::Render()
{
	if (!mFlags.visible)
		return;
	NNS_G2dPushMtx();
	{
		NNS_G2dTranslate(
			FX32_HALF * mParent->GetWidth() * mFlags.hParentCenter + mTranslationX * FX32_ONE - FX32_HALF * mWidth * mFlags.hOrigin,
			FX32_HALF * mParent->GetHeight() * mFlags.vParentCenter + mTranslationY * FX32_ONE - FX32_HALF * mHeight * mFlags.vOrigin,
			0);
		if (mRotation)
		{
			NNS_G2dTranslate(FX32_HALF * mWidth, FX32_HALF * mHeight, 0);
			NNS_G2dRotZ(FX_SinIdx(FX_DEG_TO_IDX(mRotation)), FX_CosIdx(FX_DEG_TO_IDX(mRotation)));
			NNS_G2dTranslate(-FX32_HALF * mWidth, -FX32_HALF * mHeight, 0);
		}
		if (mScaleX != FX16_ONE || mScaleY != FX16_ONE)
		{
			NNS_G2dTranslate(FX32_HALF * mWidth, FX32_HALF * mHeight, 0);
			NNS_G2dScale(mScaleX, mScaleY, FX32_ONE);
			NNS_G2dTranslate(-FX32_HALF * mWidth, -FX32_HALF * mHeight, 0);
		}
		RenderChildren();
		if (mFont && mText)
		{
			if (mInvalidated)
			{
				NNS_G2dCharCanvasClear(&mCanvas, 0);
				int flags = 0;
				if (mTextFlags.textHAlignment == TEXT_PANE_TEXT_HALIGNMENT_LEFT)
					flags |= NNS_G2D_HORIZONTALALIGN_LEFT;
				else if (mTextFlags.textHAlignment == TEXT_PANE_TEXT_HALIGNMENT_CENTER)
					flags |= NNS_G2D_HORIZONTALALIGN_CENTER;
				else
					flags |= NNS_G2D_HORIZONTALALIGN_RIGHT;
				if (mTextFlags.textVAlignment == TEXT_PANE_TEXT_VALIGNMENT_TOP)
					flags |= NNS_G2D_VERTICALALIGN_TOP;
				else if (mTextFlags.textHAlignment == TEXT_PANE_TEXT_VALIGNMENT_CENTER)
					flags |= NNS_G2D_VERTICALALIGN_MIDDLE;
				else
					flags |= NNS_G2D_VERTICALALIGN_BOTTOM;
				NNS_G2dTextCanvasDrawTextRect(
					&mTextCanvas, 0, 0, mWidth, mHeight, mPaletteIndex & 0xF, NNS_G2D_VERTICALORIGIN_TOP | NNS_G2D_HORIZONTALORIGIN_LEFT | flags, (NNSG2dChar*)mText);
				NNS_GfdRegisterNewVramTransferTask(mScreen == Layout::LAYOUT_SCREEN_MAIN ? NNS_GFD_DST_2D_OBJ_CHAR_MAIN : NNS_GFD_DST_2D_OBJ_CHAR_SUB, mObjVramOffset, mTextCharacterData, mTextCharacterDataLength);
				mInvalidated = false;
			}
			NNS_G2dDrawCell(mTextCell);
		}
	}
	NNS_G2dPopMtx();
}
