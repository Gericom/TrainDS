#include <nitro.h>
#include "core.h"
#include "util.h"
#include "PicturePane.h"

void PicturePane::Render()
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
		if (mCell)
			NNS_G2dDrawCell(mCell);		
	}
	NNS_G2dPopMtx();
}
