#pragma once

#include "Map.h"
#include "engine/cameras/FreeRoamCamera.h"
#include "managers/SfxManager.h"
#include "vehicles/Wagon.h"
#include "engine/Hemisphere.h"
#include "engine/controllers/TOTDController.h"
#include "vehicles/Train.h"
class LookAtCamera;

class GameController
{
private:
	void RenderSky();
	void RenderFlare();

	void UpdateSingleFrame()
	{
		mTrain->Update();
		mSfxManager->Update(mCamera);
	}

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
		mTrain = new Train(this);
		mTrain->AddWagon(new Wagon(this, "a3"));
		mTrain->AddWagon(new Wagon(this, "usa_tanker"));
		mTrain->AddWagon(new Wagon(this, "usa_tanker"));
		mHemisphere = new Hemisphere();
		mTOTDController = new TOTDController(this);
		mTOTDController->Update();
	}

	~GameController()
	{
		delete mTOTDController;
		delete mMap;
		delete mSfxManager;
		mTrain->DestroyClearWagons();
		delete mTrain;
		delete mHemisphere;
	}

	Map* mMap;
	Train* mTrain;
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
	int mFlareAlpha;
	GXRgb mAverageSkyColor;

	void Render(RenderMode mode);

	void Update(int nrFrames)
	{
		for (int i = 0; i < nrFrames; i++)
			UpdateSingleFrame();
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