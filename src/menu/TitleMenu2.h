#ifndef __TITLEMENU2_H__
#define __TITLEMENU2_H__

#include "SimpleMenu.h"
#include "core/os/VAlarm.h"
#include "terrain/GameController.h"

class TitleMenu2 : public SimpleMenu
{
private:
	GameController* mGameController;
	GameController::RenderMode mRenderMode;

	NNSSndStrmHandle mMusicHandle;

	NNSG2dFont mFont;
	void* mFontData;
	NNSG2dCharCanvas mCanvas;
	NNSG2dTextCanvas mTextCanvas;

	OS::VAlarm* mVRAMCopyVAlarm;
	void OnVRAMCopyVAlarm();
public:
	TitleMenu2() : SimpleMenu(17, 17), mRenderMode(GameController::RENDER_MODE_FAR)
	{ }

	void Initialize(int arg);

	static void OnVRAMCopyVAlarm(void* arg)
	{
		((TitleMenu2*)arg)->OnVRAMCopyVAlarm();
	}

	void Render();
	void VBlank();
	void Finalize();

	static void GotoMenu()
	{
		gNextMenuArg = 0;// TITLEMENU_ARG_DONT_PLAY_INTRO;
		gNextMenuCreateFunc = CreateMenu;
	}

private:
	void HandleKeys();

	static Menu* CreateMenu()
	{
		return new TitleMenu2();
	}
};

#endif