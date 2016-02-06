#ifndef __GAME_H__
#define __GAME_H__
#include "Menu.h"

class Game : public Menu
{
private:
	TerrainManager* mTerrainManager;
public:
	void Initialize(int arg);
	void Render();
	void VBlank();
	void Finalize();
};

#endif __GAME_H__