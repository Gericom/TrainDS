#include <nitro.h>
#include "core.h"
#include "menu/Game.h"
#include "DragTool.h"

void DragTool::OnPenDown(int x, int y)
{
	mPickingX = x;
	mPickingY = y;
	mContext->RequestPicking(x, y, DragTool::OnPenDownPickingCallback, this);
}

void DragTool::OnPenDownPickingCallback(picking_result_t result)
{
	int x, y;
	mContext->mGameController->GetMapPosFromPickingResult(result, x, y);
	if (x < 0 || y < 0)
		return;
	VecFx32 worldPos;
	mContext->mGameController->mMap->ScreenPosToWorldPos(mPickingX, mPickingY, x, y, &worldPos);
	//NOCASH_Printf("world: %d; %d; %d", worldPos.x / 4096, worldPos.y / 4096, worldPos.z / 4096);
	OnDragStart(&worldPos);
	mDragStarted = TRUE;
}

void DragTool::OnPenMove(int x, int y)
{
	if (!mDragStarted)
		return;
	mPickingX = x;
	mPickingY = y;
	mContext->RequestPicking(x, y, DragTool::OnPenMovePickingCallback, this);
}

void DragTool::OnPenMovePickingCallback(picking_result_t result)
{
	int x, y;
	mContext->mGameController->GetMapPosFromPickingResult(result, x, y);
	if (x < 0 || y < 0)
		return;
	VecFx32 worldPos;
	mContext->mGameController->mMap->ScreenPosToWorldPos(mPickingX, mPickingY, x, y, &worldPos);
	//NOCASH_Printf("world: %d; %d; %d", worldPos.x / 4096, worldPos.y / 4096, worldPos.z / 4096);
	OnDragging(&worldPos);
}

void DragTool::OnPenUp(int x, int y)
{
	if (!mDragStarted)
		return;
	mPickingX = x;
	mPickingY = y;
	mContext->RequestPicking(x, y, DragTool::OnPenUpPickingCallback, this);
}

void DragTool::OnPenUpPickingCallback(picking_result_t result)
{
	int x, y;
	mContext->mGameController->GetMapPosFromPickingResult(result, x, y);
	if (x < 0 || y < 0)
		return;
	VecFx32 worldPos;
	mContext->mGameController->mMap->ScreenPosToWorldPos(mPickingX, mPickingY, x, y, &worldPos);
	//NOCASH_Printf("world: %d; %d; %d", worldPos.x / 4096, worldPos.y / 4096, worldPos.z / 4096);
	OnDragEnd(&worldPos);
	mDragStarted = FALSE;
}
