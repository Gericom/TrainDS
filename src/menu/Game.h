#ifndef __GAME_H__
#define __GAME_H__
#include "Menu.h"
#include "vehicles/train.h"
#include "terrain/terrain.h"

class TerrainManager;
class UIManager;
class TrackBuildUISlice;
class LookAtCamera;
class ThirdPersonCamera;

class Game : public Menu
{
	friend class TrackBuildUISlice;
private:
	typedef void (Game::*PickingCallbackFunc)(u16 result);

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
	u16 mPenDownResult;
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

	void OnPenDownPickingCallback(u16 result);
	void OnPenUpPickingCallback(u16 result);

	void OnPenDown(int x, int y);
	void OnPenMove(int x, int y);
	void OnPenUp(int x, int y);

	void Render();
	void VBlank();
	void Finalize();
};

#endif