#include <nitro.h>
#include <nnsys/g3d.h>
#include <nnsys/gfd.h>
#include "core.h"
#include "util.h"
#include "Menu.h"
#include "Depot.h"
#include "TitleMenu.h"
#include "ui/layoutengine/behavior/ListRecyclerBehavior.h"
#include "DepotListAdapter.h"

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

	GX_SetBankForSubBG(GX_VRAM_SUB_BG_32_H);
	GX_SetBankForSubOBJ(GX_VRAM_SUB_OBJ_16_I);
	GXS_SetGraphicsMode(GX_BGMODE_0);
	GXS_SetVisiblePlane(GX_PLANEMASK_OBJ | GX_PLANEMASK_BG1);
   
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

	mEnvModel = (NNSG3dResFileHeader*)Util_LoadLZ77FileToBuffer("/data/wagons/atsf_f7/low.nsbmd.lz", NULL, FALSE);
	NNS_G3dResDefaultSetup(mEnvModel);
	NNSG3dResFileHeader* mEnvTextures = (NNSG3dResFileHeader*)Util_LoadLZ77FileToBuffer("/data/wagons/atsf_f7/low.nsbtx.lz", NULL, TRUE);
	NNS_G3dResDefaultSetup(mEnvTextures);
	NNSG3dResMdl* model = NNS_G3dGetMdlByIdx(NNS_G3dGetMdlSet(mEnvModel), 0);
	NNSG3dResTex* tex = NNS_G3dGetTex(mEnvTextures);
	NNS_G3dBindMdlSet(NNS_G3dGetMdlSet(mEnvModel), tex);
	NNS_G3dRenderObjInit(&mEnvRenderObj, model);
	NNS_FndFreeToExpHeap(gHeapHandle, mEnvTextures);
	//NNS_G3dRenderObjSetCallBack(&mEnvRenderObj, &rotEnvMap, NULL, NNS_G3D_SBC_ENVMAP, NNS_G3D_SBC_CALLBACK_TIMING_B);

	NNS_GfdInitVramTransferManager(mVramTransferTaskArray, DEPOT_VRAM_TRANSFER_MANAGER_NR_TASKS);

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

	mFontManager = new FontManager();

	mFontData = Util_LoadLZ77FileToBuffer("/data/fonts/fot_rodin_bokutoh_pro_db_9pt.NFTR.lz", NULL, false);
	MI_CpuClear8(&mFont, sizeof(mFont));
	NNS_G2dFontInitAuto(&mFont, mFontData);

	mFontManager->RegisterFont("rodin_db_9", &mFont);

	NNS_G2dInitOamManagerModule();
	mSubUIManager = new UIManager(UIManager::UI_MANAGER_SCREEN_SUB, &mImageProxy, &mImagePaletteProxy);

	void* cData = Util_LoadFileToBuffer("/data/menu/depot/Depot.ncer", NULL, FALSE);
	NNS_G2dGetUnpackedCellBank(cData, &mLayoutCellDataBank);

	void* mCharDataSub = Util_LoadFileToBuffer("/data/menu/depot/Depot.ncgr", NULL, TRUE);
	NNS_G2dGetUnpackedCharacterData(mCharDataSub, &charDataUnpacked);
	u32 objDataLength = charDataUnpacked->szByte;
	NNS_G2dInitImageProxy(&mImageProxy);
	NNS_G2dLoadImage1DMapping(charDataUnpacked, 0, NNS_G2D_VRAM_TYPE_2DSUB, &mImageProxy);
	NNS_FndFreeToExpHeap(gHeapHandle, mCharDataSub);

	void* mPalDataSub = Util_LoadFileToBuffer("/data/menu/depot/Depot.nclr", NULL, TRUE);
	NNS_G2dInitImagePaletteProxy(&mImagePaletteProxy);
	NNS_G2dGetUnpackedPaletteData(mPalDataSub, &palDataUnpacked);
	NNS_G2dLoadPalette(palDataUnpacked, 0, NNS_G2D_VRAM_TYPE_2DSUB, &mImagePaletteProxy);
	NNS_FndFreeToExpHeap(gHeapHandle, mPalDataSub);

	lyt_res_t* lytData = (lyt_res_t*)Util_LoadFileToBuffer("/data/menu/depot/Depot.bnlyt", NULL, TRUE);
	u32 objDataOffset = objDataLength;
	mLayoutTest = new Layout(lytData, Layout::LAYOUT_SCREEN_SUB, mLayoutCellDataBank, mFontManager, objDataOffset);
	NNS_FndFreeToExpHeap(gHeapHandle, lytData);

	ListRecyclerBehavior* listRecycler = new ListRecyclerBehavior();
	listRecycler->SetAdapter(new DepotListAdapter());
	mLayoutTest->FindPaneByName("List")->SetBehavior(listRecycler);

	mSubUIManager->AddLayout(mLayoutTest);

	G2S_SetWnd0Position(18, 10, 238, 152);
	G2S_SetWnd0InsidePlane(GX_WND_PLANEMASK_BG1 | GX_WND_PLANEMASK_OBJ, false);
	G2S_SetWndOutsidePlane(GX_WND_PLANEMASK_BG1, false);
	GXS_SetVisibleWnd(GX_WNDMASK_W0);
}

void Depot::Render()
{
	mSubUIManager->ProcessInput();
	Core_ReadInput();
	if (gKeys & PAD_BUTTON_B)
		TitleMenu::GotoMenu();

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
	mSubUIManager->Render();
}

void Depot::VBlank()
{
	mSubUIManager->ProcessVBlank();
	NNS_GfdDoVramTransfer();
}

void Depot::Finalize()
{
	//We don't have to free resources here, because that's done by using the group id
}