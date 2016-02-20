#include <nitro.h>
#include "core.h"
#include "terrain\terrain.h"
#include "PathWorker.h"

static void FX_Lerp(VecFx32* a, VecFx32* b, fx32 t, VecFx32* result)
{
	result->x = a->x + FX_Mul(b->x - a->x, t);
	result->y = a->y + FX_Mul(b->y - a->y, t);
	result->z = a->z + FX_Mul(b->z - a->z, t);
}

void PathWorker::CalculatePoint()
{
	mCurPoint.x = mCurPoint.y = mCurPoint.z = 0;
	if(mCurDistance == 0)
	{
		mCurPoint.x = mCurPiece->x * FX32_ONE;
		mCurPoint.y = mCurPiece->y * TILE_HEIGHT;
		mCurPoint.z = mCurPiece->z * FX32_ONE;
		return;
	}
	if(mCurDistance < 0)
	{
		if(mPrevDistance < 0) return;
		VecFx32 a;
		a.x = mCurPiece->x * FX32_ONE;
		a.y = mCurPiece->y * TILE_HEIGHT;
		a.z = mCurPiece->z * FX32_ONE;
		VecFx32 b;
		b.x = mCurPiece->prev[0]->x * FX32_ONE;
		b.y = mCurPiece->prev[0]->y * TILE_HEIGHT;
		b.z = mCurPiece->prev[0]->z * FX32_ONE;
		FX_Lerp(&a, &b, FX_Div(-mCurDistance, mPrevDistance), &mCurPoint);
	}
	if(mNextDistance < 0) return;
	VecFx32 a;
	a.x = mCurPiece->x * FX32_ONE;
	a.y = mCurPiece->y * TILE_HEIGHT;
	a.z = mCurPiece->z * FX32_ONE;
	VecFx32 b;
	b.x = mCurPiece->next[0]->x * FX32_ONE;
	b.y = mCurPiece->next[0]->y * TILE_HEIGHT;
	b.z = mCurPiece->next[0]->z * FX32_ONE;
	FX_Lerp(&a, &b, FX_Div(mCurDistance, mNextDistance), &mCurPoint);
}

static fx32 getPieceDistance(trackpiece_t* a, trackpiece_t* b, VecFx32* direction)
{
	VecFx32 diff;
	diff.x = (b->x - a->x) * FX32_ONE;
	diff.y = (b->y - a->y) * TILE_HEIGHT;
	diff.z = (b->z - a->z) * FX32_ONE;

	VEC_Normalize(&diff, direction);
	return VEC_Mag(&diff);
}

PathWorker::PathWorker(trackpiece_t* curPiece, fx32 curDistance)
{
	mCurPiece = curPiece;
	mCurDistance = curDistance;
	if(mCurPiece->next[0] != NULL)
		mNextDistance = getPieceDistance(mCurPiece, mCurPiece->next[0], &mNextDirection);
	else mNextDistance = -FX32_ONE;
	if(mCurPiece->prev[0] != NULL)
		mPrevDistance = getPieceDistance(mCurPiece, mCurPiece->prev[0], &mPrevDirection);
	else mPrevDistance = -FX32_ONE;
	CalculatePoint();
}

void PathWorker::Proceed(fx32 distance, VecFx32* point, VecFx32* direction)
{
	if(distance != 0)
	{
		mCurDistance += distance;
		while(mCurDistance > 0 && mNextDistance >= 0 && mCurDistance > mNextDistance)
		{
			mCurDistance -= mNextDistance;
			mCurPiece = mCurPiece->next[0];
			if(mCurPiece->next[0] != NULL)
				mNextDistance = getPieceDistance(mCurPiece, mCurPiece->next[0], &mNextDirection);
			else mNextDistance = -FX32_ONE;
			if(mCurPiece->prev[0] != NULL)
				mPrevDistance = getPieceDistance(mCurPiece, mCurPiece->prev[0], &mPrevDirection);
			else mPrevDistance = -FX32_ONE;
		}
		while(mCurDistance < 0 && mPrevDistance >= 0 && -mCurDistance > mPrevDistance)
		{
			mCurDistance += mPrevDistance;
			mCurPiece = mCurPiece->prev[0];
			if(mCurPiece->next[0] != NULL)
				mNextDistance = getPieceDistance(mCurPiece, mCurPiece->next[0], &mNextDirection);
			else mNextDistance = -FX32_ONE;
			if(mCurPiece->prev[0] != NULL)
				mPrevDistance = getPieceDistance(mCurPiece, mCurPiece->prev[0], &mPrevDirection);
			else mPrevDistance = -FX32_ONE;
		}
		CalculatePoint();
	}
	*point = mCurPoint;
	if(mCurDistance == 0) direction->x = direction->y = direction->z = 0;
	else if(mCurDistance < 0) *direction = mPrevDirection;
	else *direction = mNextDirection;
}