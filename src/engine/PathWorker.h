#ifndef __PATH_WORKER_H__
#define __PATH_WORKER_H__

class PathWorker
{
private:
	trackpiece_t* mCurPiece;
	fx32 mCurDistance;
	VecFx32 mCurPoint;
	VecFx32 mCurDirection;

	VecFx32 mCurPiecePoint;
	//VecFx32 mPrevPiecePoint;
	VecFx32 mNextPiecePoint;

	//fx32 mPrevDistance;
	//VecFx32 mPrevDirection;
	fx32 mNextDistance;
	VecFx32 mNextDirection;

	void SetupPoint();
	void CalculatePoint();
public:
	PathWorker(trackpiece_t* curPiece, fx32 curDistance);
	void Proceed(fx32 distance, VecFx32* point, VecFx32* direction);
};

#endif