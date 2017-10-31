#include <nitro.h>
#include "core.h"
#include "util.h"
//this is probably bad to do, but I'm not gonna redo this
extern "C" {
#include <../build/libraries/g2d/include/g2di_Mtx32.h>
}
#include "Pane.h"

NNSG2dFVec2 Pane::GetBaseTranslation() const
{
	fx32 baseWidth = mParent ? mParent->GetWidth() : mWidth;
	fx32 baseHeight = mParent ? mParent->GetHeight() : mHeight;
	NNSG2dFVec2 result = {
		FX32_HALF * baseWidth * mFlags.hParentCenter + mTranslationX * FX32_ONE - FX32_HALF * mWidth * mFlags.hOrigin,
		FX32_HALF * baseHeight * mFlags.vParentCenter + mTranslationY * FX32_ONE - FX32_HALF * mHeight * mFlags.vOrigin
	};
	return result;
}

MtxFx32 Pane::ApplyTransform(const MtxFx32* mtx) const
{
	MtxFx32 tmpMtx, newMtx;
	NNSG2dFVec2 baseTrans = GetBaseTranslation();
	NNSi_G2dSetTranslate(&tmpMtx, baseTrans.x, baseTrans.y);
	NNSi_G2dMtxConcat32(mtx, &tmpMtx, &newMtx);

	if (mRotation || mScaleX != FX16_ONE || mScaleY != FX16_ONE)
	{
		NNSi_G2dSetTranslate(&tmpMtx, FX32_HALF * mWidth, FX32_HALF * mHeight);
		NNSi_G2dMtxConcat32(&newMtx, &tmpMtx, &newMtx);
		if (mRotation)
		{
			NNSi_G2dSetRotate(&tmpMtx, FX_SinIdx(FX_DEG_TO_IDX(mRotation)), FX_CosIdx(FX_DEG_TO_IDX(mRotation)));
			NNSi_G2dMtxConcat32(&newMtx, &tmpMtx, &newMtx);
		}
		if (mScaleX != FX16_ONE || mScaleY != FX16_ONE)
		{
			NNSi_G2dSetScale(&tmpMtx, mScaleX, mScaleY);
			NNSi_G2dMtxConcat32(&newMtx, &tmpMtx, &newMtx);
		}
		NNSi_G2dSetTranslate(&tmpMtx, -FX32_HALF * mWidth, -FX32_HALF * mHeight);
		NNSi_G2dMtxConcat32(&newMtx, &tmpMtx, &newMtx);
	}
	return newMtx;
}

bool Pane::IsPointInBounds(const MtxFx32* mtx, fx32 px, fx32 py, NNSG2dFVec2* newPoint) const
{
	NNSG2dFVec2 tmp = {px, py};
	NNSG2dFVec2 point;
	MulMtx22((const MtxFx22*)mtx, &tmp, &point);
	point.x -= mtx->_20;
	point.y -= mtx->_21;
	if (newPoint)
		*newPoint = point;
	return point.x >= 0 && point.y >= 0 && point.x < mWidth * FX32_ONE && point.y < mHeight * FX32_ONE;
}

void Pane::Render()
{
	if (!mFlags.visible)
		return;
	NNS_G2dPushMtx();
	{
		NNSG2dFVec2 baseTrans = GetBaseTranslation();
		NNS_G2dTranslate(baseTrans.x, baseTrans.y, 0);
		if (mRotation || mScaleX != FX16_ONE || mScaleY != FX16_ONE)
		{
			NNS_G2dTranslate(FX32_HALF * mWidth, FX32_HALF * mHeight, 0);
			if (mRotation)
				NNS_G2dRotZ(FX_SinIdx(FX_DEG_TO_IDX(mRotation)), FX_CosIdx(FX_DEG_TO_IDX(mRotation)));
			if (mScaleX != FX16_ONE || mScaleY != FX16_ONE)
				NNS_G2dScale(mScaleX, mScaleY, FX32_ONE);
			NNS_G2dTranslate(-FX32_HALF * mWidth, -FX32_HALF * mHeight, 0);
		}
		Pane* pane = NULL;
		while ((pane = (Pane*)NNS_FndGetPrevListObject(&mChildrenList, pane)) != NULL)
			pane->Render();
		RenderContent();
	}
	NNS_G2dPopMtx();
}

#define PANE_HANDLE_EVENT(name)																			\
	do {																								\
		MtxFx32 newMtx = ApplyTransform(mtx);															\
		NNSG2dFVec2 newPoint;																			\
		if (!IsPointInBounds(&newMtx, px, py, &newPoint))												\
			return false;																				\
		/*capture phase*/																				\
		if (GetTouchable() && mBehavior && mBehavior->##name##Capture(newPoint.x, newPoint.y))	\
			return true;																				\
																										\
		bool found = false;																				\
		Pane* pane = NULL;																				\
		while ((pane = (Pane*)NNS_FndGetPrevListObject(&mChildrenList, pane)) != NULL)					\
			if (found = pane->##name##(&newMtx, px, py))												\
				break;																					\
																										\
		/*bubble phase*/																				\
		if (GetTouchable() && mBehavior)																\
			mBehavior->##name##Bubble(newPoint.x, newPoint.y);									\
		return found || GetTouchable();																	\
	} while(0)

bool Pane::OnPenDown(const MtxFx32* mtx, fx32 px, fx32 py)
{
	PANE_HANDLE_EVENT(OnPenDown);
}

bool Pane::OnPenMove(const MtxFx32* mtx, fx32 px, fx32 py)
{
	PANE_HANDLE_EVENT(OnPenMove);
}

bool Pane::OnPenUp(const MtxFx32* mtx, fx32 px, fx32 py)
{
	PANE_HANDLE_EVENT(OnPenUp);
}
