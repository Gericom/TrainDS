#include <nitro.h>
#include <nnsys/g3d.h>
#include "core.h"
#include "util.h"
#include "Menu.h"
#include "../terrain/terrain.h"
#include "../terrain/TerrainManager.h"
#include "../engine/PathWorker.h"
#include "../vehicles/train.h"
#include "Game.h"

static tile_t sDummyMap[16][16];
static trackpiece_t sDummyPieces[8];

static const GXRgb sEdgeMarkingColorTable[8] =
{
	GX_RGB(12, 27, 31),
	0, 0, 0, 0, 0, 0, 0
};

static const GXRgb sDiffSelectionColorTable[8] =
{
	GX_RGB(16, 20, 21),
	0, 0, 0, 0, 0, 0, 0
};

static const GXRgb sAmbSelectionColorTable[8] =
{
	GX_RGB(11, 14, 15),
	0, 0, 0, 0, 0, 0, 0
};

void Game::Initialize(int arg)
{
	GX_SetBankForLCDC(GX_VRAM_LCDC_ALL);
	MI_CpuClearFast((void *)HW_LCDC_VRAM, HW_LCDC_VRAM_SIZE);
	(void)GX_DisableBankForLCDC();

	MI_CpuFillFast((void *)HW_OAM, 192, HW_OAM_SIZE);   // clear OAM
	MI_CpuClearFast((void *)HW_PLTT, HW_PLTT_SIZE);     // clear the standard palette

	MI_CpuFillFast((void *)HW_DB_OAM, 192, HW_DB_OAM_SIZE);     // clear OAM
	MI_CpuClearFast((void *)HW_DB_PLTT, HW_DB_PLTT_SIZE);       // clear the standard palette

	//GX_SetBankForLCDC(GX_VRAM_LCDC_C);

	G3X_Init();
	G3X_InitMtxStack();
	GX_SetBankForTex(GX_VRAM_TEX_01_AB);
	GX_SetBankForTexPltt(GX_VRAM_TEXPLTT_0123_E);
   
   	GX_SetGraphicsMode(GX_DISPMODE_GRAPHICS, GX_BGMODE_3, GX_BG0_AS_3D);
	GX_SetVisiblePlane(GX_PLANEMASK_BG0);
   	GXS_SetGraphicsMode(GX_BGMODE_0);

	G3X_SetShading(GX_SHADING_TOON); 
	G3X_AntiAlias(TRUE);
	G3_SwapBuffers(GX_SORTMODE_AUTO, GX_BUFFERMODE_W);
	
	G3X_AlphaTest(FALSE, 0);                   // AlphaTest OFF
	G3X_AlphaBlend(TRUE);                      // AlphaTest ON
	G3X_EdgeMarking(TRUE);

	G3X_SetEdgeColorTable(&sEdgeMarkingColorTable[0]);

	G3X_SetClearColor(GX_RGB(119 >> 3, 199 >> 3, 244 >> 3),31, 0x7fff, 0, FALSE);
	G3_ViewPort(0, 0, 255, 191);

	GX_SetDispSelect(GX_DISP_SELECT_SUB_MAIN);

	NNS_GfdResetLnkTexVramState();
	NNS_GfdResetLnkPlttVramState();

	mTerrainManager = new TerrainManager();

	MI_CpuClearFast(&sDummyMap[0][0], sizeof(sDummyMap));

	for(int y = 0; y < 16; y++)
	{
		for(int x = 0; x < 16; x++)
		{
			if(y == 5 && x > 6 && x < 8)
			{
				sDummyMap[y][x].groundType = 0;//1;
				sDummyMap[y][x].y = 1;
			}
			else if(y == 5 && x > 6 && x > 8) sDummyMap[y][x].groundType = 0;//1;
			else if(y == 6 && x > 6 && x < 8)
			{
				sDummyMap[y][x].ltCorner = TILE_CORNER_UP;
				sDummyMap[y][x].rtCorner = TILE_CORNER_UP;
				sDummyMap[y][x].lbCorner = TILE_CORNER_FLAT;
				sDummyMap[y][x].rbCorner = TILE_CORNER_FLAT;
			}
			else if(y == 4 && x > 6  && x < 8)
			{
				sDummyMap[y][x].ltCorner = TILE_CORNER_FLAT;
				sDummyMap[y][x].rtCorner = TILE_CORNER_FLAT;
				sDummyMap[y][x].lbCorner = TILE_CORNER_UP;
				sDummyMap[y][x].rbCorner = TILE_CORNER_UP;
			}
		}
	}
	sDummyMap[5][8].ltCorner = TILE_CORNER_UP;
	sDummyMap[5][8].lbCorner = TILE_CORNER_UP;
	sDummyMap[5][8].groundType = 0;//1;
	sDummyMap[4][8].lbCorner = TILE_CORNER_UP;
	sDummyMap[6][8].ltCorner = TILE_CORNER_UP;
	sDummyMap[6][6].rtCorner = TILE_CORNER_UP;
	sDummyMap[4][6].rbCorner = TILE_CORNER_UP;
	sDummyMap[5][6].rtCorner = TILE_CORNER_UP;
	sDummyMap[5][6].rbCorner = TILE_CORNER_UP;
	sDummyPieces[0].kind = TRACKPIECE_KIND_FLAT;
	sDummyPieces[0].rot = TRACKPIECE_ROT_0;
	sDummyPieces[0].x = 7;
	sDummyPieces[0].y = 0;
	sDummyPieces[0].z = 7;
	sDummyPieces[1].kind = TRACKPIECE_KIND_FLAT_SMALL_CURVED_LEFT;
	sDummyPieces[1].rot = TRACKPIECE_ROT_0;
	sDummyPieces[1].x = 8;
	sDummyPieces[1].y = 0;
	sDummyPieces[1].z = 7;
	sDummyPieces[2].kind = TRACKPIECE_KIND_FLAT;
	sDummyPieces[2].rot = TRACKPIECE_ROT_90;
	sDummyPieces[2].x = 9;
	sDummyPieces[2].y = 0;
	sDummyPieces[2].z = 5;
	sDummyPieces[3].kind = TRACKPIECE_KIND_FLAT_SMALL_CURVED_LEFT;
	sDummyPieces[3].rot = TRACKPIECE_ROT_90;
	sDummyPieces[3].x = 9;
	sDummyPieces[3].y = 0;
	sDummyPieces[3].z = 4;
	sDummyPieces[4].kind = TRACKPIECE_KIND_FLAT;
	sDummyPieces[4].rot = TRACKPIECE_ROT_180;
	sDummyPieces[4].x = 7;
	sDummyPieces[4].y = 0;
	sDummyPieces[4].z = 3;
	sDummyPieces[5].kind = TRACKPIECE_KIND_FLAT_SMALL_CURVED_LEFT;
	sDummyPieces[5].rot = TRACKPIECE_ROT_180;
	sDummyPieces[5].x = 6;
	sDummyPieces[5].y = 0;
	sDummyPieces[5].z = 3;
	sDummyPieces[6].kind = TRACKPIECE_KIND_FLAT;
	sDummyPieces[6].rot = TRACKPIECE_ROT_270;
	sDummyPieces[6].x = 5;
	sDummyPieces[6].y = 0;
	sDummyPieces[6].z = 5;
	sDummyPieces[7].kind = TRACKPIECE_KIND_FLAT_SMALL_CURVED_LEFT;
	sDummyPieces[7].rot = TRACKPIECE_ROT_270;
	sDummyPieces[7].x = 5;
	sDummyPieces[7].y = 0;
	sDummyPieces[7].z = 6;

	for(int i = 0; i < 8; i++)
	{
		sDummyPieces[i].prev[0] = &sDummyPieces[(i == 0) ? 7 : (i - 1)];
		sDummyPieces[i].prev[1] = sDummyPieces[i].prev[2] = sDummyPieces[i].prev[3] = NULL;
		sDummyPieces[i].next[0] = &sDummyPieces[(i == 7) ? 0 : (i + 1)];
		sDummyPieces[i].next[1] = sDummyPieces[i].next[2] = sDummyPieces[i].next[3] = NULL;
	}

	mTrain.firstPart = &mTrainPart;
	mTrain.isDriving = FALSE;
	mTrain.firstPart->pathWorker1 = new PathWorker(&sDummyPieces[0], 0);
	mTrain.firstPart->pathWorker2 = new PathWorker(&sDummyPieces[0], FX32_ONE);
	mTrain.firstPart->next = NULL;

	mLocModel = (NNSG3dResFileHeader*)Util_LoadFileToBuffer("/data/locomotives/atsf_f7/low.nsbmd", NULL);
	NNS_G3dResDefaultSetup(mLocModel);
	NNSG3dResFileHeader* mLocTextures = (NNSG3dResFileHeader*)Util_LoadFileToBuffer("/data/locomotives/atsf_f7/low.nsbtx", NULL);
	NNS_G3dResDefaultSetup(mLocTextures);
	NNSG3dResMdl* model = NNS_G3dGetMdlByIdx(NNS_G3dGetMdlSet(mLocModel), 0);
	NNS_G3dMdlSetMdlLightEnableFlagAll(model, GX_LIGHTMASK_0);
	NNS_G3dMdlSetMdlDiffAll(model, GX_RGB(21,21,21));
	NNS_G3dMdlSetMdlAmbAll(model, GX_RGB(15,15,15));
	NNS_G3dMdlSetMdlSpecAll(model, GX_RGB(0,0,0));
	NNS_G3dMdlSetMdlEmiAll(model, GX_RGB(0,0,0));
	NNSG3dResTex* tex = NNS_G3dGetTex(mLocTextures);
	NNS_G3dBindMdlSet(NNS_G3dGetMdlSet(mLocModel), tex);
	NNS_G3dRenderObjInit(&mTrain.firstPart->renderObj, model);
	NNS_FndFreeToExpHeap(mHeapHandle, mLocTextures);

	NNS_SndArcLoadSeqArc(SEQ_TRAIN, mSndHeapHandle);
	NNS_SndArcLoadBank(BANK_TRAIN, mSndHeapHandle);
	NNS_SndArcPlayerStartSeqArc(&mTrain.trackSoundHandle, SEQ_TRAIN, TRAIN_TRACK);

	mLastTouchState = 0;
	mPicking = FALSE;
	mPickingOK = FALSE;
	mProcessPicking = FALSE;
}

void Game::Render()
{
	if(mPicking)
	{
		mProcessPicking = TRUE;
		mPicking = FALSE;
	}
	if(mProcessPicking)//process picking result
	{
		uint16_t val = ((uint16_t*)HW_LCDC_VRAM_D)[mPickingPoint.x + mPickingPoint.y * 256];
		mPickingOK = (val & 0x7FFF) == 1;
	}
	G3X_Reset();
	G3X_ResetMtxStack();
	NNS_G3dGlbPerspectiveW(FX32_SIN30, FX32_COS30, (256 * 4096 / 192), 1 * 4096, 512 * 4096, 40960);
	u16 keyData = PAD_Read();
	TPData raw_point;
    TPData disp_point;
	while (TP_RequestRawSampling(&raw_point) != 0);
	if (keyData & PAD_BUTTON_A) mTrain.isDriving = TRUE;
	else mTrain.isDriving = FALSE;
    TP_GetCalibratedPoint(&disp_point, &raw_point);
	if(disp_point.touch && !mLastTouchState)//we should do picking
	{
		mPicking = TRUE;
		mPickingPoint = disp_point;
		reg_G3X_DISP3DCNT = reg_G3X_DISP3DCNT & ~REG_G3X_DISP3DCNT_TME_MASK;
		G3X_SetClearColor(0, 31, 0x7fff, 0, FALSE);
		G3X_EdgeMarking(FALSE);
		G3X_AntiAlias(FALSE);
	}
	else
	{
		reg_G3X_DISP3DCNT = reg_G3X_DISP3DCNT | REG_G3X_DISP3DCNT_TME_MASK;
		G3X_SetClearColor(GX_RGB(119 >> 3, 199 >> 3, 244 >> 3),31, 0x7fff, 0, FALSE);
		G3X_EdgeMarking(TRUE);
		G3X_AntiAlias(TRUE);
	}
	mLastTouchState = disp_point.touch;
	Train_UpdatePos(&mTrain);
//#define FIRST_PERSON
#ifdef FIRST_PERSON
	VecFx32 pos;
	pos.x = tpos.x - 8 * FX32_ONE;// + 0.4 * dir.x;
	pos.y = tpos.y + FX32_HALF + (FX32_HALF >> 1);// + 0.4 * dir.y;
	pos.z = tpos.z - 8 * FX32_ONE;// + 0.4 * dir.z;
	VecFx32 up;
	up.x = 0;
	up.y = FX32_ONE;
	up.z = 0;
	VecFx32 dst;
	dst.x = tpos.x - 8 * FX32_ONE + 4 * dir.x;
	dst.y = tpos.y + FX32_HALF + 4 * dir.y;// + 1 * FX32_ONE;
	dst.z = tpos.z - 8 * FX32_ONE + 4 * dir.z;
#else
//#define TOP_VIEW
#ifndef TOP_VIEW
	VecFx32 pos;
	pos.x = 3 * FX32_ONE;
	pos.y = 2.25 * FX32_ONE;
	pos.z = -0.75 * FX32_ONE;
	VecFx32 up;
	up.x = 0;
	up.y = FX32_ONE;
	up.z = 0;
	VecFx32 dst;
	dst.x = 0 * FX32_ONE;
	dst.y = 0;
	dst.z = -2 * FX32_ONE;
#else
	VecFx32 pos;
	pos.x = -0.5 * FX32_ONE;
	pos.y = 5 * FX32_ONE;
	pos.z = -2.5 * FX32_ONE;
	VecFx32 up;
	up.x = 0;
	up.y = 0;
	up.z = FX32_ONE;
	VecFx32 dst;
	dst.x = -0.5 * FX32_ONE;
	dst.y = 0;
	dst.z = -2.5 * FX32_ONE;
#endif
#endif
	NNS_G3dGlbLookAt(&pos, &up, &dst);

	Train_UpdateSound(&mTrain, &pos, &dst, &up);

	NNS_G3dGlbPolygonAttr(GX_LIGHTMASK_0, GX_POLYGONMODE_MODULATE, GX_CULL_BACK, 0, 31, GX_POLYGON_ATTR_MISC_NONE);
	NNS_G3dGlbLightVector(GX_LIGHTID_0, -2048, -2897, -2048);
	NNS_G3dGlbLightColor(GX_LIGHTID_0, GX_RGB(31,31,31));
	if(mPicking)
	{
		NNS_G3dGlbMaterialColorDiffAmb(GX_RGB(0,0,0), GX_RGB(0,0,0), FALSE);
		NNS_G3dGlbMaterialColorSpecEmi(GX_RGB(0,0,0), GX_RGB(0,0,0), FALSE);
	}
	else
	{
		NNS_G3dGlbMaterialColorDiffAmb(GX_RGB(21,21,21), GX_RGB(15,15,15), FALSE);
		NNS_G3dGlbMaterialColorSpecEmi(GX_RGB(0,0,0), GX_RGB(0,0,0), FALSE);
	}
	NNS_G3dGlbFlushP();
	G3_Translate(-8 * FX32_ONE, 0, -8 * FX32_ONE);
	G3_PushMtx();
	{
		for(int y = 0; y < 16; y++)
		{
			G3_PushMtx();
			{
				for(int x = 0; x < 16; x++)
				{
					tile_render(&sDummyMap[y][x], mTerrainManager);
					G3_Translate(FX32_ONE, 0, 0);
				}
			}
			G3_PopMtx(1);
			G3_Translate(0, 0, FX32_ONE);
		}
	}
	G3_PopMtx(1);
	for(int i = 0; i < 8; i++)
	{
		trackpiece_render(&sDummyPieces[i], mTerrainManager);
	}
	NNS_G3dGePushMtx();
	{
		NNS_G3dGeTranslateVec(&mTrain.firstPart->position);
		//calculate rotation matrix
		VecFx32 up = {0, FX32_ONE, 0};
		VecFx32 cam = {0,0,0};
		VecFx32 dir = mTrain.firstPart->direction;
		dir.z = -dir.z;

		MtxFx43 rot2;
		MTX_LookAt(&cam, &up, &dir, &rot2);
		NNS_G3dGeMultMtx43(&rot2);

		NNS_G3dGeMtxMode(GX_MTXMODE_POSITION);
		NNS_G3dGeScale(FX32_ONE / 7, FX32_ONE / 7, FX32_ONE / 7);
		NNS_G3dGeMtxMode(GX_MTXMODE_POSITION_VECTOR);

		if(mPicking)
		{
			NNS_G3dMdlSetMdlDiffAll(mTrain.firstPart->renderObj.resMdl, GX_RGB(0,0,0));
			NNS_G3dMdlSetMdlAmbAll(mTrain.firstPart->renderObj.resMdl, GX_RGB(0,0,0));
			NNS_G3dMdlSetMdlSpecAll(mTrain.firstPart->renderObj.resMdl, GX_RGB(0,0,0));
			NNS_G3dMdlSetMdlEmiAll(mTrain.firstPart->renderObj.resMdl, 1);
		}
		else
		{
			if(mPickingOK) NNS_G3dMdlSetMdlDiffAll(mTrain.firstPart->renderObj.resMdl, sDiffSelectionColorTable[0]);
			else NNS_G3dMdlSetMdlDiffAll(mTrain.firstPart->renderObj.resMdl, GX_RGB(21,21,21));
			if(mPickingOK) NNS_G3dMdlSetMdlAmbAll(mTrain.firstPart->renderObj.resMdl, sAmbSelectionColorTable[0]);
			else NNS_G3dMdlSetMdlAmbAll(mTrain.firstPart->renderObj.resMdl, GX_RGB(15,15,15));
			NNS_G3dMdlSetMdlSpecAll(mTrain.firstPart->renderObj.resMdl, GX_RGB(0,0,0));
			NNS_G3dMdlSetMdlEmiAll(mTrain.firstPart->renderObj.resMdl, GX_RGB(0,0,0));
			if(mPickingOK) NNS_G3dMdlSetMdlPolygonIDAll(mTrain.firstPart->renderObj.resMdl, 1);
			else NNS_G3dMdlSetMdlPolygonIDAll(mTrain.firstPart->renderObj.resMdl, 0);
		}

		NNS_G3dDraw(&mTrain.firstPart->renderObj);
	}
	NNS_G3dGePopMtx(1);
	NNS_G3dGeFlushBuffer();
	G3_SwapBuffers(GX_SORTMODE_AUTO, GX_BUFFERMODE_W);
}

void Game::VBlank()
{
	if(mPicking)
	{
		GX_SetBankForBG(GX_VRAM_BG_128_C);
		G2_SetBG3ControlDCBmp(GX_BG_SCRSIZE_DCBMP_256x256, GX_BG_AREAOVER_XLU, GX_BG_BMPSCRBASE_0x00000);
		GX_SetVisiblePlane(GX_PLANEMASK_BG3);
		GX_SetBankForLCDC(GX_VRAM_LCDC_D);
		//Capture the picking data
		GX_SetCapture(GX_CAPTURE_SIZE_256x192, GX_CAPTURE_MODE_A, GX_CAPTURE_SRCA_3D, (GXCaptureSrcB)0, GX_CAPTURE_DEST_VRAM_D_0x00000, 16, 0);
	}
	else
	{
		GX_SetBankForLCDC(GX_VRAM_LCDC_C);
		GX_SetVisiblePlane(GX_PLANEMASK_BG0);
		//capture to be able to react as fast as possible on a touch (we use this image the hide the picking)
		GX_SetCapture(GX_CAPTURE_SIZE_256x192, GX_CAPTURE_MODE_A, GX_CAPTURE_SRCA_3D, (GXCaptureSrcB)0, GX_CAPTURE_DEST_VRAM_C_0x00000, 16, 0);
	}
}

void Game::Finalize()
{
	delete mTrain.firstPart->pathWorker1;
	delete mTrain.firstPart->pathWorker2;
	delete mTerrainManager;
	delete mLocModel;
}