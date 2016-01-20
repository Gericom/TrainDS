#include <nitro.h>
#include <nnsys/gfd.h>
#include "core.h"
#include "terrain/terrain.h"
#include "terrain/TerrainManager.h"

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
	G3X_Init();
	G3X_InitMtxStack();
	GX_SetBankForTex(GX_VRAM_TEX_01_AB);
	GX_SetBankForTexPltt(GX_VRAM_TEXPLTT_0123_E);
   
   	GX_SetGraphicsMode(GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BG0_AS_3D);
	GX_SetVisiblePlane(GX_PLANEMASK_BG0);
   	GXS_SetGraphicsMode(GX_BGMODE_0);
	//GXS_SetVisiblePlane(GX_PLANEMASK_BG1 | GX_PLANEMASK_OBJ);

	G3X_SetShading(GX_SHADING_TOON); 
	G3X_AntiAlias(TRUE);
	G3_SwapBuffers(GX_SORTMODE_AUTO, GX_BUFFERMODE_W);

	G3X_AlphaTest(FALSE, 0);                   // AlphaTest OFF
	G3X_AlphaBlend(TRUE);                      // AlphaTest ON

	G3X_SetClearColor(GX_RGB(119 >> 3, 199 >> 3, 244 >> 3),31, 0x7fff, 63, FALSE);
	G3_ViewPort(0, 0, 255, 191);

	uint32_t szWork = NNS_GfdGetLnkTexVramManagerWorkSize( 4096 );
    void* pMgrWork = NNS_FndAllocFromExpHeapEx(mHeapHandle, szWork, 16);
    NNS_GfdInitLnkTexVramManager(256 * 1024, 0, pMgrWork, szWork, TRUE);

	szWork = NNS_GfdGetLnkPlttVramManagerWorkSize( 4096 );
    pMgrWork = NNS_FndAllocFromExpHeapEx(mHeapHandle, szWork, 16);
    NNS_GfdInitLnkPlttVramManager(64 * 1024, pMgrWork, szWork, TRUE);

	gTerrainManager = new TerrainManager();

	OS_WaitVBlankIntr();

    GX_DispOn();
    GXS_DispOn();

	while(1)
	{
		G3X_Reset();
		G3_MtxMode(GX_MTXMODE_PROJECTION);
		{
			G3_Identity();
			G3_Perspective(FX32_SIN30, FX32_COS30, (256 * 4096 / 192), 1 * 4096, 512 * 4096, NULL);
			G3_StoreMtx(0);
		}
		G3_MtxMode(GX_MTXMODE_TEXTURE);
		{
			G3_Identity();
		}
		G3_MtxMode(GX_MTXMODE_POSITION_VECTOR);
		{
			G3_Identity();
			VecFx32 pos;
			pos.x = 8 * FX32_ONE;
			pos.y = 2 * FX32_ONE;
			pos.z = 8 * FX32_ONE;
			VecFx32 up;
			up.x = 0;
			up.y = 4096;
			up.z = 0;
			VecFx32 dst;
			dst.x = 4.5 * FX32_ONE;
			dst.y = 0;
			dst.z = 4.5 * FX32_ONE;
			
			G3_RotX(FX32_SIN45, FX32_COS45);
			G3_Translate(0, -2 * FX32_ONE, 0);
			//G3_LookAt(&pos, &up, &dst, NULL);
			G3_Translate(-8 * FX32_ONE, 0, -8 * FX32_ONE);
			tile_t dummyTile;
			dummyTile.y = 0;
			for(int y = 0; y < 16; y++)
			{
				for(int x = 0; x < 16; x++)
				{
					if(y == 5)//(x+y)&1)
						dummyTile.groundType = 1;
					else 
						dummyTile.groundType = 0;
					tile_render(&dummyTile);
					G3_Translate(FX32_ONE, 0, 0);
				}
				G3_Translate(-16 * FX32_ONE, 0, FX32_ONE);
			}
		}
		G3_SwapBuffers(GX_SORTMODE_AUTO, GX_BUFFERMODE_W);
		OS_WaitVBlankIntr();
	}
}