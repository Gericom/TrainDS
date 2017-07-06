#ifndef __GAMECONTROLLER_H__
#define __GAMECONTROLLER_H__

#include "Map.h"
#include "engine/cameras/FreeRoamCamera.h"
#include "managers/SfxManager.h"
#include "vehicles/Wagon.h"
#include "engine/Hemisphere.h"
#include "engine/TOTDController.h"
class LookAtCamera;

class GameController
{
public:
	enum RenderMode
	{
		RENDER_MODE_FAR,
		RENDER_MODE_NEAR,
		RENDER_MODE_PICKING
	};

	GameController(LookAtCamera* camera)
		: mFogColor(GX_RGB(148 >> 3, 181 >> 3, 206 >> 3)), mLightColor(0x7FFF)
	{
		mLightDirection.x = -2048;
		mLightDirection.y = -2897;
		mLightDirection.z = -2048;
		mMap = new Map(this);
		mSfxManager = new SfxManager();
		mCamera = camera;
		mWagon = new Wagon(this, "a3");
		mHemisphere = new Hemisphere();
		mTOTDController = new TOTDController(this);
		mTOTDController->Update();
	}

	~GameController()
	{
		delete mTOTDController;
		delete mMap;
		delete mSfxManager;
		delete mWagon;
		delete mHemisphere;
	}

	Map* mMap;
	Wagon* mWagon;
	SfxManager* mSfxManager;
	LookAtCamera* mCamera;
	Hemisphere* mHemisphere;
	TOTDController* mTOTDController;

	int mPickingXStart;
	int mPickingXEnd;
	int mPickingZStart;

	GXRgb mFogColor;
	GXRgb mLightColor;
	VecFx16 mLightDirection;
	VecFx32 mSunPosition;
	int mSunX, mSunY;
	bool mDisplayFlare;
	GXRgb mSunColorMatch;

	void Render(RenderMode mode);

	void Update()
	{
		mWagon->Update();
		mSfxManager->Update(mCamera);
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
};

#endif