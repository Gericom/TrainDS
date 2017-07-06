#include <nitro.h>
#include "core.h"
#include "util.h"
#include "videoplayer/Vx2Tables.h"
#include "videoplayer/DecodeARMv5Stride256VXS2.h"
#include "videoplayer/yuv2rgb_new.h"
#include "Menu.h"
#include "Game.h"
#include "VideoPlayer.h"

extern int videoPlayer_mWaveDataOffs_write;
extern volatile int videoPlayer_curBlock;
extern volatile int videoPlayer_nrFramesInQueue;
extern volatile int videoPlayer_firstQueueBlock;
extern volatile int videoPlayer_lastQueueBlock;
extern volatile int videoPlayer_mShouldCopyFrame;
extern AsmData videoPlayer_mAsmData;
extern OSAlarm videoPlayer_mFrameAlarm;
extern uint16_t* videoPlayer_mFrameQueue;

void VideoPlayer_VBlankIntr()
{
	OS_SetIrqCheckFlag(OS_IE_V_BLANK); // checking V-Blank interrupt
	if (videoPlayer_mShouldCopyFrame)
	{
		videoPlayer_mShouldCopyFrame = FALSE;
		if (videoPlayer_nrFramesInQueue > 0)
		{
			((uint16_t*)HW_DB_BG_PLTT)[0] = GX_RGB(0, 0, 0);
			MI_DmaCopy32Async(2, &videoPlayer_mFrameQueue[videoPlayer_firstQueueBlock * FRAME_SIZE], (uint16_t*)HW_BG_VRAM, FRAME_SIZE * 2, NULL, NULL);//REG_BG2PA, sizeof(BG23AffineInfo));
			videoPlayer_curBlock = videoPlayer_firstQueueBlock;
			//firstQueueBlock = (firstQueueBlock + 1) % NR_FRAME_BLOCKS;
			if (videoPlayer_firstQueueBlock + 1 >= NR_FRAME_BLOCKS)
				videoPlayer_firstQueueBlock = 0;
			else
				videoPlayer_firstQueueBlock++;
			videoPlayer_nrFramesInQueue--;
		}
		else
		{
#ifdef DEBUG_FRAME_TIMEINGS
			((uint16_t*)HW_DB_BG_PLTT)[0] = GX_RGB(31, 0, 0);
#endif
		}
	}
}

void VideoPlayer_frameHandler(void *arg)
{
	videoPlayer_mShouldCopyFrame = TRUE;
}

void VideoPlayer::Run_ITCM()
{
	FSFile video;
	FS_InitFile(&video);
	FS_OpenFile(&video, mVideoPlayerStartupParams.videoPath);
	uint32_t videoLength = FS_GetLength(&video);
	//read first wave block
	FS_ReadFile(&video, &mWaveData[0], AUDIO_BLOCK_SIZE * 2);
	DC_FlushRange(&mWaveData[0], AUDIO_BLOCK_SIZE * 2);
	while (SND_RecvCommandReply(SND_COMMAND_NOBLOCK) != NULL);
	const int timer = SND_TIMER_CLOCK / (32768 * PLAY_RATE / FRAME_RATE);
	SND_SetupChannelPcm(0, SND_WAVE_FORMAT_PCM16, mWaveData, SND_CHANNEL_LOOP_REPEAT, 0, WAVE_DATA_BUFFER_LENGTH / 2, 0x7F, SND_CHANNEL_DATASHIFT_NONE, timer, 0);
	SND_SetupChannelPcm(1, SND_WAVE_FORMAT_PCM16, mWaveData, SND_CHANNEL_LOOP_REPEAT, 0, WAVE_DATA_BUFFER_LENGTH / 2, 0x7F, SND_CHANNEL_DATASHIFT_NONE, timer, 127);
	SND_StartTimer(3, 0, 0, 0);
	SND_FlushCommand(SND_COMMAND_NOBLOCK);
	videoPlayer_mWaveDataOffs_write = 1;

	uint32_t length;
	FS_ReadFile(&video, &length, 4);
	FS_SeekFile(&video, -4, FS_SEEK_CUR);
	//Read first frame to buffer
	void* frame2 = mTmpBuf1;
	FS_ReadFile(&video, frame2, length + 4);

	int flushAudio = -1;
	//uint16_t* ptr = (uint16_t*)G2_GetBG3ScrPtr();
	int framenr = 1;
	bool nowait = false;
	while (FS_GetPosition(&video) < videoLength)   //1)
	{
		if (mVideoPlayerStartupParams.canSkip && (PAD_Read() & PAD_BUTTON_A))
		{
			nowait = TRUE;
			break;
		}
		void* frame = frame2;
		FS_ReadFile(&video, &length, 4);
		if (flushAudio >= 0)
		{
			DC_FlushRange(&mWaveData[flushAudio * AUDIO_BLOCK_SIZE], AUDIO_BLOCK_SIZE * 2);
			flushAudio = -1;
		}
		FS_SeekFile(&video, -4, FS_SEEK_CUR);
		frame2 = (frame == mTmpBuf1) ? mTmpBuf2 : mTmpBuf1;
		FS_ReadFileAsync(&video, frame2, length + 4);

		uint8_t* lasty = videoPlayer_mAsmData.Y[5];
		uint8_t* lastuv = videoPlayer_mAsmData.UV[5];
		videoPlayer_mAsmData.Y[5] = videoPlayer_mAsmData.Y[4];
		videoPlayer_mAsmData.UV[5] = videoPlayer_mAsmData.UV[4];
		videoPlayer_mAsmData.Y[4] = videoPlayer_mAsmData.Y[3];
		videoPlayer_mAsmData.UV[4] = videoPlayer_mAsmData.UV[3];
		videoPlayer_mAsmData.Y[3] = videoPlayer_mAsmData.Y[2];
		videoPlayer_mAsmData.UV[3] = videoPlayer_mAsmData.UV[2];
		videoPlayer_mAsmData.Y[2] = videoPlayer_mAsmData.Y[1];
		videoPlayer_mAsmData.UV[2] = videoPlayer_mAsmData.UV[1];
		videoPlayer_mAsmData.Y[1] = videoPlayer_mAsmData.Y[0];
		videoPlayer_mAsmData.UV[1] = videoPlayer_mAsmData.UV[0];
		videoPlayer_mAsmData.Y[0] = lasty;
		videoPlayer_mAsmData.UV[0] = lastuv;

		videoPlayer_mAsmData.Data = (uint8_t*)frame + 4;
		//DC_InvalidateRange(frame, 8192);//256 * 192 * 2);
		DecodeARMv5Stride256VXS2_Func(&videoPlayer_mAsmData);

		framenr++;
		if (!(framenr % FRAME_RATE))//Once per second
		{
			FS_ReadFileAsync(&video, &mWaveData[videoPlayer_mWaveDataOffs_write * AUDIO_BLOCK_SIZE], AUDIO_BLOCK_SIZE * 2);
			flushAudio = videoPlayer_mWaveDataOffs_write;
			videoPlayer_mWaveDataOffs_write = (videoPlayer_mWaveDataOffs_write + 1) % NR_WAVE_DATA_BUFFERS;
		}

		while (videoPlayer_nrFramesInQueue >= NR_FRAME_BLOCKS || videoPlayer_lastQueueBlock == videoPlayer_curBlock);

		//OSTick start = OS_GetTick();
		yuv2rgb_new(&videoPlayer_mAsmData.Y[0][0], &videoPlayer_mAsmData.UV[0][0], &videoPlayer_mFrameQueue[videoPlayer_lastQueueBlock * FRAME_SIZE]);
		DC_FlushRange(&videoPlayer_mFrameQueue[videoPlayer_lastQueueBlock * FRAME_SIZE], FRAME_SIZE * 2);//(uint16_t*)(HW_BG_VRAM + 96 * 1024 * lastQueueBlock));
																				 //OSTick diff = OS_GetTick() - start;
																				 //NOCASH_Printf("copy2vram: %d", diff);
		if (videoPlayer_lastQueueBlock + 1 >= NR_FRAME_BLOCKS)
			videoPlayer_lastQueueBlock = 0;
		else
			videoPlayer_lastQueueBlock++;
		//lastQueueBlock = (lastQueueBlock + 1) % NR_FRAME_BLOCKS;
		videoPlayer_nrFramesInQueue++;
	}
	FS_CloseFile(&video);
	while (!nowait && videoPlayer_nrFramesInQueue > 0);
	GX_VBlankIntr(false);
	OS_CancelAlarm(&videoPlayer_mFrameAlarm);
	SND_StopTimer(3, 0, 0, 0);
	SND_FlushCommand(SND_COMMAND_NOBLOCK);
	GX_SetMasterBrightness(-16);
	GXS_SetMasterBrightness(-16);
	if (mVideoPlayerStartupParams.onvideofinishCallback)
		mVideoPlayerStartupParams.onvideofinishCallback();
	else
		while (1);
}