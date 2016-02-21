#ifndef __DEPOT_H__
#define __DEPOT_H__
#include "Menu.h"

class Depot : public Menu
{
private:
	NNSG3dResFileHeader* mEnvModel;
	NNSG3dRenderObj mEnvRenderObj;
	u16 sincos;
public:
	Depot() { sincos = 0; }
	void Initialize(int arg);
	void Render();
	void VBlank();
	void Finalize();
};

#endif