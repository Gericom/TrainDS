#ifndef __GAMECONTROLLER_H__
#define __GAMECONTROLLER_H__

#include "Map.h"
#include "engine/FreeRoamCamera.h"
#include "managers/SfxManager.h"
#include "vehicles/Wagon.h"
class LookAtCamera;

class GameController
{
public:
	GameController()
	{
		mMap = new Map();
		mSfxManager = new SfxManager();
		mCamera = new FreeRoamCamera();
		mWagon = new Wagon(this, "a3");
	}

	~GameController()
	{
		delete mMap;
		delete mSfxManager;
		delete mCamera;
		delete mWagon;
	}

	Map* mMap;
	Wagon* mWagon;
	SfxManager* mSfxManager;
	FreeRoamCamera* mCamera;

	void Update()
	{
		mSfxManager->Update(mCamera);
	}
};

#endif