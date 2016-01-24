#ifndef __TITLE_MENU_H__
#define __TITLE_MENU_H__
#include <nnsys/g2d.h>
#include "Menu.h"

#define TITLEMENU_ARG_PLAY_INTRO		0
#define TITLEMENU_ARG_DONT_PLAY_INTRO	1

#define TITLEMENU_STATE_INTRO			0
#define TITLEMENU_STATE_MENU_IN			8
#define TITLEMENU_STATE_MENU_LOOP		9
#define TITLEMENU_STATE_MENU_OUT		10

class TitleMenu : public Menu
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
public:
	void Initialize(int arg);
	void Render();
	void VBlank();
	void Finalize();
private:
	void HandleKeys();
};

#endif