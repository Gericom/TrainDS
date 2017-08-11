#include <nitro.h>
#include "core.h"
#include "util.h"
#include "TrackPieceEx.h"
#include "TrackVertex.h"

/*void TrackVertex::InvalidateAll()
{
	for (int i = 0; i < 4; i++)
	{
		if (mConnections[i])
			mConnections[i]->Invalidate();
	}
}*/

bool TrackVertex::Connect(TrackPieceEx* piece, int inPoint)
{
	for (int i = 0; i < 4; i++)
	{
		if (mConnections[i] == NULL)
		{
			mConnections[i] = piece;
			mConnectionInPoints[i] = inPoint;
			//InvalidateAll();
			return true;
		}
	}
	return false;
}

void TrackVertex::Disconnect(TrackPieceEx* piece)
{
	for (int i = 0; i < 4; i++)
	{
		if (mConnections[i] == piece)
		{
			mConnections[i] = NULL;
			mConnectionInPoints[i] = -1;
			//InvalidateAll();
			return;
		}
	}
}

void TrackVertex::GetNextPoint(TrackPieceEx* piece, VecFx32* dst)
{
	TrackPieceEx* newPiece;
	int newInPoint;
	GetNextTrack(piece, newPiece, newInPoint);
	if (newPiece)
		newPiece->GetVertex(newPiece->GetOutPointId(newInPoint))->GetPosition(dst);
	else
		*dst = mPosition;
}

void TrackVertex::GetNextTrack(TrackPieceEx* cur, TrackPieceEx* &track, int &inPoint)
{
	VecFx32 curdir;
	VecFx32 otherdir;
	VecFx32 a;
	for (int i = 0; i < 4; i++)
	{
		if (mConnections[i] == cur)
		{
			cur->GetVertex(cur->GetOutPointId(mConnectionInPoints[i]))->GetPosition(&a);
			break;
		}
	}
	VEC_Subtract(&mPosition, &a, &curdir);
	VEC_Normalize(&curdir, &curdir);
	for (int i = 0; i < 4; i++)
	{
		if (mConnections[i] != NULL && mConnections[i] != cur)
		{
			mConnections[i]->GetVertex(mConnections[i]->GetOutPointId(mConnectionInPoints[i]))->GetPosition(&a);
			VEC_Subtract(&a, &mPosition, &otherdir);
			VEC_Normalize(&otherdir, &otherdir);
			fx32 dot = VEC_DotProduct(&curdir, &otherdir);
			if (dot >= 0)
			{
				track = mConnections[i];
				inPoint = mConnectionInPoints[i];
				return;
			}
		}
	}
	track = NULL;
}