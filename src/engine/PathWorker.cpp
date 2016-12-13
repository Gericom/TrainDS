#include <nitro.h>
#include "core.h"
#include "terrain\terrain.h"
#include "terrain\track\TrackPiece.h"
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
	if (mNextDistance < 0) return;
	mCurPiece->CalculatePoint(&mCurPiecePoint, &mNextPiecePoint, &mNextDirection, FX_Div(mCurDistance, mNextDistance), &mCurPoint, &mCurDirection);
	/*if(mCurPiece->kind == TRACKPIECE_KIND_FLAT)//Linear Interpolation
	{
		FX_Lerp(&mCurPiecePoint, &mNextPiecePoint, FX_Div(mCurDistance, mNextDistance), &mCurPoint);
		mCurDirection = mNextDirection;
	}
	else if(mCurPiece->kind == TRACKPIECE_KIND_FLAT_SMALL_CURVED_LEFT)//Quarter Circle
	{
		fx32 frac = FX_Div(mCurDistance, mNextDistance);
		u16 idx = FX_DEG_TO_IDX(frac * 90);
		fx32 sin, cos;
		fx32 sinfix, cosfix;
		if(mCurPiece->rot == TRACKPIECE_ROT_0 || mCurPiece->rot == TRACKPIECE_ROT_180)
		{
			cos = FX_SinIdx(idx);
			sin = FX_CosIdx(idx);
			sinfix = FX32_ONE - sin;
			cosfix = cos;
		}
		else
		{
			cos = FX_CosIdx(idx);
			sin = FX_SinIdx(idx);
			sinfix = sin;
			cosfix = FX32_ONE - cos;
		}
		mCurPoint.x = mCurPiecePoint.x + FX_Mul(mNextPiecePoint.x - mCurPiecePoint.x, cosfix);
		mCurPoint.y = mCurPiecePoint.y + FX_Mul(mNextPiecePoint.y - mCurPiecePoint.y, frac);
		mCurPoint.z = mCurPiecePoint.z + FX_Mul(mNextPiecePoint.z - mCurPiecePoint.z, sinfix);
		mCurDirection.y = mNextDirection.y;
		if(mCurPiece->rot == TRACKPIECE_ROT_0)//ai - b
		{
			mCurDirection.x = sin;
			mCurDirection.z = -cos;
		}
		else if(mCurPiece->rot == TRACKPIECE_ROT_90)//-a - bi
		{
			mCurDirection.x = -sin;
			mCurDirection.z = -cos;
		}
		else if(mCurPiece->rot == TRACKPIECE_ROT_180)//-ai + b
		{
			mCurDirection.x = -sin;
			mCurDirection.z = cos;
		}
		else if(mCurPiece->rot == TRACKPIECE_ROT_270)//a + bi
		{
			mCurDirection.x = sin;
			mCurDirection.z = cos;
		}
		VEC_Normalize(&mCurDirection, &mCurDirection);
	}*/
}

void PathWorker::SetupPoint()
{
	fx32 xa = 0, za = 0;
	TrackPiece* a = mCurPiece;
	if(a->rot == TRACKPIECE_ROT_0) za = FX32_HALF * SCENE_SCALE;
	else if(a->rot == TRACKPIECE_ROT_90) { xa = FX32_HALF * SCENE_SCALE; za = FX32_ONE * SCENE_SCALE; }
	else if(a->rot == TRACKPIECE_ROT_180) { xa = FX32_ONE * SCENE_SCALE; za = FX32_HALF * SCENE_SCALE; }
	else if(a->rot == TRACKPIECE_ROT_270) xa = FX32_HALF * SCENE_SCALE;
	mCurPiecePoint.x = a->x * FX32_ONE * SCENE_SCALE + xa;
	mCurPiecePoint.y = a->y * TILE_HEIGHT * SCENE_SCALE;
	mCurPiecePoint.z = a->z * FX32_ONE * SCENE_SCALE + za;
	if(mCurPiece->next[0] != NULL)
	{
		a = mCurPiece->next[0];
		xa = 0;
		za = 0;
		if(a->rot == TRACKPIECE_ROT_0) za = FX32_HALF * SCENE_SCALE;
		else if(a->rot == TRACKPIECE_ROT_90) { xa = FX32_HALF * SCENE_SCALE; za = FX32_ONE * SCENE_SCALE; }
		else if(a->rot == TRACKPIECE_ROT_180) { xa = FX32_ONE * SCENE_SCALE; za = FX32_HALF * SCENE_SCALE; }
		else if(a->rot == TRACKPIECE_ROT_270) xa = FX32_HALF * SCENE_SCALE;
		mNextPiecePoint.x = a->x * FX32_ONE * SCENE_SCALE + xa;
		mNextPiecePoint.y = a->y * TILE_HEIGHT * SCENE_SCALE;
		mNextPiecePoint.z = a->z * FX32_ONE * SCENE_SCALE + za;

		VecFx32 diff;
		VEC_Subtract(&mNextPiecePoint, &mCurPiecePoint, &diff);
		VEC_Normalize(&diff, &mNextDirection);
		fx32 linDist = VEC_Mag(&diff);
		mNextDistance = mCurPiece->GetNextDistance(linDist);
	}
	else mNextDistance = -FX32_ONE;
}

PathWorker::PathWorker(TrackPiece* curPiece, fx32 curDistance)
{
	mCurPiece = curPiece;
	mCurDistance = curDistance;
	SetupPoint();
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
			SetupPoint();
		}
		while(mCurDistance < 0 && mCurPiece->prev[0] != NULL)
		{
			mCurPiece = mCurPiece->prev[0];
			SetupPoint();
			if(mNextDistance < 0) break;
			mCurDistance = mNextDistance + mCurDistance;
		}
		CalculatePoint();
	}
	*point = mCurPoint;
	*direction = mCurDirection;
}