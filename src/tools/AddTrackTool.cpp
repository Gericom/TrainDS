#include <nitro.h>
#include "core.h"
#include "menu/Game.h"
#include "terrain/track/FlexTrack.h"
#include "DragTool.h"
#include "AddTrackTool.h"

void AddTrackTool::OnDragStart(VecFx32* position)
{
	mStartPos = *position;
	mNewTrackPiece = new FlexTrack(mStartPos.x, mStartPos.y, mStartPos.z, position->x, position->y, position->z);
	mContext->mMap->BeginAddTrackPiece(mNewTrackPiece);
	mContext->mMap->TrySnapGhostTrack();
}

void AddTrackTool::OnDragging(VecFx32* position)
{
	if (!mNewTrackPiece)
		return;
	mNewTrackPiece->mEndPosition = *position;
	mContext->mMap->TrySnapGhostTrack();
}

void AddTrackTool::OnDragEnd(VecFx32* position)
{
	if (!mNewTrackPiece)
		return;
	mNewTrackPiece->mEndPosition = *position;
	mContext->mMap->TrySnapGhostTrack();
	mContext->mMap->FinishAddTrackPiece(mNewTrackPiece);
	mNewTrackPiece = NULL;
	//FlexTrack* piece = new FlexTrack(mStartPos.x, mStartPos.y, mStartPos.z, position->x, position->y, position->z);
	//mContext->mMap->AddTrackPiece(piece);
}