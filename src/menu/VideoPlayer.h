#ifndef __VIDEOPLAYER_H__
#define __VIDEOPLAYER_H__
#include "Menu.h"

typedef void (*onvideofinish_callback_t)();

typedef struct
{
	const char* videoPath;
	BOOL canSkip;
	onvideofinish_callback_t onvideofinishCallback;
} videoplayer_startup_params_t;

extern videoplayer_startup_params_t mVideoPlayerStartupParams;

class VideoPlayer : public Menu
{
private:
	NNSG2dCharCanvas mCanvas;
	NNSG2dTextCanvas mTextCanvas;

	int16_t* mWaveData;
	uint8_t* mTmpBuf1;
	uint8_t* mTmpBuf2;
	void Run_ITCM();
public:
	void Run(int arg);

	static void GotoMenu(const char* videoPath, BOOL canSkip, onvideofinish_callback_t finishCallback)
	{
		mVideoPlayerStartupParams.videoPath = videoPath;
		mVideoPlayerStartupParams.canSkip = canSkip;
		mVideoPlayerStartupParams.onvideofinishCallback = finishCallback;
		gNextMenuArg = 0;
		gNextMenuCreateFunc = CreateMenu;
	}

private:
	static Menu* CreateMenu() 
	{	
		return new VideoPlayer(); 
	}
};

#endif