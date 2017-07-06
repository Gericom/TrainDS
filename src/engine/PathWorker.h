#ifndef __PATH_WORKER_H__
#define __PATH_WORKER_H__

//typedef struct trackpiece_t;

class Map;
class TrackPieceEx;

class PathWorker
{
private:
	TrackPieceEx* mCurPiece;
	int mCurInPoint;

	fx32 mCurDistance;
	VecFx32 mCurPoint;
	VecFx32 mCurDirection;

	VecFx32 mCurPiecePoint;
	VecFx32 mNextPiecePoint;

	fx32 mNextDistance;
	VecFx32 mNextDirection;

	void SetupPoint();
	void CalculatePoint();
public:
	PathWorker(TrackPieceEx* curPiece, int curInPoint, fx32 curDistance);
	void Proceed(fx32 distance, VecFx32* point, VecFx32* direction);
	void GetCurrent(VecFx32* point, VecFx32* direction)
	{
		if (point)
			*point = mCurPoint;
		if (direction)
			*direction = mCurDirection;
	}
};

#endif