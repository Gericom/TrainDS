#include "common.h"

#include "terrain\track\TrackPieceEx.h"
#include "PathWorker.h"

void PathWorker::CalculatePoint()
{
	mCurPiece->CalculatePoint(mCurInPoint, FX_Div(mCurDistance, mNextDistance), &mCurPoint, &mCurDirection);
	//mCurPoint.x = mCurPoint.y = mCurPoint.z = 0;
	//if (mNextDistance < 0) return;
	//mCurPiece->CalculatePoint(&mCurPiecePoint, &mNextPiecePoint, &mNextDirection, FX_Div(mCurDistance, mNextDistance), &mCurPoint, &mCurDirection);
}

void PathWorker::SetupPoint()
{
	mNextDistance = mCurPiece->GetTrackLength(mCurInPoint);
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

PathWorker::PathWorker(TrackPieceEx* curPiece, int curInPoint, fx32 curDistance)
	: mCurPiece(curPiece), mCurInPoint(curInPoint), mCurDistance(curDistance)
{
	SetupPoint();
	//CalculatePoint();
	Proceed(0, NULL, NULL);
}

void PathWorker::Proceed(fx32 distance, VecFx32* point, VecFx32* direction)
{
	//if(distance != 0)
	{
		mCurDistance += distance;
		while(mCurDistance > 0 && mNextDistance >= 0 && mCurDistance > mNextDistance)
		{
			mCurDistance -= mNextDistance;

			TrackPieceEx* newPiece;
			int newInPoint;
			mCurPiece->GetVertex(mCurPiece->GetOutPointId(mCurInPoint))->GetNextTrack(mCurPiece, newPiece, newInPoint);
			//mCurPiece->GetConnnectedTrack(mCurPiece->GetOutPointId(mCurInPoint), newPiece, newInPoint);
			if (newPiece != NULL)
			{
				mCurPiece = newPiece;
				mCurInPoint = newInPoint;
				SetupPoint();
			}
			else
				mCurDistance = mCurDistance + mNextDistance - distance;
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
	if(point)
		*point = mCurPoint;
	if(direction)
		*direction = mCurDirection;
}