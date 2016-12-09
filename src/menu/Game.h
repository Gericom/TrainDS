#ifndef __GAME_H__
#define __GAME_H__
#include "SimpleMenu.h"
#include "vehicles/train.h"
#include "terrain/terrain.h"

class TerrainManager;
class UIManager;
class TrackBuildUISlice;
class LookAtCamera;
class ThirdPersonCamera;

typedef uint16_t picking_result_t;

#define PICKING_TYPE_MAP	0
#define PICKING_TYPE_TRAIN	1

#define PICKING_COLOR(type,idx) ((picking_result_t)(0x8000 | ((type) & 7) << 12) | ((idx) & 0xFFF))
#define PICKING_TYPE(result) (((result) >> 12) & 7)
#define PICKING_IDX(result) ((result) & 0xFFF)

class Game : public SimpleMenu
{
	friend class TrackBuildUISlice;
private:
	typedef void (Game::*PickingCallbackFunc)(picking_result_t result);

	TerrainManager* mTerrainManager;

	NNSG3dResFileHeader* mLocModel;
	train_t mTrain;
	train_part_t mTrainPart;

	/*LookAtCamera*/ThirdPersonCamera* mCamera;

	BOOL mPicking;
	BOOL mProcessPicking;
	int mPickingPointX;
	int mPickingPointY;
	BOOL mPickingOK;
	OSTick mPenDownTime;
	OSTick mPenUpTime;
	PickingCallbackFunc mPickingCallback;
	picking_result_t mPenDownResult;
	int mPenDownPointX;
	int mPenDownPointY;

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

	void Pick(int x, int y, PickingCallbackFunc callback);
public:
	Game() : SimpleMenu(17, 17) { }

	void Initialize(int arg);

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

	void OnPenDownPickingCallback(picking_result_t result);
	void OnPenUpPickingCallback(picking_result_t result);

	void OnPenDown(int x, int y);
	void OnPenMove(int x, int y);
	void OnPenUp(int x, int y);

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