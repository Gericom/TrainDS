#include "common.h"

#include "menu/Game.h"
#include "terrain/track/FlexTrack.h"
#include "DragTool.h"
#include "AddTrackTool.h"

void AddTrackTool::OnDragStart(VecFx32* position)
{
	mStartPos = *position;
	//mNewTrackPiece = new FlexTrack(mContext->mGameController->mMap, &mStartPos, position);
	//mContext->mGameController->mMap->BeginAddTrackPiece(mNewTrackPiece);
	//mContext->mGameController->mMap->TrySnapGhostTrack(0);
}

void AddTrackTool::OnDragging(VecFx32* position)
{
	if (!mNewTrackPiece)
		return;
	//mNewTrackPiece->SetPoint(1, position);
	TrackPieceEx* ignore;
	int ignore2;
	//mNewTrackPiece->GetConnnectedTrack(0, ignore, ignore2);
	//mContext->mGameController->mMap->TrySnapGhostTrack(1, ignore);
}

void AddTrackTool::OnDragEnd(VecFx32* position)
{
	if (!mNewTrackPiece)
		return;
	//mNewTrackPiece->SetPoint(1, position);
	TrackPieceEx* ignore;
	int ignore2;
	//mNewTrackPiece->GetConnnectedTrack(0, ignore, ignore2);
	//mContext->mGameController->mMap->TrySnapGhostTrack(1, ignore);
	//mContext->mGameController->mMap->FinishAddTrackPiece(mNewTrackPiece);
	mNewTrackPiece = NULL;
	//FlexTrack* piece = new FlexTrack(mStartPos.x, mStartPos.y, mStartPos.z, position->x, position->y, position->z);
	//mContext->mMap->AddTrackPiece(piece);
}