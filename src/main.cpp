#include <nitro.h>
#include "core.h"

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
	Core_Init();
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
	FX_Init();

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
}

void NitroMain ()
{
	Init();
	while(1);
}