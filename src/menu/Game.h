#ifndef __GAME_H__
#define __GAME_H__
#include "SimpleMenu.h"
#include "vehicles/train.h"
#include "terrain/terrain.h"
#include "terrain/Map.h"
#include "terrain/managers/SfxManager.h"

class TerrainManager;
class UIManager;
class TrackBuildUISlice;
class LookAtCamera;
class ThirdPersonCamera;
class FreeRoamCamera;
class DragTool;
class Wagon;

class Game : public SimpleMenu
{
	friend class TrackBuildUISlice;
private:
	typedef void (*PickingCallbackFunc)(void* arg, picking_result_t result);

	enum PickingState
	{
		PICKING_STATE_READY,
		PICKING_STATE_RENDERING,
		PICKING_STATE_CAPTURING
	};

	NNSG3dResFileHeader* mLocModel;
	train_t mTrain;
	train_part_t mTrainPart;

	Wagon* mWagon;

	int mPickingPointX;
	int mPickingPointY;
	int mPickingXStart;
	int mPickingXEnd;
	int mPickingZStart;
	int mSelectedTrain;
	int mSelectedMapX;
	int mSelectedMapZ;
	OSTick mPenDownTime;
	OSTick mPenUpTime;
	PickingCallbackFunc mPickingCallback;
	void* mPickingCallbackArg;
	picking_result_t mPenDownResult;
	int mPenDownPointX;
	int mPenDownPointY;

	PickingState mPickingState;
	bool mPickingRequested;

	bool mAntiAliasEnabled;

	int mKeyTimer;

	void* mTrackBuildCellData;
	NNSG2dCellDataBank* mTrackBuildCellDataBank;
	NNSG2dImageProxy mImageProxy;
	NNSG2dImagePaletteProxy mImagePaletteProxy;

	NNSG2dFont mFont;
	void* mFontData;
	NNSG2dCharCanvas mCanvas;
	NNSG2dTextCanvas mTextCanvas;

	UIManager* mUIManager;
	TrackBuildUISlice* mTrackBuildUISlice;

	texture_t mShadowTex;

	DragTool* mDragTool;

	OSVAlarm mVRAMCopyVAlarm;

	int mRenderState;

	void OnPenDown(int x, int y);
	void OnPenMove(int x, int y);
	void OnPenUp(int x, int y);

	picking_result_t mPickingResult;

	void HandlePickingVBlank();
	void HandlePickingEarly();
	void HandlePickingLate();
public:
	FreeRoamCamera*/*ThirdPersonCamera**/ mCamera;

	Map* mMap;
	SfxManager* mSfxManager;

	void OnVRAMCopyVAlarm();

public:
	Game() : SimpleMenu(17, 17), mSelectedTrain(-1), mSelectedMapX(-1), mSelectedMapZ(-1), mAntiAliasEnabled(TRUE), mKeyTimer(0), 
		mPickingState(PICKING_STATE_READY), mPickingRequested(false) 
	{ }

	void Initialize(int arg);

	static void OnVRAMCopyVAlarm(void* arg)
	{
		((Game*)arg)->OnVRAMCopyVAlarm();
	}

	static void OnPenDown(Menu* context, int x, int y)
	{
		((Game*)context)->OnPenDown(x, y);
	}

	static void OnPenMove(Menu* context, int x, int y)
	{
		((Game*)context)->OnPenMove(x, y);
	}

	static void OnPenUp(Menu* context, int x, int y)
	{
		((Game*)context)->OnPenUp(x, y);
	}

	static void OnPenDownPickingCallback(void* arg, picking_result_t result)
	{
		((Game*)arg)->OnPenDownPickingCallback(result);
	}

	void OnPenDownPickingCallback(picking_result_t result);

	static void OnPenUpPickingCallback(void* arg, picking_result_t result)
	{
		((Game*)arg)->OnPenUpPickingCallback(result);
	}

	void OnPenUpPickingCallback(picking_result_t result);

	void RequestPicking(int x, int y, PickingCallbackFunc callback, void* arg);

	void Render();
	void VBlank();
	void Finalize();

	static void GotoMenu()
	{
		gNextMenuArg = 0;
		gNextMenuCreateFunc = CreateMenu;
	}

	void GetMapPosFromPickingResult(picking_result_t result, int &mapX, int &mapY)
	{
		if (PICKING_IDX(result) <= 0 || PICKING_TYPE(result) != PICKING_TYPE_MAP)
		{
			mapX = -1;
			mapY = -1;
			return;
		}
		int idx = PICKING_IDX(result) - 1;
		mapX = mPickingXStart + idx % (mPickingXEnd - mPickingXStart);
		mapY = mPickingZStart + idx / (mPickingXEnd - mPickingXStart);
	}

private:
	static Menu* CreateMenu()
	{
		return new Game();
	}
};

#endif