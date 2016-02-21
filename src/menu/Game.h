#ifndef __GAME_H__
#define __GAME_H__
#include "Menu.h"

class PathWorker;

class Game : public Menu
{
private:
	TerrainManager* mTerrainManager;

	NNSG3dResFileHeader* mLocModel;
	NNSG3dRenderObj mLocRenderObj;

	PathWorker* mPathWorker;
public:
	void Initialize(int arg);
	void Render();
	void VBlank();
	void Finalize();
};

#endif