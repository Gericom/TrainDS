#include <nitro.h>
#include "core.h"
#include "menu/Game.h"
#include "terrain/track/FlexTrack.h"
#include "DragTool.h"
#include "AddTrackTool.h"

void AddTrackTool::OnDragStart(VecFx32* position)
{
	mStartPos = *position;
	mNewTrackPiece = new FlexTrack(&mStartPos, position);
	mContext->mMap->BeginAddTrackPiece(mNewTrackPiece);
	mContext->mMap->TrySnapGhostTrack(0);
}

void AddTrackTool::OnDragging(VecFx32* position)
{
	if (!mNewTrackPiece)
		return;
	mNewTrackPiece->mPoints[1] = *position;
	TrackPieceEx* ignore;
	int ignore2;
	mNewTrackPiece->GetConnnectedTrack(0, ignore, ignore2);
	mContext->mMap->TrySnapGhostTrack(1, ignore);
}

void AddTrackTool::OnDragEnd(VecFx32* position)
{
	if (!mNewTrackPiece)
		return;
	mNewTrackPiece->mPoints[1] = *position;
	TrackPieceEx* ignore;
	int ignore2;
	mNewTrackPiece->GetConnnectedTrack(0, ignore, ignore2);
	mContext->mMap->TrySnapGhostTrack(1, ignore);
	mContext->mMap->FinishAddTrackPiece(mNewTrackPiece);
	mNewTrackPiece = NULL;
	//FlexTrack* piece = new FlexTrack(mStartPos.x, mStartPos.y, mStartPos.z, position->x, position->y, position->z);
	//mContext->mMap->AddTrackPiece(piece);
}