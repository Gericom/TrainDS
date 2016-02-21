#include <nitro.h>
#include <nnsys/g3d.h>
#include <nnsys/gfd.h>
#include "core.h"
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
	//Done in NitroStartup
	//OS_Init();
	OS_InitTick();
	OS_InitThread();
	GX_Init();

	GX_DispOff();
	GXS_DispOff();

	OS_SetIrqFunction(OS_IE_V_BLANK, VBlankIntr);
	(void)OS_EnableIrqMask(OS_IE_V_BLANK);
	(void)OS_EnableIrq();

	FS_Init(DEFAULT_DMA_NUMBER);

	(void)GX_VBlankIntr(TRUE);         // to generate V-Blank interrupt request

	GX_SetBankForLCDC(GX_VRAM_LCDC_ALL);
	MI_CpuClearFast((void *)HW_LCDC_VRAM, HW_LCDC_VRAM_SIZE);
	(void)GX_DisableBankForLCDC();

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

    GX_DispOn();
    GXS_DispOn();

	Menu* menu = new Game();//TitleMenu();//Game();
	menu->Initialize(0);////TITLEMENU_ARG_PLAY_INTRO);///*0);//*/TITLEMENU_ARG_DONT_PLAY_INTRO);
	while(1)
	{
		menu->Render();
		NNS_SndMain();
		OS_WaitVBlankIntr();
		menu->VBlank();
	}
}