#ifndef __DEPOT_H__
#define __DEPOT_H__
#include "SimpleMenu.h"

class Depot : public SimpleMenu
{
private:
	NNSG3dResFileHeader* mEnvModel;
	NNSG3dRenderObj mEnvRenderObj;
	u16 sincos;
public:
	Depot() : SimpleMenu(17,17) { sincos = 0; }
	void Initialize(int arg);
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
		return new Depot();
	}
};

#endif