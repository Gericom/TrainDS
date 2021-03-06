#pragma once

#include "TrackPieceEx.h"

#define FLEXTRACK_NR_POINTS	6

#define FLEXTRACK_TRACK_WIDTH					FX32_CONST(0.4)
#define FLEXTRACK_INV_TRACK_WIDTH	FX32_CONST(1.0 / 0.4)

class FlexTrack : public TrackPieceEx
{
private:
	TrackVertex* mVertices[2];
	//VecFx32 mPoints[2];
	VecFx32 mCurvePoints[FLEXTRACK_NR_POINTS];
	VecFx32 mCurveNormals[FLEXTRACK_NR_POINTS];
	fx32 mCurveLength;
	box2d_t mBounds;
public:
	FlexTrack(Map* map, TrackVertex* a, TrackVertex* b)
		: TrackPieceEx(map)
	{ 
		mVertices[0] = a;
		a->Connect(this, 0);
		mVertices[1] = b;
		b->Connect(this, 1);
		Invalidate();
	}

	int GetNrConnectionPoints()
	{
		return 2;
	}

	int GetOutPointId(int inPoint)
	{
		if (inPoint == 0)
			return 1;
		else
			return 0;
	}

	TrackVertex* GetVertex(int id)
	{
		return mVertices[id];
	}

	void ConnectVertex(int id, TrackVertex* vtx)
	{
		if (mVertices[id])
			mVertices[id]->Disconnect(this);
		mVertices[id] = vtx;
		vtx->Connect(this, id);
	}

	void DisconnectVertex(int id)
	{
		if (mVertices[id])
		{
			mVertices[id]->Disconnect(this);
			mVertices[id] = NULL;
		}
	}

	/*void Connect(int id, TrackPieceEx* track, int inPoint, bool updatePos)
	{
		if (updatePos)
		{
			VecFx32 tmp;
			track->GetConnectionPoint(inPoint, &tmp);
			SetPoint(id, &tmp);
		}
		//this is to prevent an endless loop
		if (mConnections[id] == track && mConnectionInPoints[id] == inPoint)
			return;
		Disconnect(id);
		mConnections[id] = track;
		mConnectionInPoints[id] = inPoint;
		mConnections[id]->Connect(inPoint, this, id, false);
		Invalidate();
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
		Invalidate();
	}*/

	void Render();
	void RenderMarkers();
	fx32 GetTrackLength(int inPoint);
	void CalculatePoint(int inPoint, fx32 progress, VecFx32* pPos, VecFx32* pDir);
	void Invalidate();

	/*void SetPoint(int id, VecFx32* pos)
	{
		mPoints[id] = *pos;
		Invalidate();
		if (mConnections[id])
			mConnections[id]->Invalidate();
	}*/

	void GetBounds(box2d_t* box)
	{
		*box = mBounds;
	}

	void GetPosition(VecFx32* dst) 
	{
		*dst = mVertices[0]->GetPosition();
	}
};