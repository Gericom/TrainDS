#ifndef __TITLEMENU_H__
#define __TITLEMENU_H__

#define TITLE_MENU_VRAM_TRANSFER_MANAGER_NR_TASKS	64

#include "SimpleMenu.h"
#include "core/os/VAlarm.h"
#include "terrain/GameController.h"
#include "terrain/TerrainManager.h"
#include "ui/layoutengine/Layout.h"
#include "ui/layoutengine/PicturePane.h"
class TitleSequencePlayer;

class TitleMenu : public SimpleMenu
{
private:
	enum TitleMenuState
	{
		TITLE_MENU_STATE_LOGO_IN,
		TITLE_MENU_STATE_LOGO_WAIT,
		TITLE_MENU_STATE_LOGO_SCALE,
		TITLE_MENU_STATE_LOOP
	};

	GameController* mGameController;
	GameController::RenderMode mRenderMode;

	int mPassedFrameCounter;

	NNSSndStrmHandle mMusicHandle;

	FontManager* mFontManager;

	NNSG2dFont mFont;
	void* mFontData;
	NNSG2dFont mFont2;
	void* mFontData2;
	NNSG2dCharCanvas mCanvas;
	NNSG2dTextCanvas mTextCanvas;

	NNSGfdVramTransferTask mVramTransferTaskArray[TITLE_MENU_VRAM_TRANSFER_MANAGER_NR_TASKS];

	NNSG2dOamManagerInstance mSubObjOamManager;
	GXOamAttr mTmpSubOamBuffer[128];
	NNSG2dRendererInstance mOAMRender;
	NNSG2dRenderSurface mOAMRenderSurface;
	NNSG2dImageProxy mImageProxy;
	NNSG2dImagePaletteProxy mImagePaletteProxy;
	Layout* mLayoutTest;
	NNSG2dCellDataBank* mLayoutCellDataBank;
	PicturePane* mButtons[3][4];
	//PicturePane* mMissionsButton;
	//PicturePane* mSandboxButton;
	//PicturePane* mDepotButton;
	//PicturePane* mOptionsButton;
	int mKeyCounter;
	int mKeyDelay;
	int mSelectedButton;
	int mLeftRight;

	texture_t mLogoLargeTextureA;
	texture_t mLogoLargeTextureB;

	TitleMenuState mState;
	int mStateCounter;

	TitleSequencePlayer* mTSPlayer;

	OS::VAlarm* mVRAMCopyVAlarm;
	void OnVRAMCopyVAlarm();

	void VBlankIrq();

	bool mSwap;
	void SetSwapBuffersFlag();
public:
	TitleMenu() : SimpleMenu(17, 17), mRenderMode(GameController::RENDER_MODE_FAR), mState(TITLE_MENU_STATE_LOGO_IN), mStateCounter(0),
		mPassedFrameCounter(1), mSwap(false), mSelectedButton(0), mKeyCounter(0), mKeyDelay(0), mLeftRight(0)
	{ }

	void Initialize(int arg);

	static void OnVRAMCopyVAlarm(void* arg)
	{
		((TitleMenu*)arg)->OnVRAMCopyVAlarm();
	}

	void Render();
	void VBlank();
	void Finalize();

	static void VBlankIrqHandler()
	{
		((TitleMenu*)gRunningMenu)->VBlankIrq();
	}

	static void GotoMenu()
	{
		gNextMenuArg = 0;// TITLEMENU_ARG_DONT_PLAY_INTRO;
		gNextMenuCreateFunc = CreateMenu;
	}

	static BOOL CallBackAddOam(const GXOamAttr* pOam, u16 affineIndex, BOOL bDoubleAffine);
	static u16 CallBackAddAffine(const MtxFx22* mtx);

private:
	void HandleKeys();

	static Menu* CreateMenu()
	{
		return new TitleMenu();
	}
};

#endif