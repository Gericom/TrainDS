#ifndef __GAMECONTROLLER_H__
#define __GAMECONTROLLER_H__

#include "Map.h"
#include "engine/FreeRoamCamera.h"
#include "managers/SfxManager.h"
#include "vehicles/Wagon.h"
#include "engine/Hemisphere.h"
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
	{
		mMap = new Map();
		mSfxManager = new SfxManager();
		mCamera = camera;
		mWagon = new Wagon(this, "a3");
		mHemisphere = new Hemisphere();
	}

	~GameController()
	{
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

	int mPickingXStart;
	int mPickingXEnd;
	int mPickingZStart;

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