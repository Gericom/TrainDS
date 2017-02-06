#ifndef __FLEXTRACK_H__
#define __FLEXTRACK_H__

#include "TrackPieceEx.h"

class FlexTrack : public TrackPieceEx
{
public:
	VecFx32 mPoints[2];
	TrackPieceEx* mConnections[2];
	int mConnectionInPoints[2];
public:
	FlexTrack(VecFx32* a, VecFx32* b)
	{ 
		mPoints[0] = *a;
		mPoints[1] = *b;
		mConnections[0] = NULL;
		mConnections[1] = NULL;
		mConnectionInPoints[0] = -1;
		mConnectionInPoints[1] = -1;
	}

	int GetNrConnectionPoints()
	{
		return 2;
	}

	void GetConnectionPoint(int id, VecFx32* dst)
	{
		*dst = mPoints[id];
	}

	int GetOutPointId(int inPoint)
	{
		if (inPoint == 0)
			return 1;
		else
			return 0;
	}

	void GetConnnectedTrack(int id, TrackPieceEx* &track, int &inPoint)
	{
		track = mConnections[id];
		inPoint = mConnectionInPoints[id];
	}

	void Render(Map* map, TerrainManager* terrainManager);
	fx32 GetTrackLength(int inPoint);
	void CalculatePoint(int inPoint, fx32 progress, VecFx32* pPos, VecFx32* pDir, Map* map);
};

#endif