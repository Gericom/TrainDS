#ifndef __VIDEOPLAYER_H__
#define __VIDEOPLAYER_H__
#include "Menu.h"

#define FRAME_RATE	(20)
#define PLAY_RATE	(FRAME_RATE)		//Makes it possible to test higher framerates without skipping audio (because the audiorate is increased aswell)

#define AUDIO_BLOCK_SIZE	(32768)// * 10 / FRAME_RATE)

#define NR_WAVE_DATA_BUFFERS	(4)

#define WAVE_DATA_BUFFER_LENGTH		(AUDIO_BLOCK_SIZE * NR_WAVE_DATA_BUFFERS)

#define FRAME_SIZE	(256 * 192)

#define NR_FRAME_BLOCKS		(10)

//shows a red bottom screen when no frames are available in time
//#define DEBUG_FRAME_TIMEINGS

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