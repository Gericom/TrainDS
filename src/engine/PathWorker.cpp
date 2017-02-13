#include <nitro.h>
#include "core.h"
#include "terrain\terrain.h"
#include "terrain\track\TrackPieceEx.h"
#include "PathWorker.h"

static void FX_Lerp(VecFx32* a, VecFx32* b, fx32 t, VecFx32* result)
{
	result->x = a->x + FX_Mul(b->x - a->x, t);
	result->y = a->y + FX_Mul(b->y - a->y, t);
	result->z = a->z + FX_Mul(b->z - a->z, t);
}

void PathWorker::CalculatePoint()
{
	mCurPiece->CalculatePoint(mCurInPoint, FX_Div(mCurDistance, mNextDistance), &mCurPoint, &mCurDirection, mMap);
	//mCurPoint.x = mCurPoint.y = mCurPoint.z = 0;
	//if (mNextDistance < 0) return;
	//mCurPiece->CalculatePoint(&mCurPiecePoint, &mNextPiecePoint, &mNextDirection, FX_Div(mCurDistance, mNextDistance), &mCurPoint, &mCurDirection);
}

void PathWorker::SetupPoint()
{
	mNextDistance = mCurPiece->GetTrackLength(mMap, mCurInPoint);
	/*fx32 xa = 0, za = 0;
	TrackPiece* a = mCurPiece;
	if(a->mRot == TRACKPIECE_ROT_0) za = FX32_HALF;
	else if(a->mRot == TRACKPIECE_ROT_90) { xa = FX32_HALF; za = FX32_ONE; }
	else if(a->mRot == TRACKPIECE_ROT_180) { xa = FX32_ONE; za = FX32_HALF; }
	else if(a->mRot == TRACKPIECE_ROT_270) xa = FX32_HALF;
	mCurPiecePoint.x = a->mPosition.x * FX32_ONE + xa;
	mCurPiecePoint.y = a->mPosition.y * TILE_HEIGHT;
	mCurPiecePoint.z = a->mPosition.z * FX32_ONE + za;
	if(mCurPiece->mNext[0] != NULL)
	{
		a = mCurPiece->mNext[0];
		xa = 0;
		za = 0;
		if(a->mRot == TRACKPIECE_ROT_0) za = FX32_HALF;
		else if(a->mRot == TRACKPIECE_ROT_90) { xa = FX32_HALF; za = FX32_ONE; }
		else if(a->mRot == TRACKPIECE_ROT_180) { xa = FX32_ONE; za = FX32_HALF; }
		else if(a->mRot == TRACKPIECE_ROT_270) xa = FX32_HALF;
		mNextPiecePoint.x = a->mPosition.x * FX32_ONE + xa;
		mNextPiecePoint.y = a->mPosition.y * TILE_HEIGHT;
		mNextPiecePoint.z = a->mPosition.z * FX32_ONE + za;

		VecFx32 diff;
		VEC_Subtract(&mNextPiecePoint, &mCurPiecePoint, &diff);
		VEC_Normalize(&diff, &mNextDirection);
		fx32 linDist = VEC_Mag(&diff);
		mNextDistance = mCurPiece->GetNextDistance(linDist);
	}
	else mNextDistance = -FX32_ONE;*/
}

PathWorker::PathWorker(TrackPieceEx* curPiece, int curInPoint, fx32 curDistance, Map* map)
{
	mCurPiece = curPiece;
	mCurInPoint = curInPoint;
	mCurDistance = curDistance;
	mMap = map;
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

			TrackPieceEx* newPiece;
			int newInPoint;
			mCurPiece->GetConnnectedTrack(mCurPiece->GetOutPointId(mCurInPoint), newPiece, newInPoint);
			if (newPiece != NULL)
			{
				mCurPiece = newPiece;
				mCurInPoint = newInPoint;
				SetupPoint();
			}
		}
		/*while(mCurDistance < 0 && mCurPiece->mPrev[0] != NULL)
		{
			mCurPiece = mCurPiece->mPrev[0];
			SetupPoint();
			if(mNextDistance < 0) break;
			mCurDistance = mNextDistance + mCurDistance;
		}*/
		CalculatePoint();
	}
	*point = mCurPoint;
	*direction = mCurDirection;
}