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

static fx32 PieceRotSin[] =
{
	FX32_SIN0, FX32_SIN90, FX32_SIN180, FX32_SIN270
};

static fx32 PieceRotCos[] =
{
	FX32_COS0, FX32_COS90, FX32_COS180, FX32_COS270
};

void PathWorker::CalculatePoint()
{
	mCurPoint.x = mCurPoint.y = mCurPoint.z = 0;
	if(mCurDistance == 0)
	{
		mCurPoint = mCurPiecePoint;
		mCurDirection = mNextDirection;//?
		//mCurDirection.x = mCurDirection.y = mCurDirection.z = 0;
		return;
	}
	if(mCurPiece->kind == TRACKPIECE_KIND_FLAT)//Linear Interpolation
	{
		/*if(mCurDistance < 0)
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
			mCurDirection = mPrevDirection;
		}*/
		if(mNextDistance < 0) return;
		FX_Lerp(&mCurPiecePoint, &mNextPiecePoint, FX_Div(mCurDistance, mNextDistance), &mCurPoint);
		mCurDirection = mNextDirection;
	}
	else if(mCurPiece->kind == TRACKPIECE_KIND_FLAT_SMALL_CURVED)//Quarter Circle
	{
		/*if(mCurDistance < 0)
		{
			if(mPrevDistance < 0) return;
			fx32 frac = FX_Div(-mCurDistance, mPrevDistance);
			u16 idx = FX_DEG_TO_IDX(frac * 90);
			fx32 sin = FX_SinIdx(idx);//y
			fx32 cos = FX_CosIdx(idx);//x
			VecFx32 a;
			a.x = mCurPiece->x * FX32_ONE;
			a.y = mCurPiece->y * TILE_HEIGHT;
			a.z = mCurPiece->z * FX32_ONE;
			VecFx32 b;
			b.x = mCurPiece->prev[0]->x * FX32_ONE;
			b.y = mCurPiece->prev[0]->y * TILE_HEIGHT;
			b.z = mCurPiece->prev[0]->z * FX32_ONE;
			mCurPoint.x = a.x + FX_Mul(b.x - a.x, FX32_ONE - cos);
			mCurPoint.y = a.y + FX_Mul(b.y - a.y, frac);
			mCurPoint.z = a.z + FX_Mul(b.z - a.z, sin);
			mCurDirection.x = cos;
			mCurDirection.y = mPrevDirection.y;
			mCurDirection.z = sin;
		}*/
		if(mNextDistance < 0) return;
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
	}
}

void PathWorker::SetupPoint()
{
	fx32 xa = 0, za = 0;
	trackpiece_t* a = mCurPiece;
	if(a->rot == TRACKPIECE_ROT_0) za = FX32_HALF;
	else if(a->rot == TRACKPIECE_ROT_90) { xa = FX32_HALF; za = FX32_ONE; }
	else if(a->rot == TRACKPIECE_ROT_180) { xa = FX32_ONE; za = FX32_HALF; }
	else if(a->rot == TRACKPIECE_ROT_270) xa = FX32_HALF;
	mCurPiecePoint.x = a->x * FX32_ONE + xa;
	mCurPiecePoint.y = a->y * TILE_HEIGHT;
	mCurPiecePoint.z = a->z * FX32_ONE + za;
	if(mCurPiece->next[0] != NULL)
	{
		a = mCurPiece->next[0];
		xa = 0;
		za = 0;
		if(a->rot == TRACKPIECE_ROT_0) za = FX32_HALF;
		else if(a->rot == TRACKPIECE_ROT_90) { xa = FX32_HALF; za = FX32_ONE; }
		else if(a->rot == TRACKPIECE_ROT_180) { xa = FX32_ONE; za = FX32_HALF; }
		else if(a->rot == TRACKPIECE_ROT_270) xa = FX32_HALF;
		mNextPiecePoint.x = a->x * FX32_ONE + xa;
		mNextPiecePoint.y = a->y * TILE_HEIGHT;
		mNextPiecePoint.z = a->z * FX32_ONE + za;

		VecFx32 diff;
		VEC_Subtract(&mNextPiecePoint, &mCurPiecePoint, &diff);
		VEC_Normalize(&diff, &mNextDirection);
		fx32 linDist = VEC_Mag(&diff);
		if(mCurPiece->kind == TRACKPIECE_KIND_FLAT)//Linear Interpolation
			mNextDistance = linDist;
		else if(mCurPiece->kind == TRACKPIECE_KIND_FLAT_SMALL_CURVED)//Quarter Circle
		{
			//Calculate the arc length
			//r = linDist sqrt(0.5)
			fx32 r = FX_Mul(linDist, FX32_SQRT1_2);
			mNextDistance = FX_Mul32x64c(r, FX64C_PI_2);
		}
	}
	else mNextDistance = -FX32_ONE;
	/*if(mCurPiece->prev[0] != NULL)
	{
		a = mCurPiece->prev[0];
		xa = 0;
		za = 0;
		if(a->rot == TRACKPIECE_ROT_0) za = FX32_HALF;
		else if(a->rot == TRACKPIECE_ROT_90) { xa = FX32_HALF; za = FX32_ONE; }
		else if(a->rot == TRACKPIECE_ROT_180) { xa = FX32_ONE; za = FX32_HALF; }
		else if(a->rot == TRACKPIECE_ROT_270) xa = FX32_HALF;
		mPrevPiecePoint.x = a->x * FX32_ONE + xa;
		mPrevPiecePoint.y = a->y * TILE_HEIGHT;
		mPrevPiecePoint.z = a->z * FX32_ONE + za;
	}
	else mPrevDistance = -FX32_ONE;*/
}

PathWorker::PathWorker(trackpiece_t* curPiece, fx32 curDistance)
{
	mCurPiece = curPiece;
	mCurDistance = curDistance;
	SetupPoint();
	//if(mCurPiece->next[0] != NULL)
	//	mNextDistance = getPieceDistance(mCurPiece, mCurPiece->next[0], &mNextDirection);
	//else mNextDistance = -FX32_ONE;
	//if(mCurPiece->prev[0] != NULL)
	//	mPrevDistance = getPieceDistance(mCurPiece, mCurPiece->prev[0], &mPrevDirection);
	//else mPrevDistance = -FX32_ONE;
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
			//if(mCurPiece->next[0] != NULL)
			//	mNextDistance = getPieceDistance(mCurPiece, mCurPiece->next[0], &mNextDirection);
			//else mNextDistance = -FX32_ONE;
			//if(mCurPiece->prev[0] != NULL)
			//	mPrevDistance = getPieceDistance(mCurPiece, mCurPiece->prev[0], &mPrevDirection);
			//else mPrevDistance = -FX32_ONE;
		}
		/*while(mCurDistance < 0 && mPrevDistance >= 0 && -mCurDistance > mPrevDistance)
		{
			mCurDistance += mPrevDistance;
			mCurPiece = mCurPiece->prev[0];
			if(mCurPiece->next[0] != NULL)
				mNextDistance = getPieceDistance(mCurPiece, mCurPiece->next[0], &mNextDirection);
			else mNextDistance = -FX32_ONE;
			if(mCurPiece->prev[0] != NULL)
				mPrevDistance = getPieceDistance(mCurPiece, mCurPiece->prev[0], &mPrevDirection);
			else mPrevDistance = -FX32_ONE;
		}*/
		CalculatePoint();
	}
	*point = mCurPoint;
	*direction = mCurDirection;
}