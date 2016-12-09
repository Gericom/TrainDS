#ifndef __TITLE_MENU_H__
#define __TITLE_MENU_H__
#include <nnsys/g2d.h>
#include "SimpleMenu.h"

#define TITLEMENU_ARG_PLAY_INTRO		0
#define TITLEMENU_ARG_DONT_PLAY_INTRO	1

#define TITLEMENU_STATE_INTRO			0
#define TITLEMENU_STATE_MENU_IN			8
#define TITLEMENU_STATE_MENU_LOOP		9
#define TITLEMENU_STATE_MENU_OUT		10

#define DOUBLE_3D_THREAD_STACK_SIZE     1024

class TitleMenu : public SimpleMenu
{
private:
	int mState;
	NNSG2dFont mFont;
	void* mFontData;
	NNSG2dCellDataBank* mCellDataSubBank;
	void* mCellDataSub;
	NNSG2dImageProxy mImageProxy;
	NNSG2dImagePaletteProxy mImagePaletteProxy;
	NNSG2dOamManagerInstance mSubObjOamManager;
	GXOamAttr mTmpSubOamBuffer[128];
	int mSelButton;
	int mStateCounter;
	NNSG2dCharCanvas mCanvas;
	NNSG2dTextCanvas mTextCanvas;
	int mKeyTimeout;
	NNSSndStrmHandle mMusicHandle;

	NNSG3dResFileHeader* mBGModel;
	NNSG3dRenderObj mBGRenderObj;

	u32 mDouble3DThreadStack[DOUBLE_3D_THREAD_STACK_SIZE / sizeof(u32)];

	BOOL mFlipFlag;
	BOOL mSwap;

	void SetSwapBuffersflag();

	void Double3DThread();
public:
	TitleMenu() : SimpleMenu(17, 17) { }

	void Initialize(int arg);
	void Render();
	void VBlank();
	void Finalize();

	static void VBlankIntr();
	static void SetupFrame2N();
	static void SetupFrame2N_1();

	static void Double3DThread(void* arg)
	{
		((TitleMenu*)arg)->Double3DThread();
	}

	static void GotoMenu()
	{
		gNextMenuArg = 0;
		gNextMenuCreateFunc = CreateMenu;
	}

private:
	void HandleKeys();

	static Menu* CreateMenu()
	{
		return new TitleMenu();
	}
};

#endif