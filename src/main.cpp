#include <nitro.h>
#include <nnsys/g3d.h>
#include <nnsys/gfd.h>
#include "core.h"
#include "util.h"
#include "terrain/TerrainManager.h"
#include "menu/TitleMenu.h"
#include "menu/Depot.h"
#include "menu/Game.h"

#define	DEFAULT_DMA_NUMBER		MI_DMA_MAX_NUM

static void ExceptionHandler(u32 ptr, void* arg)
{
	OSContext* context = (OSContext*)ptr;
	uint32_t pc = context->pc_plus4;
	uint32_t lr = context->lr;
	NOCASH_Break();
	while(1);
}

void NitroStartUp()
{
	OS_Init();
	Core_PreInit();
}

static void VBlankIntr(void)
{
	OS_SetIrqCheckFlag(OS_IE_V_BLANK); // checking V-Blank interrupt
}

static void Init()
{
	TPCalibrateParam calibrate;
	//Done in NitroStartup
	//OS_Init();
	OS_InitTick();
	OS_InitThread();
	TP_Init();
	TP_GetUserInfo(&calibrate);
	TP_SetCalibrateParam(&calibrate);
	GX_Init();

	GX_DispOff();
	GXS_DispOff();

	OS_SetIrqFunction(OS_IE_V_BLANK, VBlankIntr);
	OS_EnableIrqMask(OS_IE_V_BLANK);
	OS_EnableIrq();

	FS_Init(DEFAULT_DMA_NUMBER);

	GX_VBlankIntr(TRUE);         // to generate V-Blank interrupt request

	GX_SetBankForLCDC(GX_VRAM_LCDC_ALL);
	MI_CpuClearFast((void *)HW_LCDC_VRAM, HW_LCDC_VRAM_SIZE);
	GX_DisableBankForLCDC();

	MI_CpuFillFast((void *)HW_OAM, 192, HW_OAM_SIZE);   // clear OAM
	MI_CpuClearFast((void *)HW_PLTT, HW_PLTT_SIZE);     // clear the standard palette

	MI_CpuFillFast((void *)HW_DB_OAM, 192, HW_DB_OAM_SIZE);     // clear OAM
	MI_CpuClearFast((void *)HW_DB_PLTT, HW_DB_PLTT_SIZE);       // clear the standard palette

	//CARD_Init();
	//card_lock_id = (u16)OS_GetLockID();

	Core_Init();
	NNS_G3dInit();
}

void NitroMain ()
{
	Init();

	OS_WaitVBlankIntr();

	GX_SetMasterBrightness(-16);
	GXS_SetMasterBrightness(-16);
    GX_DispOn();
    GXS_DispOn();

	NNS_FndSetGroupIDForExpHeap(mHeapHandle, 0x5A);//This is to be able to simply free all resources used by the menu after it is closed
	//Game loop
	//Should handle switching menu's and deallocating the shit they didn't (and don't have to) clean up
	while(1)
	{
		Menu* menu = new Game();
		menu->Initialize(0);////TITLEMENU_ARG_PLAY_INTRO);///*0);//*/TITLEMENU_ARG_DONT_PLAY_INTRO);
		//Fade in
		for(int i = -16; i <= 0; i++)
		{
			menu->Render();
			NNS_SndMain();
			OS_WaitVBlankIntr();
			menu->VBlank();
			GX_SetMasterBrightness(i);
			GXS_SetMasterBrightness(i);
		}
		while(1)
		{
			menu->Render();
			NNS_SndMain();
			OS_WaitVBlankIntr();
			menu->VBlank();
		}
		//Fade out
		for(int i = 0; i >= -16; i--)
		{
			menu->Render();
			NNS_SndMain();
			OS_WaitVBlankIntr();
			menu->VBlank();
			GX_SetMasterBrightness(i);
			GXS_SetMasterBrightness(i);
		}
		menu->Finalize();
		Util_FreeAllToExpHeapByGroupId(mHeapHandle, 0x5A);//Release everything allocated by the menu
	}
}