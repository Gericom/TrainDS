#ifndef __DEPOT_H__
#define __DEPOT_H__
#include "SimpleMenu.h"
#include "ui/UIManager.h"
#include "ui/layoutengine/FontManager.h"

#define DEPOT_VRAM_TRANSFER_MANAGER_NR_TASKS	64

class Depot : public SimpleMenu
{
private:
	NNSG3dResFileHeader* mEnvModel;
	NNSG3dRenderObj mEnvRenderObj;
	u16 sincos;

	NNSGfdVramTransferTask mVramTransferTaskArray[DEPOT_VRAM_TRANSFER_MANAGER_NR_TASKS];

	UIManager* mSubUIManager;
	FontManager* mFontManager;
	NNSG2dFont mFont;
	void* mFontData;
	NNSG2dImageProxy mImageProxy;
	NNSG2dImagePaletteProxy mImagePaletteProxy;
	Layout* mLayoutTest;
	NNSG2dCellDataBank* mLayoutCellDataBank;
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