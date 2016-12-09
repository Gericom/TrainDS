#include <nitro.h>
#include <nnsys/g3d.h>
#include <nnsys/gfd.h>
#include "core.h"
#include "util.h"
#include "Menu.h"
#include "Depot.h"

static void rotEnvMap(NNSG3dRS* rs)
{
    //
    // Note that if the NNS_G3D_SBC_ENVMAP command is in the callback,
    // function, the matrix mode becomes GX_MTXMODE_TEXTURE.
    // The matrix mode must remain GX_MTXMODE_TEXTURE even after the
    // callback function has ended.
    //
    static MtxFx33 mRot = {
        FX32_ONE, 0, 0,
        0, FX32_ONE, 0,
        0, 0, FX32_ONE
    };
    MtxFx33 m;

	/*u16 keyData = PAD_Read();

	if (keyData & PAD_BUTTON_B)
    {
        MTX_RotZ33(&m, FX_SinIdx(256), FX_CosIdx(256));
        MTX_Concat33(&m, &mRot, &mRot);
    }

    if (keyData & PAD_BUTTON_Y)
    {
        MTX_RotY33(&m, FX_SinIdx(256), FX_CosIdx(256));
        MTX_Concat33(&m, &mRot, &mRot);
    }

    if (keyData & PAD_BUTTON_X)
    {
        MTX_RotX33(&m, FX_SinIdx(256), FX_CosIdx(256));
        MTX_Concat33(&m, &mRot, &mRot);
    }*/

    // By creating effect_mtx on your own you can change the direction in which the texture is projected.
    NNS_G3dGeMultMtx33(&mRot);

    // Flag is set because the created effect_mtx part been replaced.
    // When this is done, you can skip the internal library processes that would otherwise proceed between Timing B and Timing C.
    rs->flag |= NNS_G3D_RSFLAG_SKIP;
}

void Depot::Initialize(int arg)
{
	GX_SetBankForLCDC(GX_VRAM_LCDC_ALL);
	MI_CpuClearFast((void *)HW_LCDC_VRAM, HW_LCDC_VRAM_SIZE);
	(void)GX_DisableBankForLCDC();

	MI_CpuFillFast((void *)HW_OAM, 192, HW_OAM_SIZE);   // clear OAM
	MI_CpuClearFast((void *)HW_PLTT, HW_PLTT_SIZE);     // clear the standard palette

	MI_CpuFillFast((void *)HW_DB_OAM, 192, HW_DB_OAM_SIZE);     // clear OAM
	MI_CpuClearFast((void *)HW_DB_PLTT, HW_DB_PLTT_SIZE);       // clear the standard palette

	G3X_Init();
	G3X_InitMtxStack();
	GX_SetBankForTex(GX_VRAM_TEX_01_AB);
	GX_SetBankForTexPltt(GX_VRAM_TEXPLTT_0123_E);
   
   	GX_SetGraphicsMode(GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BG0_AS_3D);
	GX_SetVisiblePlane(GX_PLANEMASK_BG0);
   	GXS_SetGraphicsMode(GX_BGMODE_0);

	G3X_SetShading(GX_SHADING_TOON); 
	G3X_AntiAlias(TRUE);
	G3_SwapBuffers(GX_SORTMODE_AUTO, GX_BUFFERMODE_W);
	
	G3X_AlphaTest(FALSE, 0);                   // AlphaTest OFF
	G3X_AlphaBlend(TRUE);                      // AlphaTest ON

	G3X_SetClearColor(GX_RGB(119 >> 3, 199 >> 3, 244 >> 3),31, 0x7fff, 63, FALSE);
	G3_ViewPort(0, 0, 255, 191);

	GX_SetDispSelect(GX_DISP_SELECT_MAIN_SUB);

	NNS_GfdResetLnkTexVramState();
	NNS_GfdResetLnkPlttVramState();

	mEnvModel = (NNSG3dResFileHeader*)Util_LoadFileToBuffer("/data/locomotives/atsf_f7/low.nsbmd", NULL, FALSE);
	NNS_G3dResDefaultSetup(mEnvModel);
	NNSG3dResFileHeader* mEnvTextures = (NNSG3dResFileHeader*)Util_LoadFileToBuffer("/data/locomotives/atsf_f7/low.nsbtx", NULL, TRUE);
	NNS_G3dResDefaultSetup(mEnvTextures);
	NNSG3dResMdl* model = NNS_G3dGetMdlByIdx(NNS_G3dGetMdlSet(mEnvModel), 0);
	NNSG3dResTex* tex = NNS_G3dGetTex(mEnvTextures);
	NNS_G3dBindMdlSet(NNS_G3dGetMdlSet(mEnvModel), tex);
	NNS_G3dRenderObjInit(&mEnvRenderObj, model);
	NNS_FndFreeToExpHeap(gHeapHandle, mEnvTextures);
	NNS_G3dRenderObjSetCallBack(&mEnvRenderObj, &rotEnvMap, NULL, NNS_G3D_SBC_ENVMAP, NNS_G3D_SBC_CALLBACK_TIMING_B);
}

void Depot::Render()
{
	G3X_Reset();
	G3X_ResetMtxStack();
	NNS_G3dGlbPerspectiveW(FX32_SIN30, FX32_COS30, (256 * 4096 / 192), 1 * 4096, 512 * 4096, 40960);
	NNS_G3dGlbLightColor(GX_LIGHTID_0, GX_RGB(31,31,31));
	NNS_G3dGlbLightColor(GX_LIGHTID_1, GX_RGB(31,31,31));
	NNS_G3dGlbLightColor(GX_LIGHTID_2, GX_RGB(31,31,31));
	NNS_G3dGlbLightColor(GX_LIGHTID_3, GX_RGB(31,31,31));
	NNS_G3dGlbFlushP();
	VecFx32 pos;
	pos.x = 0;
	pos.y = 3 * FX32_ONE;
	pos.z = 11 * FX32_ONE;
	VecFx32 up;
	up.x = 0;
	up.y = FX32_ONE;
	up.z = 0;
	VecFx32 dst;
	dst.x = 0;
	dst.y = 2 * FX32_ONE;
	dst.z = 0;
	NNS_G3dGlbLookAt(&pos, &up, &dst);
	MtxFx33 mRot;
	u16 keyData = PAD_Read();
    if (keyData & PAD_BUTTON_A) sincos += 256;
    MTX_RotY33(&mRot, FX_SinIdx(sincos), FX_CosIdx(sincos));    
    NNS_G3dGlbSetBaseRot(&mRot);
	NNS_G3dGlbFlushP();
	NNS_G3dDraw(&mEnvRenderObj);

    MTX_Identity33(&mRot);
    NNS_G3dGlbSetBaseRot(&mRot);
	NNS_G3dGlbFlush();
    NNS_G3dGeFlushBuffer();
	G3_SwapBuffers(GX_SORTMODE_AUTO, GX_BUFFERMODE_W);
}

void Depot::VBlank()
{

}

void Depot::Finalize()
{
	//We don't have to free resources here, because that's done by using the group id
}