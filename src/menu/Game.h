#ifndef __GAME_H__
#define __GAME_H__
#include "SimpleMenu.h"
#include "vehicles/train.h"
#include "terrain/terrain.h"
#include "terrain/Map.h"

class TerrainManager;
class UIManager;
class TrackBuildUISlice;
class LookAtCamera;
class ThirdPersonCamera;

class Game : public SimpleMenu
{
	friend class TrackBuildUISlice;
private:
	typedef void (Game::*PickingCallbackFunc)(picking_result_t result);

	NNSG3dResFileHeader* mLocModel;
	train_t mTrain;
	train_part_t mTrainPart;

	/*LookAtCamera*/ThirdPersonCamera* mCamera;

	Map* mMap;

	bool mPicking;
	bool mProcessPicking;
	int mPickingPointX;
	int mPickingPointY;
	int mPickingXStart;
	int mPickingXEnd;
	int mPickingZStart;
	int mSelectedTrain;
	int mSelectedMapX;
	int mSelectedMapZ;
	bool mPickingOK;
	OSTick mPenDownTime;
	OSTick mPenUpTime;
	PickingCallbackFunc mPickingCallback;
	picking_result_t mPenDownResult;
	int mPenDownPointX;
	int mPenDownPointY;

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

	void Pick(int x, int y, PickingCallbackFunc callback);
public:
	Game() : SimpleMenu(17, 17), mSelectedTrain(-1), mSelectedMapX(-1), mSelectedMapZ(-1), mAntiAliasEnabled(TRUE), mKeyTimer(0) { }

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