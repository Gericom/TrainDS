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

	void Connect(int id, TrackPieceEx* track, int inPoint, bool updatePos)
	{
		if (updatePos)
			track->GetConnectionPoint(inPoint, &mPoints[id]);
		//this is to prevent an endless loop
		if (mConnections[id] == track && mConnectionInPoints[id] == inPoint)
			return;
		Disconnect(id);
		mConnections[id] = track;
		mConnectionInPoints[id] = inPoint;
		mConnections[id]->Connect(inPoint, this, id, false);
	}

	void Disconnect(int id)
	{
		if (!mConnections[id])
			return;
		//this is to prevent an endless loop
		TrackPieceEx* old = mConnections[id];
		int old2 = mConnectionInPoints[id];
		mConnections[id] = NULL;
		mConnectionInPoints[id] = -1;
		old->Disconnect(old2);
	}

	void Render(Map* map, TerrainManager* terrainManager);
	void RenderMarkers(Map* map, TerrainManager* terrainManager);
	fx32 GetTrackLength(int inPoint);
	void CalculatePoint(int inPoint, fx32 progress, VecFx32* pPos, VecFx32* pDir, Map* map);
};

#endif