#include <nitro.h>
#include <nnsys/g3d.h>
#include <nnsys/gfd.h>
#include "core.h"
#include "util.h"
#include "Loader.h"

void Loader::BeginLoad()
{
	GX_SetBankForBG(GX_VRAM_BG_128_D);
	GX_SetBankForOBJ(GX_VRAM_OBJ_16_G);
	GX_SetBankForSubBG(GX_VRAM_SUB_BG_32_H);
	GX_SetBankForSubOBJ(GX_VRAM_SUB_OBJ_16_I);
	NNSG2dScreenData* screenDataUnpacked;
	void* screenData = Util_LoadFileToBuffer("/data/menu/menu_bg_bottom.NSCR", NULL, true);
	NNS_G2dGetUnpackedScreenData(screenData, &screenDataUnpacked);
	NNSG2dCharacterData* charDataUnpacked;
	void* charData = Util_LoadFileToBuffer("/data/menu/menu_bg_bottom.NCGR", NULL, true);
	NNS_G2dGetUnpackedCharacterData(charData, &charDataUnpacked);
	NNSG2dPaletteData* palDataUnpacked;
	void* palData = Util_LoadFileToBuffer("/data/menu/menu_bg_bottom.NCLR", NULL, true);
	NNS_G2dGetUnpackedPaletteData(palData, &palDataUnpacked);
	NNS_G2dBGSetup(NNS_G2D_BGSELECT_SUB1, screenDataUnpacked, charDataUnpacked, palDataUnpacked, GX_BG_SCRBASE_0x6000, GX_BG_CHARBASE_0x00000);
	NNS_FndFreeToExpHeap(gHeapHandle, screenData);
	NNS_FndFreeToExpHeap(gHeapHandle, charData);
	NNS_FndFreeToExpHeap(gHeapHandle, palData);

	screenData = Util_LoadFileToBuffer("/data/menu/loader_bg_top1.NSCR", NULL, true);
	NNS_G2dGetUnpackedScreenData(screenData, &screenDataUnpacked);
	charData = Util_LoadLZ77FileToBuffer("/data/menu/loader_bg_top1.NCGR.lz", NULL, true);
	NNS_G2dGetUnpackedCharacterData(charData, &charDataUnpacked);
	palData = Util_LoadFileToBuffer("/data/menu/loader_bg_top1.NCLR", NULL, true);
	NNS_G2dGetUnpackedPaletteData(palData, &palDataUnpacked);
	NNS_G2dBGSetup(NNS_G2D_BGSELECT_MAIN1, screenDataUnpacked, charDataUnpacked, palDataUnpacked, GX_BG_SCRBASE_0xc000, GX_BG_CHARBASE_0x00000);
	NNS_FndFreeToExpHeap(gHeapHandle, screenData);
	NNS_FndFreeToExpHeap(gHeapHandle, charData);
	NNS_FndFreeToExpHeap(gHeapHandle, palData);

	GX_SetGraphicsMode(GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BG0_AS_3D);
	GX_SetVisiblePlane(GX_PLANEMASK_BG1);
	GXS_SetGraphicsMode(GX_BGMODE_0);
	GXS_SetVisiblePlane(GX_PLANEMASK_BG1);
	GX_SetDispSelect(GX_DISP_SELECT_MAIN_SUB);

	OS_CreateThread(&mLoaderThread, LoaderThreadMain, this, mLoaderThreadStack + LOADER_WORKER_THREAD_STACK_SIZE / sizeof(u32), LOADER_WORKER_THREAD_STACK_SIZE, LOADER_WORKER_THREAD_PRIORITY);
	OS_WakeupThreadDirect(&mLoaderThread);
}

void Loader::FinishLoad()
{
	mShouldFinish = true;
}

#define LOADER_FADE_LENGTH	17

void Loader::LoaderThreadMain()
{
	int brightness;
	mState = LOADER_STATE_FADE_IN;
	mStateCounter = 0;
	while (true)
	{
		switch (mState)
		{
		case LOADER_STATE_FADE_IN:
			break;
		case LOADER_STATE_LOOP:
			break;
		case LOADER_STATE_FADE_OUT:
			break;
		}
		NNS_SndMain();
		NNS_SndUpdateDriverInfo();
		OS_WaitVBlankIntr();
		switch (mState)
		{
		case LOADER_STATE_FADE_IN:
			brightness = -16 + ((0 - -16) * mStateCounter) / (LOADER_FADE_LENGTH - 1);
			GX_SetMasterBrightness(brightness);
			GXS_SetMasterBrightness(brightness);
			mStateCounter++;
			if (mStateCounter == LOADER_FADE_LENGTH)
			{
				mStateCounter = 0;
				mState = LOADER_STATE_LOOP;
			}
			break;
		case LOADER_STATE_LOOP:
			if (mShouldFinish)
			{
				mStateCounter = 0;
				mState = LOADER_STATE_FADE_OUT;
			}
			break;
		case LOADER_STATE_FADE_OUT:
			brightness = 0 + ((-16 - 0) * mStateCounter) / (LOADER_FADE_LENGTH - 1);
			GX_SetMasterBrightness(brightness);
			GXS_SetMasterBrightness(brightness);
			mStateCounter++;
			if (mStateCounter == LOADER_FADE_LENGTH)
			{
				goto end;
			}
			break;
		}
	}
end:
	Util_Reset2D(false);
	mState = LOADER_STATE_FINISHED;
}