#include <nitro.h>
#include "core.h"
#include "util.h"
#include "overlay.h"
#include "videoplayer/Vx2Tables.h"
#include "videoplayer/DecodeARMv5Stride256VXS2.h"
#include "videoplayer/yuv2rgb_new.h"
#include "Menu.h"
#include "Game.h"
#include "VideoPlayer.h"

#define FRAME_RATE	(20)
#define PLAY_RATE	(FRAME_RATE)		//Makes it possible to test higher framerates without skipping audio (because the audiorate is increased aswell)

#define AUDIO_BLOCK_SIZE	(32768)// * 10 / FRAME_RATE)

#define NR_WAVE_DATA_BUFFERS	(4)

#define WAVE_DATA_BUFFER_LENGTH		(AUDIO_BLOCK_SIZE * NR_WAVE_DATA_BUFFERS)

videoplayer_startup_params_t mVideoPlayerStartupParams;

#include <nitro/dtcm_begin.h>
static BOOL mWaveDataOffs_write = 0;
//FrameQueue implementation
#define NR_FRAME_BLOCKS		(10)
static volatile int curBlock = -1;
static volatile int nrFramesInQueue = 0;
static volatile int firstQueueBlock = 0;//block to read from (most of the time (curBlock + 1) % 4)
static volatile int lastQueueBlock = 0;//block to write to (most of the time (firstQueueBlock + nrFramesInQueue) % 4) 

static volatile int mShouldCopyFrame = 0;

static AsmData mAsmData;
static OSAlarm mFrameAlarm;
static uint16_t* mFrameQueue;
#include <nitro/dtcm_end.h>

#define FRAME_SIZE	(256 * 192)

//shows a red bottom screen when no frames are available in time
//#define DEBUG_FRAME_TIMEINGS

#pragma VIDEOPLAYER_OVERLAY_BEGIN

static void VBlankIntr()
{
	OS_SetIrqCheckFlag(OS_IE_V_BLANK); // checking V-Blank interrupt
	if(mShouldCopyFrame)
	{
		mShouldCopyFrame = FALSE;
		if(nrFramesInQueue > 0)
		{
			((uint16_t*)HW_DB_BG_PLTT)[0] = GX_RGB(0,0,0);
			MI_DmaCopy32Async(2, &mFrameQueue[firstQueueBlock * FRAME_SIZE], (uint16_t*)HW_BG_VRAM, FRAME_SIZE * 2, NULL, NULL);//REG_BG2PA, sizeof(BG23AffineInfo));
			curBlock = firstQueueBlock;
			//firstQueueBlock = (firstQueueBlock + 1) % NR_FRAME_BLOCKS;
			if(firstQueueBlock + 1 >= NR_FRAME_BLOCKS)
				firstQueueBlock = 0;
			else
				firstQueueBlock++;
			nrFramesInQueue--;
		}
		else
		{
#ifdef DEBUG_FRAME_TIMEINGS
			((uint16_t*)HW_DB_BG_PLTT)[0] = GX_RGB(31,0,0);
#endif
		}
	} 
}

static void frameHandler(void *arg)
{
	mShouldCopyFrame = TRUE;
}

#pragma VIDEOPLAYER_OVERLAY_END

#include <nitro/code16.h>
void VideoPlayer::Run(int arg)
{
	curBlock = -1;
	nrFramesInQueue = 0;
	firstQueueBlock = 0;//block to read from (most of the time (curBlock + 1) % 4)
	lastQueueBlock = 0;//block to write to (most of the time (firstQueueBlock + nrFramesInQueue) % 4) 
	mShouldCopyFrame = 0;
	//load vx2_decoder overlay
	LOAD_OVERLAY_ITCM(videoplayer_itcm);

	GX_SetBankForLCDC(GX_VRAM_LCDC_ALL);
	MI_CpuClearFast((void*)HW_LCDC_VRAM, HW_LCDC_VRAM_SIZE);
	GX_DisableBankForLCDC();

	MI_CpuFillFast((void*)HW_OAM, 192, HW_OAM_SIZE);   // clear OAM
	MI_CpuClearFast((void*)HW_PLTT, HW_PLTT_SIZE);     // clear the standard palette

	MI_CpuFillFast((void*)HW_DB_OAM, 192, HW_DB_OAM_SIZE);     // clear OAM
	MI_CpuClearFast((void*)HW_DB_PLTT, HW_DB_PLTT_SIZE);       // clear the standard palette

	OS_SetIrqFunction(OS_IE_V_BLANK, VBlankIntr);
	//allocate wave buffers
	mWaveData = (int16_t*)NNS_FndAllocFromExpHeapEx(gHeapHandle, WAVE_DATA_BUFFER_LENGTH * 2, 32);
	//allocate framequeue
	mFrameQueue = (uint16_t*)NNS_FndAllocFromExpHeapEx(gHeapHandle, FRAME_SIZE * NR_FRAME_BLOCKS * 2, 32);
	//allocate temp buffers
	mTmpBuf1 = (uint8_t*)NNS_FndAllocFromExpHeapEx(gHeapHandle, 32768, 32);
	mTmpBuf2 = (uint8_t*)NNS_FndAllocFromExpHeapEx(gHeapHandle, 32768, 32);

	GX_SetBankForBG(GX_VRAM_BG_512_ABCD);
	GX_SetGraphicsMode(GX_DISPMODE_GRAPHICS, GX_BGMODE_3, GX_BG0_AS_2D);
	GX_SetVisiblePlane(GX_PLANEMASK_BG3);
	G2_SetBG3ControlDCBmp(GX_BG_SCRSIZE_DCBMP_256x256, GX_BG_AREAOVER_XLU, GX_BG_BMPSCRBASE_0x00000);

	MI_DmaFill32Async(2, (uint16_t*)HW_BG_VRAM, 0, FRAME_SIZE * 2, NULL, NULL);

	/*if(mVideoPlayerStartupParams.canSkip)
	{
		GXS_SetOBJVRamModeChar(GX_OBJVRAMMODE_CHAR_1D_32K);
		GX_SetBankForSubOBJ(GX_VRAM_SUB_OBJ_16_I);

		NNS_G2dCharCanvasInitForOBJ1D(&mCanvas, G2S_GetOBJCharPtr(), 32, 2, NNS_G2D_CHARA_COLORMODE_16);
		NNS_G2dArrangeOBJ1D(&((GXOamAttr*)HW_DB_OAM)[0], 32, 2, 0, 88, GX_OAM_COLORMODE_16, 0, NNS_G2D_OBJVRAMMODE_32K);
		NNS_G2dTextCanvasInit(&mTextCanvas, &mCanvas, &gSMGFont, 0, 0);
		NNS_G2dCharCanvasClear(&mCanvas, 0);
		NNS_G2dTextCanvasDrawTextRect(&mTextCanvas, 0, 0, 256, 16, 1, NNS_G2D_VERTICALORIGIN_TOP | NNS_G2D_HORIZONTALORIGIN_LEFT | NNS_G2D_HORIZONTALALIGN_CENTER | NNS_G2D_VERTICALALIGN_MIDDLE, (NNSG2dChar*)L"Fist A to skip");

		for(int i = 0; i < 16; i++)
		{
			int rnew = 0 + ((16 - 0) * i) / 15;
			int gnew = 0 + ((16 - 0) * i) / 15;
			int bnew = 0 + ((16 - 0) * i) / 15;
			((uint16_t*)HW_DB_OBJ_PLTT)[i] = GX_RGB(rnew, gnew, bnew);
		}

		GXS_SetVisiblePlane(GX_PLANEMASK_OBJ);
	}*/

	GX_SetMasterBrightness(0);
	GXS_SetMasterBrightness(0);

	OS_CreateAlarm(&mFrameAlarm);
	OS_SetPeriodicAlarm(&mFrameAlarm, OS_GetTick() + OS_MilliSecondsToTicks(1), OS_MicroSecondsToTicks(1000000 / PLAY_RATE), &frameHandler, NULL);

	mAsmData.Width = 256;
	mAsmData.Height = 192;
	for(int i = 0; i < 6; i++)
	{
		mAsmData.Y[i] = (uint8_t*)NNS_FndAllocFromExpHeapEx(gHeapHandle, mAsmData.Height * 256, 16);
		mAsmData.UV[i] = (uint8_t*)NNS_FndAllocFromExpHeapEx(gHeapHandle, mAsmData.Height / 2 * 256, 16);
	}
	mAsmData.Table0 = (void*)&Vx2Table0[0];
	mAsmData.Table1 = (void*)&Vx2Table1[0];
	mAsmData.MinMaxTable = &Vx2MinMaxTable[0];
	mAsmData.Quantizer = 0;
	mAsmData.YuvFormat = 0;
	MI_CpuClear8(&mAsmData.Internal[0], sizeof(mAsmData.Internal));
	Run_ITCM();
}
#include <nitro/codereset.h>

#pragma VIDEOPLAYER_OVERLAY_BEGIN
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
	mWaveDataOffs_write = 1;

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
	while(FS_GetPosition(&video) < videoLength)   //1)
	{
		if(mVideoPlayerStartupParams.canSkip && (PAD_Read() & PAD_BUTTON_A))
		{
			nowait = TRUE;
			break;
		}
		void* frame = frame2;
		FS_ReadFile(&video, &length, 4);
		if(flushAudio >= 0)
		{
			DC_FlushRange(&mWaveData[flushAudio * AUDIO_BLOCK_SIZE], AUDIO_BLOCK_SIZE * 2);
			flushAudio = -1;
		}
		FS_SeekFile(&video, -4, FS_SEEK_CUR);
		frame2 = (frame == mTmpBuf1) ? mTmpBuf2 : mTmpBuf1;
		FS_ReadFileAsync(&video, frame2, length + 4);

		uint8_t* lasty = mAsmData.Y[5];
		uint8_t* lastuv = mAsmData.UV[5];
		mAsmData.Y[5] = mAsmData.Y[4];
        mAsmData.UV[5] = mAsmData.UV[4];
		mAsmData.Y[4] = mAsmData.Y[3];
        mAsmData.UV[4] = mAsmData.UV[3];
		mAsmData.Y[3] = mAsmData.Y[2];
        mAsmData.UV[3] = mAsmData.UV[2];
		mAsmData.Y[2] = mAsmData.Y[1];
        mAsmData.UV[2] = mAsmData.UV[1];
		mAsmData.Y[1] = mAsmData.Y[0];
        mAsmData.UV[1] = mAsmData.UV[0];
		mAsmData.Y[0] = lasty;
		mAsmData.UV[0] = lastuv;

		mAsmData.Data = (uint8_t*)frame + 4;
		//DC_InvalidateRange(frame, 8192);//256 * 192 * 2);
		DecodeARMv5Stride256VXS2_Func(&mAsmData);

		framenr++;
		if(!(framenr % FRAME_RATE))//Once per second
		{
			FS_ReadFileAsync(&video, &mWaveData[mWaveDataOffs_write * AUDIO_BLOCK_SIZE], AUDIO_BLOCK_SIZE * 2);
			flushAudio = mWaveDataOffs_write;
			mWaveDataOffs_write = (mWaveDataOffs_write + 1) % NR_WAVE_DATA_BUFFERS;
		}

		while(nrFramesInQueue >= NR_FRAME_BLOCKS || lastQueueBlock == curBlock);

		//OSTick start = OS_GetTick();
		yuv2rgb_new(&mAsmData.Y[0][0], &mAsmData.UV[0][0], &mFrameQueue[lastQueueBlock * FRAME_SIZE]);
		DC_FlushRange(&mFrameQueue[lastQueueBlock * FRAME_SIZE], FRAME_SIZE * 2);//(uint16_t*)(HW_BG_VRAM + 96 * 1024 * lastQueueBlock));
		//OSTick diff = OS_GetTick() - start;
		//NOCASH_Printf("copy2vram: %d", diff);
		if(lastQueueBlock + 1 >= NR_FRAME_BLOCKS)
			lastQueueBlock = 0;
		else
			lastQueueBlock++;
		//lastQueueBlock = (lastQueueBlock + 1) % NR_FRAME_BLOCKS;
		nrFramesInQueue++;
	}
	FS_CloseFile(&video);
	while(!nowait && nrFramesInQueue > 0);
	GX_VBlankIntr(FALSE);
	OS_CancelAlarm(&mFrameAlarm);
	SND_StopTimer(3, 0, 0, 0);
	SND_FlushCommand(SND_COMMAND_NOBLOCK);
	GX_SetMasterBrightness(-16);
	GXS_SetMasterBrightness(-16);
	if (mVideoPlayerStartupParams.onvideofinishCallback)
		mVideoPlayerStartupParams.onvideofinishCallback();
	else
		while (1);
}
#pragma VIDEOPLAYER_OVERLAY_END