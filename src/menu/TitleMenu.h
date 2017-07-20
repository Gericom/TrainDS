#ifndef __TITLEMENU_H__
#define __TITLEMENU_H__

#include "SimpleMenu.h"
#include "core/os/VAlarm.h"
#include "terrain/GameController.h"
#include "terrain/TerrainManager.h"
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

	NNSG2dFont mFont;
	void* mFontData;
	NNSG2dCharCanvas mCanvas;
	NNSG2dTextCanvas mTextCanvas;

	texture_t mLogoLargeTexture;

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
		mPassedFrameCounter(1), mSwap(false)
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

private:
	void HandleKeys();

	static Menu* CreateMenu()
	{
		return new TitleMenu();
	}
};

#endif