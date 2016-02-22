#ifndef __GAME_H__
#define __GAME_H__
#include "Menu.h"
#include "../vehicles/train.h"

class PathWorker;

class Game : public Menu
{
private:
	TerrainManager* mTerrainManager;

	NNSG3dResFileHeader* mLocModel;
	train_t mTrain;
	train_part_t mTrainPart;

	int mLastTouchState;
	BOOL mPicking;
	BOOL mProcessPicking;
	TPData mPickingPoint;
	BOOL mPickingOK;
public:
	void Initialize(int arg);
	void Render();
	void VBlank();
	void Finalize();
};

#endif