#include <nitro.h>
#include "core.h"
#include "util.h"
#include "videoplayer/Vx2Tables.h"
#include "videoplayer/DecodeARMv5Stride256VXS2.h"
#include "videoplayer/yuv2rgb_new.h"
#include "Menu.h"
#include "Game.h"
#include "VideoPlayer.h"

videoplayer_startup_params_t mVideoPlayerStartupParams;

#include <nitro/dtcm_begin.h>
bool videoPlayer_mWaveDataOffs_write = 0;
//FrameQueue implementation
volatile int videoPlayer_curBlock = -1;
volatile int videoPlayer_nrFramesInQueue = 0;
volatile int videoPlayer_firstQueueBlock = 0;//block to read from (most of the time (curBlock + 1) % 4)
volatile int videoPlayer_lastQueueBlock = 0;//block to write to (most of the time (firstQueueBlock + nrFramesInQueue) % 4) 

volatile int videoPlayer_mShouldCopyFrame = 0;

AsmData videoPlayer_mAsmData;
OSAlarm videoPlayer_mFrameAlarm;
uint16_t* videoPlayer_mFrameQueue;
#include <nitro/dtcm_end.h>

extern void VideoPlayer_VBlankIntr();
extern void VideoPlayer_frameHandler(void* arg);

#include <nitro/code16.h>
void VideoPlayer::Run(int arg)
{
	videoPlayer_curBlock = -1;
	videoPlayer_nrFramesInQueue = 0;
	videoPlayer_firstQueueBlock = 0;//block to read from (most of the time (curBlock + 1) % 4)
	videoPlayer_lastQueueBlock = 0;//block to write to (most of the time (firstQueueBlock + nrFramesInQueue) % 4) 
	videoPlayer_mShouldCopyFrame = 0;
	//load vx2_decoder overlay
	LOAD_OVERLAY_ITCM(videoplayer_itcm);

	GX_SetBankForLCDC(GX_VRAM_LCDC_ALL);
	MI_CpuClearFast((void*)HW_LCDC_VRAM, HW_LCDC_VRAM_SIZE);
	GX_DisableBankForLCDC();

	MI_CpuFillFast((void*)HW_OAM, 192, HW_OAM_SIZE);   // clear OAM
	MI_CpuClearFast((void*)HW_PLTT, HW_PLTT_SIZE);     // clear the standard palette

	MI_CpuFillFast((void*)HW_DB_OAM, 192, HW_DB_OAM_SIZE);     // clear OAM
	MI_CpuClearFast((void*)HW_DB_PLTT, HW_DB_PLTT_SIZE);       // clear the standard palette

	OS_SetIrqFunction(OS_IE_V_BLANK, VideoPlayer_VBlankIntr);
	//allocate wave buffers
	mWaveData = (int16_t*)NNS_FndAllocFromExpHeapEx(gHeapHandle, WAVE_DATA_BUFFER_LENGTH * 2, 32);
	//allocate framequeue
	videoPlayer_mFrameQueue = (uint16_t*)NNS_FndAllocFromExpHeapEx(gHeapHandle, FRAME_SIZE * NR_FRAME_BLOCKS * 2, 32);
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

	OS_CreateAlarm(&videoPlayer_mFrameAlarm);
	OS_SetPeriodicAlarm(&videoPlayer_mFrameAlarm, OS_GetTick() + OS_MilliSecondsToTicks(1), OS_MicroSecondsToTicks(1000000 / PLAY_RATE), &VideoPlayer_frameHandler, NULL);

	videoPlayer_mAsmData.Width = 256;
	videoPlayer_mAsmData.Height = 192;
	for(int i = 0; i < 6; i++)
	{
		videoPlayer_mAsmData.Y[i] = (uint8_t*)NNS_FndAllocFromExpHeapEx(gHeapHandle, videoPlayer_mAsmData.Height * 256, 16);
		videoPlayer_mAsmData.UV[i] = (uint8_t*)NNS_FndAllocFromExpHeapEx(gHeapHandle, videoPlayer_mAsmData.Height / 2 * 256, 16);
	}
	videoPlayer_mAsmData.Table0 = (void*)&Vx2Table0[0];
	videoPlayer_mAsmData.Table1 = (void*)&Vx2Table1[0];
	videoPlayer_mAsmData.MinMaxTable = &Vx2MinMaxTable[0];
	videoPlayer_mAsmData.Quantizer = 0;
	videoPlayer_mAsmData.YuvFormat = 0;
	MI_CpuClear8(&videoPlayer_mAsmData.Internal[0], sizeof(videoPlayer_mAsmData.Internal));
	Run_ITCM();
}
#include <nitro/codereset.h>