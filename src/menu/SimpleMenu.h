#ifndef __SIMPLEMENU_H__
#define __SIMPLEMENU_H__

#include "Menu.h"

class SimpleMenu : public Menu
{
private:
	int mFadeInFrames;
	int mFadeOutFrames;
public:
	SimpleMenu(int fadeInFrames, int fadeOutFrames)
		: mFadeInFrames(fadeInFrames), mFadeOutFrames(fadeOutFrames)
	{ }

	void Run(int arg)
	{
		Initialize(arg);
		//Fade in
		for (int i = 0; i < mFadeInFrames; i++)
		{
			int brightness = -16 + ((0 - -16) * i) / (mFadeInFrames - 1);
			Render();
			NNS_SndMain();
			OS_WaitVBlankIntr();
			VBlank();
			GX_SetMasterBrightness(brightness);
			GXS_SetMasterBrightness(brightness);
		}
		while (!gNextMenuCreateFunc)
		{
			Render();
			NNS_SndMain();
			OS_WaitVBlankIntr();
			VBlank();
		}
		//Fade out
		for (int i = 0; i < mFadeOutFrames; i++)
		{
			int brightness = 0 + ((-16 - 0) * i) / (mFadeOutFrames - 1);
			Render();
			NNS_SndMain();
			OS_WaitVBlankIntr();
			VBlank();
			GX_SetMasterBrightness(brightness);
			GXS_SetMasterBrightness(brightness);
		}
		Finalize();
	}

	virtual void Initialize(int arg) = 0;
	virtual void Render() = 0;
	virtual void VBlank() = 0;
	virtual void Finalize() = 0;
};

#endif