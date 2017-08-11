#pragma once

class TrackPieceEx;

class TrackVertex
{
private:
	VecFx32 mPosition;
	TrackPieceEx* mConnections[4];
	int mConnectionInPoints[4];

public:
	TrackVertex(const VecFx32* position)
		: mPosition(*position)
	{
		mConnections[0] = NULL;
		mConnections[1] = NULL;
		mConnections[2] = NULL;
		mConnections[3] = NULL;
		mConnectionInPoints[0] = -1;
		mConnectionInPoints[1] = -1;
		mConnectionInPoints[2] = -1;
		mConnectionInPoints[3] = -1;
	}

	void GetPosition(VecFx32* dst)
	{
		*dst = mPosition;
	}

	//void InvalidateAll();

	bool Connect(TrackPieceEx* piece, int inPoint);
	void Disconnect(TrackPieceEx* piece);

	void GetNextPoint(TrackPieceEx* piece, VecFx32* dst);
	void GetNextTrack(TrackPieceEx* cur, TrackPieceEx* &track, int &inPoint);
};