#ifndef __GAME_H__
#define __GAME_H__
#include "SimpleMenu.h"
#include "core/os/VAlarm.h"
#include "terrain/TerrainManager.h"
#include "terrain/GameController.h"

class TerrainManager;
class UIManager;
class TrackBuildUISlice;
class LookAtCamera;
class ThirdPersonCamera;
class FreeRoamCamera;
class DragTool;
class Wagon;
class Button;

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

	enum FrameType
	{
		//regular rendering, these frames should always appear in pairs
		FRAME_TYPE_MAIN_FAR,	//for the far part
		FRAME_TYPE_MAIN_NEAR,	//for the near part
		//injected frames
		FRAME_TYPE_MAIN_PICKING,//for picking
		FRAME_TYPE_SUB			//for sub screen 3d
	};

	FreeRoamCamera* mCamera;

	int mPickingPointX;
	int mPickingPointY;
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

	bool mSub3DInvalidated;

	void InvalidateSub3D()
	{
		mSub3DInvalidated = true;
	}

	int mKeyTimer;
	int mDebugKeyTimer;

	void* mTrackBuildCellData;
	NNSG2dCellDataBank* mTrackBuildCellDataBank;
	NNSG2dImageProxy mImageProxy;
	NNSG2dImagePaletteProxy mImagePaletteProxy;

	NNSG2dImageProxy mImageProxyMain;
	NNSG2dImagePaletteProxy mImagePaletteProxyMain;

	NNSG2dFont mSubFont;
	void* mSubFontData;
	NNSG2dCharCanvas mSubCanvas;
	NNSG2dTextCanvas mSubTextCanvas;

	NNSG2dFont mSubFont2;
	void* mSubFontData2;
	NNSG2dCharCanvas mSubCanvas2;
	NNSG2dTextCanvas mSubTextCanvas2;

	UIManager* mUIManager;
	TrackBuildUISlice* mTrackBuildUISlice;

	NNSG2dCellDataBank* mCellDataMainBank;
	void* mCellDataMain;

	NNSG2dOamManagerInstance mSubObjOamManager;
	GXOamAttr mTmpSubOamBuffer[128];

	NNSG2dCellDataBank* mCellDataSubBank;
	void* mCellDataSub;

	Button* mPointerButton;

	bool mTrainMode;

	texture_t mShadowTex;

	DragTool* mDragTool;

	OS::VAlarm* mVRAMCopyVAlarm;
	OS::VAlarm* mSub3DCopyVAlarm;

	FrameType mCurFrameType;
	FrameType mLastFrameType;

	void OnPenDown(int x, int y);
	void OnPenMove(int x, int y);
	void OnPenUp(int x, int y);

	picking_result_t mPickingResult;

	void HandlePickingVBlank();
	void HandlePickingEarly();
	void HandlePickingLate();

	int MakeTextCell(GXOamAttr* pOAM, int x, int y, int w, int h, int palette, u32 address);

	void OnVRAMCopyVAlarm();
	void OnSub3DCopyVAlarm();
public:
	GameController* mGameController;

public:
	Game() : SimpleMenu(17, 17), mSelectedTrain(-1), mSelectedMapX(-1), mSelectedMapZ(-1), mKeyTimer(0), mDebugKeyTimer(0), mTrainMode(false), 
		mPickingState(PICKING_STATE_READY), mPickingRequested(false), mCurFrameType(FRAME_TYPE_MAIN_FAR), mLastFrameType(FRAME_TYPE_MAIN_FAR),
		mSub3DCopyVAlarm(NULL), mSub3DInvalidated(false)
	{ }

	void Initialize(int arg);

	static void OnVRAMCopyVAlarm(void* arg)
	{
		((Game*)arg)->OnVRAMCopyVAlarm();
	}

	static void OnSub3DCopyVAlarm(void* arg)
	{
		((Game*)arg)->OnSub3DCopyVAlarm();
	}

	static void OnPenDown(void* arg, int x, int y)
	{
		((Game*)arg)->OnPenDown(x, y);
	}

	static void OnPenMove(void* arg, int x, int y)
	{
		((Game*)arg)->OnPenMove(x, y);
	}

	static void OnPenUp(void* arg, int x, int y)
	{
		((Game*)arg)->OnPenUp(x, y);
	}

	void RequestPicking(int x, int y, PickingCallbackFunc callback, void* arg);

	void Render();
	void VBlank();
	void Finalize();

	static void GotoMenu()
	{
		gNextMenuArg = 0;
		gNextMenuCreateFunc = CreateMenu;
	}

private:
	static Menu* CreateMenu()
	{
		return new Game();
	}
};

#endif