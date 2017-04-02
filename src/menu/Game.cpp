#include <nitro.h>
#include <nnsys/g3d.h>
#include <stdlib.h>
#include "core.h"
#include "util.h"
#include "Menu.h"
#include "terrain/GameController.h"
#include "terrain/TerrainManager.h"
#include "terrain/track/TrackPieceEx.h"
#include "terrain/track/FlexTrack.h"
#include "terrain/scenery/RCT2Tree1.h"
#include "terrain/managers/TerrainTextureManager16.h"
#include "terrain/managers/TerrainTextureManager8.h"
#include "engine/PathWorker.h"
#include "ui/UIManager.h"
#include "engine/Camera.h"
#include "engine/LookAtCamera.h"
#include "engine/ThirdPersonCamera.h"
#include "engine/FreeRoamCamera.h"
#include "inih/INIReader.h"
#include "tools/DragTool.h"
#include "tools/AddTrackTool.h"
#include "vehicles/Wagon.h"
#include "Game.h"

#define SWAP_BUFFERS_SORTMODE	GX_SORTMODE_MANUAL //AUTO
#define SWAP_BUFFERS_BUFFERMODE	GX_BUFFERMODE_Z

//static tile_t sDummyMap[64][64];
//static trackpiece_t sDummyPieces[8];
static TrackPieceEx* sDummyPieces[120];//10];

static const GXRgb sEdgeMarkingColorTable[8] =
{
	GX_RGB(31, 27, 12),
	GX_RGB(12, 27, 31),
	0, 0, 0, 0, 0, 0
};

static const GXRgb sDiffSelectionColorTable[8] =
{
	GX_RGB(21, 20, 16),
	GX_RGB(16, 20, 21),
	0, 0, 0, 0, 0, 0
};

static const GXRgb sAmbSelectionColorTable[8] =
{
	GX_RGB(15, 14, 11),
	GX_RGB(11, 14, 15),
	0, 0, 0, 0, 0, 0
};

static const GXRgb sToonTable[32] =
{
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0,
	GX_RGB(31,31,31),
	GX_RGB(4, 4, 4)
};

//Camera tempoarly
//#define FIRST_PERSON
//#define TOP_VIEW

#define VIEW_ANGLE_SIN	FX32_SIN30
#define VIEW_ANGLE_COS	FX32_COS30

void Game::Initialize(int arg)
{
	//load overlay
	LOAD_OVERLAY_ITCM(rendering_itcm);

	GX_SetBankForLCDC(GX_VRAM_LCDC_ALL);
	MI_CpuClearFast((void *)HW_LCDC_VRAM, HW_LCDC_VRAM_SIZE);
	(void)GX_DisableBankForLCDC();

	MI_CpuFillFast((void *)HW_OAM, 192, HW_OAM_SIZE);   // clear OAM
	MI_CpuClearFast((void *)HW_PLTT, HW_PLTT_SIZE);     // clear the standard palette

	MI_CpuFillFast((void *)HW_DB_OAM, 192, HW_DB_OAM_SIZE);     // clear OAM
	MI_CpuClearFast((void *)HW_DB_PLTT, HW_DB_PLTT_SIZE);       // clear the standard palette

	//GX_SetBankForLCDC(GX_VRAM_LCDC_C);

	GX_SetBankForLCDC(GX_VRAM_LCDC_D);

	GX_SetBankForOBJ(GX_VRAM_OBJ_16_F);

	GX_SetBankForSubBG(GX_VRAM_SUB_BG_32_H);
	GX_SetBankForSubOBJ(GX_VRAM_SUB_OBJ_16_I);

	G3X_Init();
	G3X_InitMtxStack();
	GX_SetBankForTex(GX_VRAM_TEX_012_ABC);
	GX_SetBankForTexPltt(GX_VRAM_TEXPLTT_0123_E);

	GX_SetGraphicsMode(GX_DISPMODE_GRAPHICS, GX_BGMODE_3, GX_BG0_AS_3D);
	GX_SetVisiblePlane(GX_PLANEMASK_BG0 | GX_PLANEMASK_OBJ);
	GXS_SetGraphicsMode(GX_BGMODE_3);
	GXS_SetVisiblePlane(GX_PLANEMASK_BG0 | GX_PLANEMASK_BG3 | GX_PLANEMASK_OBJ);
	G2_SetBG0Priority(3);
	G2_SetBG3Priority(3);

	G3X_SetShading(GX_SHADING_HIGHLIGHT);
	G3X_AntiAlias(true);
	G3_SwapBuffers(SWAP_BUFFERS_SORTMODE, SWAP_BUFFERS_BUFFERMODE);

	G3X_AlphaTest(false, 0);                   // AlphaTest OFF
	G3X_AlphaBlend(true);                      // AlphaTest ON
	G3X_EdgeMarking(true);

	G3X_SetEdgeColorTable(&sEdgeMarkingColorTable[0]);

	G3X_SetClearColor(GX_RGB(119 >> 3, 199 >> 3, 244 >> 3), 31, 0x7fff, 0, false);
	G3_ViewPort(0, 0, 255, 191);

	GX_SetDispSelect(GX_DISP_SELECT_SUB_MAIN);

	NNS_GfdResetLnkTexVramState();
	NNS_GfdResetLnkPlttVramState();

	NNS_G2dInitOamManagerModule();
	NNS_G2dGetNewOamManagerInstanceAsFastTransferMode(&mSubObjOamManager, 0, 128, NNS_G2D_OAMTYPE_SUB);

	mCellDataSub = Util_LoadFileToBuffer("/data/game/OBJ.NCER", NULL, FALSE);
	NNS_G2dGetUnpackedCellBank(mCellDataSub, &mCellDataSubBank);

	NNSG2dCharacterData* mCharDataSubUnpacked;
	void* mCharDataSub = Util_LoadFileToBuffer("/data/game/IngameOBJ.NCGR", NULL, TRUE);
	NNS_G2dGetUnpackedCharacterData(mCharDataSub, &mCharDataSubUnpacked);
	NNS_G2dInitImageProxy(&mImageProxy);
	NNS_G2dLoadImage2DMapping(mCharDataSubUnpacked, 0, NNS_G2D_VRAM_TYPE_2DSUB, &mImageProxy);
	NNS_FndFreeToExpHeap(gHeapHandle, mCharDataSub);

	NNSG2dPaletteData* mPalDataSubUnpacked;
	void* mPalDataSub = Util_LoadFileToBuffer("/data/game/IngameOBJ.NCLR", NULL, TRUE);
	NNS_G2dInitImagePaletteProxy(&mImagePaletteProxy);
	NNS_G2dGetUnpackedPaletteData(mPalDataSub, &mPalDataSubUnpacked);
	NNS_G2dLoadPalette(mPalDataSubUnpacked, 0, NNS_G2D_VRAM_TYPE_2DSUB, &mImagePaletteProxy);
	NNS_FndFreeToExpHeap(gHeapHandle, mPalDataSub);

	NNSG2dScreenData* mScreenDataSubUnpacked;
	void* mScreenDataSub = Util_LoadFileToBuffer("/data/game/BG.NSCR", NULL, TRUE);
	NNS_G2dGetUnpackedScreenData(mScreenDataSub, &mScreenDataSubUnpacked);
	mCharDataSub = Util_LoadFileToBuffer("/data/game/IngameBG.NCGR", NULL, TRUE);
	NNS_G2dGetUnpackedCharacterData(mCharDataSub, &mCharDataSubUnpacked);
	mPalDataSub = Util_LoadFileToBuffer("/data/game/IngameBG.NCLR", NULL, TRUE);
	NNS_G2dGetUnpackedPaletteData(mPalDataSub, &mPalDataSubUnpacked);
	NNS_G2dBGSetup(NNS_G2D_BGSELECT_SUB0, mScreenDataSubUnpacked, mCharDataSubUnpacked, mPalDataSubUnpacked, GX_BG_SCRBASE_0x0800, GX_BG_CHARBASE_0x00000);
	NNS_FndFreeToExpHeap(gHeapHandle, mScreenDataSub);
	NNS_FndFreeToExpHeap(gHeapHandle, mCharDataSub);
	NNS_FndFreeToExpHeap(gHeapHandle, mPalDataSub);

	MI_CpuClear8(&mTmpSubOamBuffer[0], sizeof(mTmpSubOamBuffer));

	mGameController = new GameController();

	VecFx32 a = { 64 * FX32_ONE - 32 * FX32_ONE, 0, (2 + 24) * FX32_ONE - 32 * FX32_ONE };
	VecFx32 b = { 64 * FX32_ONE - 32 * FX32_ONE, 0, (2 + 20 + 24) * FX32_ONE - 32 * FX32_ONE };
	sDummyPieces[0] = new FlexTrack(&a, &b);

	mGameController->mWagon->PutOnTrack(sDummyPieces[0], 10 * FX32_ONE);

	GX_SetOBJVRamModeChar(GX_OBJVRAMMODE_CHAR_1D_32K);

	mFontData = Util_LoadFileToBuffer("/data/fonts/droid_sans_mono_10pt.NFTR", NULL, false);
	MI_CpuClear8(&mFont, sizeof(mFont));
	NNS_G2dFontInitAuto(&mFont, mFontData);

	NNS_G2dCharCanvasInitForOBJ1D(&mCanvas, (uint8_t*)G2_GetOBJCharPtr(), 8, 4, NNS_G2D_CHARA_COLORMODE_16);
	NNS_G2dTextCanvasInit(&mTextCanvas, &mCanvas, &mFont, 0, 1);
	NNS_G2dCharCanvasClear(&mCanvas, 0);
	NNS_G2dTextCanvasDrawTextRect(
		&mTextCanvas, 0, 0, 64, 32, 1, NNS_G2D_VERTICALORIGIN_TOP | NNS_G2D_HORIZONTALORIGIN_LEFT | NNS_G2D_HORIZONTALALIGN_CENTER | NNS_G2D_VERTICALALIGN_MIDDLE, (NNSG2dChar*)L"Tri's Test");

	for(int i = 0; i < 16; i++)
		((uint16_t*)HW_OBJ_PLTT)[i] = 0x7FFF;
	((uint16_t*)HW_OBJ_PLTT)[0] = 0;

	G2_SetOBJAttr(&GXOamAttrArray[0], 0, 0, 0, GX_OAM_MODE_NORMAL, FALSE, GX_OAM_EFFECT_NONE, GX_OAM_SHAPE_64x32, GX_OAM_COLORMODE_16, 0, 0, 0);

	reg_G2_BLDCNT = 0x2801;

	mSubFontData = Util_LoadFileToBuffer("/data/fonts/fot_rodin_bokutoh_pro_db_9pt.NFTR", NULL, false);
	MI_CpuClear8(&mSubFont, sizeof(mSubFont));
	NNS_G2dFontInitAuto(&mSubFont, mSubFontData);

	NNS_G2dCharCanvasInitForOBJ1D(&mSubCanvas, ((uint8_t*)G2S_GetOBJCharPtr()) + 8192, 12, 2, NNS_G2D_CHARA_COLORMODE_16);
	NNS_G2dTextCanvasInit(&mSubTextCanvas, &mSubCanvas, &mSubFont, 0, 1);
	NNS_G2dCharCanvasClear(&mSubCanvas, 0);
	NNS_G2dTextCanvasDrawTextRect(
		&mSubTextCanvas, 0, 0, 32, 16, 1, NNS_G2D_VERTICALORIGIN_TOP | NNS_G2D_HORIZONTALORIGIN_LEFT | NNS_G2D_HORIZONTALALIGN_CENTER | NNS_G2D_VERTICALALIGN_MIDDLE, (NNSG2dChar*)L"Speed");

	mSubCanvas.charBase = ((uint8_t*)G2S_GetOBJCharPtr()) + 8192 + 256;
	NNS_G2dTextCanvasDrawTextRect(
		&mSubTextCanvas, 0, 0, 32, 16, 1, NNS_G2D_VERTICALORIGIN_TOP | NNS_G2D_HORIZONTALORIGIN_LEFT | NNS_G2D_HORIZONTALALIGN_CENTER | NNS_G2D_VERTICALALIGN_MIDDLE, (NNSG2dChar*)L"Stop");

	mSubCanvas.charBase = ((uint8_t*)G2S_GetOBJCharPtr()) + 8192 + 2 * 256;
	NNS_G2dTextCanvasDrawTextRect(
		&mSubTextCanvas, 0, 0, 32, 16, 1, NNS_G2D_VERTICALORIGIN_TOP | NNS_G2D_HORIZONTALORIGIN_LEFT | NNS_G2D_HORIZONTALALIGN_CENTER | NNS_G2D_VERTICALALIGN_MIDDLE, (NNSG2dChar*)L"Horn");

	mSubCanvas.charBase = ((uint8_t*)G2S_GetOBJCharPtr()) + 8192 + 3 * 256;
	NNS_G2dTextCanvasDrawTextRect(
		&mSubTextCanvas, 0, 0, 64, 16, 1, NNS_G2D_VERTICALORIGIN_TOP | NNS_G2D_HORIZONTALORIGIN_LEFT | NNS_G2D_HORIZONTALALIGN_CENTER | NNS_G2D_VERTICALALIGN_MIDDLE, (NNSG2dChar*)L"Headlight");

	mSubFontData2 = Util_LoadFileToBuffer("/data/fonts/fot_rodin_bokutoh_pro_b_13pt.NFTR", NULL, false);
	MI_CpuClear8(&mSubFont2, sizeof(mSubFont2));
	NNS_G2dFontInitAuto(&mSubFont2, mSubFontData2);

	NNS_G2dCharCanvasInitForOBJ1D(&mSubCanvas2, ((uint8_t*)G2S_GetOBJCharPtr()) + 8192 + 5 * 256, 16, 4, NNS_G2D_CHARA_COLORMODE_16);
	NNS_G2dTextCanvasInit(&mSubTextCanvas2, &mSubCanvas2, &mSubFont2, 0, 1);
	NNS_G2dCharCanvasClear(&mSubCanvas2, 0);
	NNS_G2dTextCanvasDrawTextRect(
		&mSubTextCanvas2, 0, 0, 128, 32, 1, NNS_G2D_VERTICALORIGIN_TOP | NNS_G2D_HORIZONTALORIGIN_LEFT | NNS_G2D_HORIZONTALALIGN_CENTER | NNS_G2D_VERTICALALIGN_MIDDLE, (NNSG2dChar*)L"Train Control");

	for (int i = 0; i < 16; i++)
	{
		int rnew = 4 + ((28 - 4) * i) / 15;
		int gnew = 4 + ((28 - 4) * i) / 15;
		int bnew = 4 + ((28 - 4) * i) / 15;
		((uint16_t*)HW_DB_OBJ_PLTT)[i + 16] = GX_RGB(rnew, gnew, bnew);
	}

	for (int i = 0; i < 16; i++)
	{
		int rnew = 3 + ((28 - 3) * i) / 15;
		int gnew = 3 + ((28 - 3) * i) / 15;
		int bnew = 2 + ((28 - 2) * i) / 15;
		((uint16_t*)HW_DB_OBJ_PLTT)[i + 32] = GX_RGB(rnew, gnew, bnew);
	}

	G2S_SetBG3ControlDCBmp(GX_BG_SCRSIZE_DCBMP_128x128, GX_BG_AREAOVER_XLU, GX_BG_BMPSCRBASE_0x00000);
	MtxFx22 bg3mtx = { FX32_ONE, 0, 0, FX32_ONE };
	G2S_SetBG3Affine(&bg3mtx, 64, 64, -(92 + 16), -(36 - 32));
	G2S_SetWnd0Position(94, 38, 94 + 156, 38 + 92);
	G2S_SetWnd0InsidePlane(GX_WND_PLANEMASK_BG3 | GX_WND_PLANEMASK_OBJ, false);
	G2S_SetWndOutsidePlane(GX_WND_PLANEMASK_BG0 | GX_WND_PLANEMASK_OBJ, false);
	GXS_SetVisibleWnd(GX_WNDMASK_W0);

	G2S_SetBG0Priority(3);
	G2S_SetBG3Priority(0);

	mUIManager = new UIManager(this);
	//mTrackBuildUISlice = new TrackBuildUISlice();
	//mUIManager->AddSlice(mTrackBuildUISlice);
	mUIManager->RegisterPenCallbacks(Game::OnPenDown, Game::OnPenMove, Game::OnPenUp);

	NNS_SndArcLoadSeqArc(SEQ_TRAIN, gSndHeapHandle);
	NNS_SndArcLoadBank(BANK_TRAIN, gSndHeapHandle);
	//NNS_SndArcPlayerStartSeqArc(&mTrain.trackSoundHandle, SEQ_TRAIN, TRAIN_TRACK);

	mPickingCallback = NULL;

	//mCamera = new ThirdPersonCamera();//LookAtCamera();
	//mCamera->mTrain = &mTrain;
	//Train_UpdatePos(&mTrain);
	//mGameController->mCamera = new FreeRoamCamera();
	//mCamera->mDestination = mTrain.firstPart->position;
	mGameController->mWagon->GetPosition(&mGameController->mCamera->mDestination);
	mGameController->mCamera->mDestination.x -= 32 * FX32_ONE;
	mGameController->mCamera->mDestination.z -= 32 * FX32_ONE;
	VecFx32 camRot = { 0, 22 * FX32_ONE, 0 };
	mGameController->mCamera->SetRotation(&camRot);
	/*#ifdef FIRST_PERSON
		mCamera->mUp.x = 0;
		mCamera->mUp.y = FX32_ONE;
		mCamera->mUp.z = 0;
	#else
	#ifndef TOP_VIEW
		mCamera->mPosition.x = 3 * FX32_ONE;
		mCamera->mPosition.y = 2.25 * FX32_ONE;
		mCamera->mPosition.z = -0.75 * FX32_ONE;
		mCamera->mUp.x = 0;
		mCamera->mUp.y = FX32_ONE;
		mCamera->mUp.z = 0;
		mCamera->mDestination.x = 0 * FX32_ONE;
		mCamera->mDestination.y = 0;
		mCamera->mDestination.z = -2 * FX32_ONE;
	#else
		mCamera->mPosition.x = -0.5 * FX32_ONE;
		mCamera->mPosition.y = 5 * FX32_ONE;
		mCamera->mPosition.z = -2.5 * FX32_ONE;
		mCamera->mUp.x = 0;
		mCamera->mUp.y = 0;
		mCamera->mUp.z = FX32_ONE;
		mCamera->mDestination.x = -0.5 * FX32_ONE;
		mCamera->mDestination.y = 0;
		mCamera->mDestination.z = -2.5 * FX32_ONE;
	#endif
	#endif*/

	NNS_G3dGlbPerspectiveW(VIEW_ANGLE_SIN, VIEW_ANGLE_COS, (256 * 4096 / 192), 4096 >> 3, /*64*//*18*//*24*//*31*/8 * 4096, 40960 * 4);

	InvalidateSub3D();

	mDragTool = new AddTrackTool(this);

	mVRAMCopyVAlarm = new OS::VAlarm();
	mVRAMCopyVAlarm->SetPeriodic(192 - 48, 5, Game::OnVRAMCopyVAlarm, this);
}

static int mVRAMReadyLine;

void Game::OnVRAMCopyVAlarm()
{
	if(mCurFrameType == FRAME_TYPE_MAIN_NEAR)
		mGameController->mMap->mTerrainTextureManager16->UpdateVramC();
	else
		mGameController->mMap->mTerrainTextureManager8->UpdateVramC();
}

void Game::RequestPicking(int x, int y, PickingCallbackFunc callback, void* arg)
{
	mPickingPointX = x;
	mPickingPointY = y;
	mPickingCallback = callback;
	mPickingCallbackArg = arg;
	mPickingRequested = true;
}

void Game::HandlePickingVBlank()
{
	if (mPickingState == PICKING_STATE_CAPTURING)
		mPickingResult = ((picking_result_t*)HW_LCDC_VRAM_B)[mPickingPointX + mPickingPointY * 256];// ((picking_result_t*)HW_LCDC_VRAM_D)[mPickingPointX + mPickingPointY * 256];
}

void Game::HandlePickingEarly()
{
	if (mPickingState == PICKING_STATE_RENDERING)
	{
		mPickingState = PICKING_STATE_CAPTURING;
	}
	else if (mPickingState == PICKING_STATE_CAPTURING)
	{
		if (mPickingCallback) mPickingCallback(mPickingCallbackArg, mPickingResult);//((picking_result_t*)HW_LCDC_VRAM_D)[mPickingPointX + mPickingPointY * 256]);
		mPickingCallback = NULL;
		mPickingState = PICKING_STATE_READY;
	}
}

void Game::HandlePickingLate()
{
	if (mPickingState == PICKING_STATE_READY && mCurFrameType == FRAME_TYPE_MAIN_FAR && mLastFrameType == FRAME_TYPE_MAIN_NEAR && mPickingRequested)
	{
		reg_G3X_DISP3DCNT = reg_G3X_DISP3DCNT & ~REG_G3X_DISP3DCNT_TME_MASK;
		G3X_SetClearColor(0, /*31*/0, 0x7fff, 0, false);
		G3X_EdgeMarking(false);
		G3X_AntiAlias(false);
		G3X_SetFog(false, GX_FOGBLEND_COLOR_ALPHA, GX_FOGSLOPE_0x0020, 0);
		mPickingRequested = false;
		mPickingState = PICKING_STATE_RENDERING;
		mCurFrameType = FRAME_TYPE_MAIN_PICKING;
	}
}

void Game::OnPenDownPickingCallback(picking_result_t result)
{
	mPenDownResult = result;
}

void Game::OnPenDown(int x, int y)
{
	mDragTool->OnPenDown(x, y);
	/*mPenDownTime = OS_GetTick();
	mPenDownPointX = x;
	mPenDownPointY = y;
	RequestPicking(x, y, Game::OnPenDownPickingCallback, this);*/
}

void Game::OnPenMove(int x, int y)
{
	mDragTool->OnPenMove(x, y);
}

int state = 0;
int firstX;
int firstZ;
FlexTrack* piece;

void Game::OnPenUpPickingCallback(picking_result_t result)
{
	if (result == mPenDownResult)
	{
		mSelectedMapX = -1;
		mSelectedMapZ = -1;
		mSelectedTrain = -1;
		if(PICKING_IDX(result) > 0)
		{
			if (PICKING_TYPE(result) == PICKING_TYPE_MAP)
			{
				int idx = PICKING_IDX(result) - 1;
				mSelectedMapX = mPickingXStart + idx % (mPickingXEnd - mPickingXStart);
				mSelectedMapZ = mPickingZStart + idx / (mPickingXEnd - mPickingXStart);
				NOCASH_Printf("Picked (%d,%d)", mSelectedMapX, mSelectedMapZ);
				/*if (state == 0)
				{
					firstX = mSelectedMapX;
					firstZ = mSelectedMapZ;
					state++;
				}
				else if (state == 1)
				{
					piece = new FlexTrack(firstX, mMap->mTiles[firstZ][firstX].y, firstZ, mSelectedMapX, mMap->mTiles[mSelectedMapZ][mSelectedMapX].y, mSelectedMapZ);
					mMap->AddTrackPiece(piece);
					//state++;
					state = 0;
				}
				else
				{
					piece->mEndPosition.x = mSelectedMapX;
					piece->mEndPosition.z = mSelectedMapZ;
				}*/
				//mMap->mVtx[mSelectedMapZ * 128 + mSelectedMapX]++;
				//mMap->mVtx[mSelectedMapZ * 128 + mSelectedMapX + 1]++;
				//mMap->mVtx[(mSelectedMapZ + 1) * 128 + mSelectedMapX]++;
				//mMap->mVtx[(mSelectedMapZ + 1) * 128 + mSelectedMapX + 1]++;
			}
			else if (PICKING_TYPE(result) == PICKING_TYPE_TRAIN)
			{
				mSelectedTrain = PICKING_IDX(result) - 1;
			}
		}
	}
}

void Game::OnPenUp(int x, int y)
{
	mDragTool->OnPenUp(x, y);
	//mPenUpTime = OS_GetTick();
	//if (abs(mPenDownPointX - x) < 16 && abs(mPenDownPointY - y) < 16)
	//{
	//	RequestPicking(x, y, Game::OnPenUpPickingCallback, this);
	//}
}

static void G3_Vtx32(VecFx32* vec)
{
	G3_PushMtx();
	G3_Translate(vec->x, vec->y, vec->z);
	G3_Vtx10(0, 0, 0);
	G3_PopMtx(1);
}

static void VEC_MinMax(VecFx32* a, VecFx32* min, VecFx32* max)
{
	if (a->x < min->x)
		min->x = a->x;
	if (a->y < min->y)
		min->y = a->y;
	if (a->z < min->z)
		min->z = a->z;

	if (a->x > max->x)
		max->x = a->x;
	if (a->y > max->y)
		max->y = a->y;
	if (a->z > max->z)
		max->z = a->z;
}

static void CalculateVisibleGrid(VecFx32* bbmin, VecFx32* bbmax)
{
	int y = 0;

	//calculate the corners of the frustum box
	VecFx32 far_top_left;
	VecFx32 near_top_left;
	NNS_G3dScrPosToWorldLine(0, 0, &near_top_left, &far_top_left);

	/*fx32 f = FX_Div(near_top_left.y, far_top_left.y - near_top_left.y);
	VecFx32 top_left;
	top_left.x = near_top_left.x - FX_Mul(f, far_top_left.x - near_top_left.x);
	top_left.y = 0;
	top_left.z = near_top_left.z - FX_Mul(f, far_top_left.z - near_top_left.z);*/

	VecFx32 far_top_right;
	VecFx32 near_top_right;
	NNS_G3dScrPosToWorldLine(255, 0, &near_top_right, &far_top_right);

	/*f = FX_Div(near_top_right.y, far_top_right.y - near_top_right.y);
	VecFx32 top_right;
	top_right.x = near_top_right.x - FX_Mul(f, far_top_right.x - near_top_right.x);
	top_right.y = 0;
	top_right.z = near_top_right.z - FX_Mul(f, far_top_right.z - near_top_right.z);*/

	VecFx32 far_bottom_left;
	VecFx32 near_bottom_left;
	NNS_G3dScrPosToWorldLine(0, 191, &near_bottom_left, &far_bottom_left);

	/*f = FX_Div(near_bottom_left.y, far_bottom_left.y - near_bottom_left.y);
	VecFx32 bottom_left;
	bottom_left.x = near_bottom_left.x - FX_Mul(f, far_bottom_left.x - near_bottom_left.x);
	bottom_left.y = 0;
	bottom_left.z = near_bottom_left.z - FX_Mul(f, far_bottom_left.z - near_bottom_left.z); */

	VecFx32 far_bottom_right;
	VecFx32 near_bottom_right;
	NNS_G3dScrPosToWorldLine(255, 191, &near_bottom_right, &far_bottom_right);

	/*f = FX_Div(near_bottom_right.y, far_bottom_right.y - near_bottom_right.y);
	VecFx32 bottom_right;
	bottom_right.x = near_bottom_right.x - FX_Mul(f, far_bottom_right.x - near_bottom_right.x);
	bottom_right.y = 0;
	bottom_right.z = near_bottom_right.z - FX_Mul(f, far_bottom_right.z - near_bottom_right.z); */

	VecFx32 min = { FX32_MAX, FX32_MAX, FX32_MAX };
	VecFx32 max = { FX32_MIN, FX32_MIN, FX32_MIN };

	VEC_MinMax(&far_top_left, &min, &max);
	VEC_MinMax(&near_top_left, &min, &max);
	VEC_MinMax(&far_top_right, &min, &max);
	VEC_MinMax(&near_top_right, &min, &max);
	VEC_MinMax(&far_bottom_left, &min, &max);
	VEC_MinMax(&near_bottom_left, &min, &max);
	VEC_MinMax(&far_bottom_right, &min, &max);
	VEC_MinMax(&near_bottom_right, &min, &max);

	//NOCASH_Printf("min: %d; %d; %d", min.x / 4096, min.y / 4096, min.z / 4096);
	//NOCASH_Printf("max: %d; %d; %d", max.x / 4096, max.y / 4096, max.z / 4096);

	*bbmin = min;
	*bbmax = max;
}

int Game::MakeTextCell(GXOamAttr* pOAM, int x, int y, int w, int h, int palette, u32 address)
{
	int count = NNS_G2dArrangeOBJ1D(pOAM, w >> 3, h >> 3, x, y, GX_OAM_COLORMODE_16, address >> 5, NNS_G2D_OBJVRAMMODE_32K);
	for (int i = 0; i < count; i++)
		G2_SetOBJMode(&pOAM[i], GX_OAM_MODE_NORMAL, palette);
	return count;
}

void Game::Render()
{
	if (mSub3DInvalidated && mLastFrameType == FRAME_TYPE_MAIN_NEAR)
	{
		mSub3DInvalidated = false;
		mCurFrameType = FRAME_TYPE_SUB;
	}

	HandlePickingEarly();

	mUIManager->ProcessInput();
	u16 keyData = PAD_Read();

	HandlePickingLate();

	if (keyData & PAD_BUTTON_B)
	{
		mGameController->mWagon->mDriving = true;
	}
	else
		mGameController->mWagon->mDriving = false;
	if (!mKeyTimer)
	{
		if (keyData & PAD_BUTTON_L && keyData & PAD_BUTTON_R)
		{
			GX_SetDispSelect((GX_GetDispSelect() == GX_DISP_SELECT_MAIN_SUB ? GX_DISP_SELECT_SUB_MAIN : GX_DISP_SELECT_MAIN_SUB));
			mKeyTimer = 20;
		}
		if (keyData & PAD_BUTTON_X)
		{
			mGameController->mMap->SetGridEnabled(!mGameController->mMap->GetGridEnabled());
			mKeyTimer = 10;
		}
		if (keyData & PAD_BUTTON_Y)
		{
			mAntiAliasEnabled = !mAntiAliasEnabled;
			mKeyTimer = 10;
		}
		if (keyData & PAD_BUTTON_SELECT)
		{
			if (mGameController->mMap->GetFirstTrackPiece() != NULL)
			{
				mGameController->mWagon->PutOnTrack(mGameController->mMap->GetFirstTrackPiece());
			}
			mKeyTimer = 10;
		}
	}
	else
		mKeyTimer--;

	VecFx32 camRot;
	mGameController->mCamera->GetRotation(&camRot);
	if (keyData & PAD_BUTTON_A)
	{
		if (keyData & PAD_KEY_LEFT)
			camRot.x += FX32_ONE >> 1;
		else if (keyData & PAD_KEY_RIGHT)
			camRot.x -= FX32_ONE >> 1;
		if (keyData & PAD_KEY_UP)
			camRot.y += FX32_ONE >> 2;
		else if (keyData & PAD_KEY_DOWN)
			camRot.y -= FX32_ONE >> 2;
		if (keyData & PAD_BUTTON_L && !(keyData & PAD_BUTTON_R))
			mGameController->mCamera->MoveY(-FX32_ONE >> 5);
		else if (keyData & PAD_BUTTON_R && !(keyData & PAD_BUTTON_L))
			mGameController->mCamera->MoveY(FX32_ONE >> 5);
	}
	else
	{
		if (keyData & PAD_KEY_LEFT)
			mGameController->mCamera->MoveX(-FX32_ONE / 50);// 24);
		else if (keyData & PAD_KEY_RIGHT)
			mGameController->mCamera->MoveX(FX32_ONE / 50);// 24);
		if (keyData & PAD_KEY_UP)
			mGameController->mCamera->MoveZ(FX32_ONE / 50);// 24);
		else if (keyData & PAD_KEY_DOWN)
			mGameController->mCamera->MoveZ(-FX32_ONE / 50);// 24);
		if (keyData & PAD_BUTTON_L && !(keyData & PAD_BUTTON_R))
			camRot.x -= FX32_ONE >> 1;
		else if (keyData & PAD_BUTTON_R && !(keyData & PAD_BUTTON_L))
			camRot.x += FX32_ONE >> 1;
	}

	mGameController->mCamera->SetRotation(&camRot);

	if (keyData & PAD_BUTTON_START)
		Game::GotoMenu();
	G3X_Reset();
	if (mCurFrameType != FRAME_TYPE_MAIN_PICKING && mCurFrameType != FRAME_TYPE_SUB)
	{
		reg_G3X_DISP3DCNT = reg_G3X_DISP3DCNT | REG_G3X_DISP3DCNT_TME_MASK;
		if(mCurFrameType == FRAME_TYPE_MAIN_FAR)
			G3X_SetClearColor(GX_RGB(168 >> 3, 209 >> 3, 255 >> 3), 31, 0x7fff, 0, true);
		else
			G3X_SetClearColor(GX_RGB(168 >> 3, 209 >> 3, 255 >> 3), /*31*/0, 0x7fff, 0, false);
		G3X_SetShading(GX_SHADING_HIGHLIGHT);
		G3X_EdgeMarking(true);
		G3X_AntiAlias(mAntiAliasEnabled);
		if (mCurFrameType == FRAME_TYPE_MAIN_FAR)
			G3X_SetFog(true, GX_FOGBLEND_COLOR_ALPHA, GX_FOGSLOPE_0x0800, 0x8000 - 0xC00);
		else
			G3X_SetFog(false, GX_FOGBLEND_ALPHA, GX_FOGSLOPE_0x0800, 0x8000 - 0xC00);
			//G3X_SetFog(/*true*/false, GX_FOGBLEND_COLOR_ALPHA, GX_FOGSLOPE_0x0400, 0x8000 - 0x100);
		//G3X_SetFogColor(GX_RGB(119 >> 3, 199 >> 3, 244 >> 3), 25);
		G3X_SetFogColor(GX_RGB(168 >> 3, 209 >> 3, 255 >> 3), 31);
		u32 fog_table[8];
		for (int i = 0; i < 8; i++)
		{
			fog_table[i] =
				(u32)(((i * 16) << 0) | ((i * 16 + 4) << 8) | ((i * 16 + 8) << 16) | ((i * 16 +
					12) << 24));
		}
		G3X_SetFogTable(&fog_table[0]);
		G3X_SetToonTable(&sToonTable[0]);
	}
	mGameController->mWagon->Update();
	if (mCurFrameType != FRAME_TYPE_SUB)
	{
		NNS_G3dGlbSetViewPort(0, 0, 255, 191);
		//Train_UpdatePos(&mTrain);
#ifdef FIRST_PERSON
		mCamera->mPosition.x = tpos.x - 8 * FX32_ONE;// + 0.4 * dir.x;
		mCamera->mPosition.y = tpos.y + FX32_HALF + (FX32_HALF >> 1);// + 0.4 * dir.y;
		mCamera->mPosition.z = tpos.z - 8 * FX32_ONE;// + 0.4 * dir.z;
		mCamera->mDestination.x = tpos.x - 8 * FX32_ONE + 4 * dir.x;
		mCamera->mDestination.y = tpos.y + FX32_HALF + 4 * dir.y;// + 1 * FX32_ONE;
		mCamera->mDestination.z = tpos.z - 8 * FX32_ONE + 4 * dir.z;
#endif
		mGameController->mCamera->Apply();

		//Train_UpdateSound(&mTrain, mCamera);

		NNS_G3dGlbPolygonAttr(GX_LIGHTMASK_0, GX_POLYGONMODE_MODULATE, GX_CULL_BACK, 0, 31, GX_POLYGON_ATTR_MISC_FOG | GX_POLYGON_ATTR_MISC_FAR_CLIPPING);
		/*VecFx32 vec = { FX32_CONST(0), FX32_CONST(-1), FX32_CONST(-1) };
		VEC_Normalize(&vec, &vec);
		if (vec.x > GX_FX32_FX10_MAX) vec.x = GX_FX32_FX10_MAX;
		else if (vec.x < GX_FX32_FX10_MIN) vec.x = GX_FX32_FX10_MIN;
		if (vec.y > GX_FX32_FX10_MAX) vec.y = GX_FX32_FX10_MAX;
		else if (vec.y < GX_FX32_FX10_MIN) vec.y = GX_FX32_FX10_MIN;
		if (vec.z > GX_FX32_FX10_MAX) vec.z = GX_FX32_FX10_MAX;
		else if (vec.z < GX_FX32_FX10_MIN) vec.z = GX_FX32_FX10_MIN;*/
		NNS_G3dGlbLightVector(GX_LIGHTID_0, /*vec.x, vec.y, vec.z);//*/-2048, -2897, -2048);
		NNS_G3dGlbLightColor(GX_LIGHTID_0, /*GX_RGB(20, 12, 3));//*/GX_RGB(31, 31, 31));
		if (mCurFrameType == FRAME_TYPE_MAIN_PICKING)
		{
			NNS_G3dGlbMaterialColorDiffAmb(GX_RGB(0, 0, 0), GX_RGB(0, 0, 0), false);
			NNS_G3dGlbMaterialColorSpecEmi(GX_RGB(0, 0, 0), GX_RGB(0, 0, 0), false);
		}
		else
		{
			NNS_G3dGlbMaterialColorDiffAmb(GX_RGB(31, 31, 31), /*GX_RGB(5, 5, 5)*/GX_RGB(10, 10, 10), false);
			NNS_G3dGlbMaterialColorSpecEmi(/*GX_RGB(3, 3, 3)*/GX_RGB(1, 1, 1), GX_RGB(0, 0, 0), false);
		}

		if (mCurFrameType != FRAME_TYPE_MAIN_PICKING)
		{
			if (mCurFrameType == FRAME_TYPE_MAIN_FAR)
			{
				NNS_G3dGlbPerspectiveW(VIEW_ANGLE_SIN, VIEW_ANGLE_COS, (256 * 4096 / 192), 8 * 4096, /*35*/50 * 4096, 40960 * 4);
			}
			else
			{
				NNS_G3dGlbPerspectiveW(VIEW_ANGLE_SIN, VIEW_ANGLE_COS, (256 * 4096 / 192), 4096 >> 3, 10 * 4096, 40960 * 4);
			}
		}
		else
			NNS_G3dGlbPerspectiveW(VIEW_ANGLE_SIN, VIEW_ANGLE_COS, (256 * 4096 / 192), 4096 >> 3, 35 * 4096, 40960 * 4);

		VecFx32 bbmin, bbmax;
		CalculateVisibleGrid(&bbmin, &bbmax);

		int xstart = (bbmin.x - 2 * FX32_ONE - FX32_HALF) / FX32_ONE + 32;
		xstart = MATH_CLAMP(xstart, 0, 128);
		int zstart = (bbmin.z - 2 * FX32_ONE - FX32_HALF) / FX32_ONE + 32;
		zstart = MATH_CLAMP(zstart, 0, 128);

		int xend = (bbmax.x + 2 * FX32_ONE + FX32_HALF) / FX32_ONE + 32;
		xend = MATH_CLAMP(xend, 0, 128);
		int zend = (bbmax.z + 2 * FX32_ONE + FX32_HALF) / FX32_ONE + 32;
		zend = MATH_CLAMP(zend, 0, 128);

		NNS_G3dGlbFlushP();
		NNS_G3dGeFlushBuffer();

		if (mPickingState == PICKING_STATE_RENDERING)
		{
			mPickingXStart = xstart;
			mPickingXEnd = xend;
			mPickingZStart = zstart;
		}

		G3_PushMtx();
		{
			VecFx32 camDir;
			mGameController->mCamera->GetLookDirection(&camDir);
			mGameController->mMap->Render(xstart, xend, zstart, zend, mCurFrameType == FRAME_TYPE_MAIN_PICKING, mSelectedMapX, mSelectedMapZ, &mGameController->mCamera->mPosition, &camDir, (mCurFrameType == FRAME_TYPE_MAIN_FAR ? 1 : 0));
			mGameController->mWagon->Render();
		}
		G3_PopMtx(1);
		mUIManager->Render();
	}
	else
	{
		reg_G3X_DISP3DCNT = reg_G3X_DISP3DCNT | REG_G3X_DISP3DCNT_TME_MASK;
		G3X_SetClearColor(GX_RGB(2, 2, 2), 31, 0x7fff, 0, false);
		G3X_SetShading(GX_SHADING_HIGHLIGHT);
		G3X_EdgeMarking(false);
		G3X_AntiAlias(true);
		G3X_SetFog(false, GX_FOGBLEND_COLOR_ALPHA, GX_FOGSLOPE_0x0020, 0);
		//sub 3d is 128x96
		NNS_G3dGlbSetViewPort(0, 96, 127, 191);

		FreeRoamCamera cam = FreeRoamCamera();
		mGameController->mWagon->GetPosition(&cam.mDestination);
		cam.mDestination.x -= 32 * FX32_ONE;
		cam.mDestination.y += FX32_CONST(0.15f);
		cam.mDestination.z -= 32 * FX32_ONE;
		cam.mCamDistance = FX32_CONST(0.55f);
		VecFx32 camRot = { 43 * FX32_ONE, 10 * FX32_ONE, 0 };
		cam.SetRotation(&camRot);
		cam.Apply();

		NNS_G3dGlbPolygonAttr(GX_LIGHTMASK_0, GX_POLYGONMODE_MODULATE, GX_CULL_BACK, 0, 31, GX_POLYGON_ATTR_MISC_FOG | GX_POLYGON_ATTR_MISC_FAR_CLIPPING);
		NNS_G3dGlbLightVector(GX_LIGHTID_0, -2048, -2897, -2048);
		NNS_G3dGlbLightColor(GX_LIGHTID_0, GX_RGB(31, 31, 31));
		NNS_G3dGlbMaterialColorDiffAmb(GX_RGB(31, 31, 31), GX_RGB(10, 10, 10), false);
		NNS_G3dGlbMaterialColorSpecEmi(GX_RGB(1, 1, 1), GX_RGB(0, 0, 0), false);
		NNS_G3dGlbPerspectiveW(VIEW_ANGLE_SIN, VIEW_ANGLE_COS, (128 * 4096 / 96), 4096 >> 4, 10 * 4096, 40960 * 4);
		NNS_G3dGlbFlushP();
		NNS_G3dGeFlushBuffer();
		G3_PushMtx();
		{
			G3_Translate(-32 * FX32_ONE, 0, -32 * FX32_ONE);
			G3_Translate(0, 0, FX32_CONST(-0.2f));
			mGameController->mWagon->Render();
		}
		G3_PopMtx(1);
	}
	//InvalidateSub3D();
	//char result[64];
	//MI_CpuClear8(result, sizeof(result));
	OS_Printf("%d: %d\n", mCurFrameType, G3X_GetPolygonListRamCount());
	//OS_SPrintf(result, "%d;%d;%d", G3X_GetVtxListRamCount(), G3X_GetPolygonListRamCount(), mVRAMReadyLine);
	//u16 result2[64];
	//MI_CpuClear8(result2, sizeof(result2));
	//for (int i = 0; i < sizeof(result); i++)
	//{
	//	result2[i] = result[i];
	//}
	//NNS_G2dCharCanvasClear(&mCanvas, 0);
	//NNS_G2dTextCanvasDrawTextRect(
	//	&mTextCanvas, 0, 0, 64, 32, 1, NNS_G2D_VERTICALORIGIN_TOP | NNS_G2D_HORIZONTALORIGIN_LEFT | NNS_G2D_HORIZONTALALIGN_CENTER | NNS_G2D_VERTICALALIGN_MIDDLE, (NNSG2dChar*)result2);// (NNSG2dChar*)L"Tri's Test");
	G3_SwapBuffers(SWAP_BUFFERS_SORTMODE, SWAP_BUFFERS_BUFFERMODE);
	mGameController->Update();
	//sub screen oam
	NNSG2dFVec2 trans;
	u16 numOamDrawn = 0;
	numOamDrawn += MakeTextCell((GXOamAttr*)&mTmpSubOamBuffer[numOamDrawn], 14 + 16, 59 - 16, 32, 16, 1, 8192);
	numOamDrawn += MakeTextCell((GXOamAttr*)&mTmpSubOamBuffer[numOamDrawn], 14 + 16, 146, 32, 16, 2, 8192 + 256);
	numOamDrawn += MakeTextCell((GXOamAttr*)&mTmpSubOamBuffer[numOamDrawn], 106 + 16, 146, 32, 16, 2, 8192 + 2 * 256);
	numOamDrawn += MakeTextCell((GXOamAttr*)&mTmpSubOamBuffer[numOamDrawn], 174, 146, 64, 16, 2, 8192 + 3 * 256);
	numOamDrawn += MakeTextCell((GXOamAttr*)&mTmpSubOamBuffer[numOamDrawn], 64, 0, 128, 32, 2, 8192 + 5 * 256);
	const NNSG2dCellData* pButton = NNS_G2dGetCellDataByIdx(mCellDataSubBank, 0);
	trans.x = 14 * FX32_ONE;
	trans.y = 144 * FX32_ONE;
	numOamDrawn += NNS_G2dMakeCellToOams(&mTmpSubOamBuffer[numOamDrawn], 128 - numOamDrawn, pButton, NULL, &trans, -1, FALSE);
	trans.x = 106 * FX32_ONE;
	trans.y = 144 * FX32_ONE;
	numOamDrawn += NNS_G2dMakeCellToOams(&mTmpSubOamBuffer[numOamDrawn], 128 - numOamDrawn, pButton, NULL, &trans, -1, FALSE);
	trans.x = 174 * FX32_ONE;
	trans.y = 144 * FX32_ONE;
	numOamDrawn += NNS_G2dMakeCellToOams(&mTmpSubOamBuffer[numOamDrawn], 128 - numOamDrawn, pButton, NULL, &trans, -1, FALSE);
	const NNSG2dCellData* pPane = NNS_G2dGetCellDataByIdx(mCellDataSubBank, 1);
	trans.x = 92 * FX32_ONE;
	trans.y = 36 * FX32_ONE;
	numOamDrawn += NNS_G2dMakeCellToOams(&mTmpSubOamBuffer[numOamDrawn], 128 - numOamDrawn, pPane, NULL, &trans, -1, FALSE);

	MtxFx22 mtx = { FX32_ONE, 0, 0, FX32_ONE };
	const u16 affineIdx = NNS_G2dEntryOamManagerAffine(&mSubObjOamManager, &mtx);

	const NNSG2dCellData* pKnob = NNS_G2dGetCellDataByIdx(mCellDataSubBank, 2);
	trans.x = 46 * FX32_ONE;
	trans.y = 91 * FX32_ONE;
	numOamDrawn += NNS_G2dMakeCellToOams(&mTmpSubOamBuffer[numOamDrawn], 128 - numOamDrawn, pKnob, &mtx, &trans, affineIdx, TRUE);
	NNS_G2dEntryOamManagerOam(&mSubObjOamManager, &mTmpSubOamBuffer[0], numOamDrawn);
}

void Game::OnSub3DCopyVAlarm()
{
	//this might cause glitches, because we update in the middle of a frame
	MI_DmaCopy32Async(0, (void*)HW_LCDC_VRAM_B, (void*)(HW_DB_BG_VRAM + 0x2000), 128 * 96 * 2, NULL, NULL);
}

void Game::VBlank()
{
	//handle it as early as possible to prevent problems with the shared vram d
	HandlePickingVBlank();
	mLastFrameType = mCurFrameType;
	NNS_G2dApplyOamManagerToHW(&mSubObjOamManager);
	NNS_G2dResetOamManagerBuffer(&mSubObjOamManager);
	if (mCurFrameType == FRAME_TYPE_MAIN_PICKING)//mPickingState == PICKING_STATE_RENDERING)
	{
		GX_SetBankForLCDC(GX_GetBankForLCDC() | GX_VRAM_LCDC_B | GX_VRAM_LCDC_D);
		GX_SetGraphicsMode(GX_DISPMODE_VRAM_D, GX_BGMODE_0, GX_BG0_AS_3D);
		//Capture the picking data
		GX_SetCapture(GX_CAPTURE_SIZE_256x192, GX_CAPTURE_MODE_A, GX_CAPTURE_SRCA_3D, (GXCaptureSrcB)0, GX_CAPTURE_DEST_VRAM_B_0x00000, 16, 0);
		mCurFrameType = FRAME_TYPE_MAIN_FAR;
	}
	else if (mCurFrameType == FRAME_TYPE_MAIN_FAR)//mRenderState == 0)
	{
		GX_SetBankForLCDC(GX_GetBankForLCDC() | GX_VRAM_LCDC_B | GX_VRAM_LCDC_D);
		GX_SetGraphicsMode(GX_DISPMODE_VRAM_D, GX_BGMODE_0, GX_BG0_AS_3D);
		GX_SetVisiblePlane(GX_PLANEMASK_BG0);
		GX_SetCapture(GX_CAPTURE_SIZE_256x192, GX_CAPTURE_MODE_A, GX_CAPTURE_SRCA_3D, (GXCaptureSrcB)0, GX_CAPTURE_DEST_VRAM_B_0x00000, 16, 0);
		mCurFrameType = FRAME_TYPE_MAIN_NEAR;
	}
	else if (mCurFrameType == FRAME_TYPE_MAIN_NEAR)
	{
		GX_SetBankForLCDC(GX_GetBankForLCDC() | GX_VRAM_LCDC_D);
		GX_SetBankForBG(GX_VRAM_BG_128_B);
		GX_SetGraphicsMode(GX_DISPMODE_GRAPHICS, GX_BGMODE_5, GX_BG0_AS_3D);
		GX_SetVisiblePlane(GX_PLANEMASK_BG0 | GX_PLANEMASK_BG3 | GX_PLANEMASK_OBJ);
		G2_SetBG3ControlDCBmp(GX_BG_SCRSIZE_DCBMP_256x256, GX_BG_AREAOVER_XLU, GX_BG_BMPSCRBASE_0x00000);
		G2_SetBG3Priority(3);
		G2_SetBG0Priority(0);
		GX_SetCapture(GX_CAPTURE_SIZE_256x192, GX_CAPTURE_MODE_A, GX_CAPTURE_SRCA_2D3D, (GXCaptureSrcB)0, GX_CAPTURE_DEST_VRAM_D_0x00000, 16, 0);
		mCurFrameType = FRAME_TYPE_MAIN_FAR;
	}
	else if (mCurFrameType == FRAME_TYPE_SUB)
	{
		GX_SetBankForLCDC(GX_GetBankForLCDC() | GX_VRAM_LCDC_B | GX_VRAM_LCDC_D);
		GX_SetGraphicsMode(GX_DISPMODE_VRAM_D, GX_BGMODE_0, GX_BG0_AS_3D);
		GX_SetCapture(GX_CAPTURE_SIZE_128x128, GX_CAPTURE_MODE_A, GX_CAPTURE_SRCA_3D, (GXCaptureSrcB)0, GX_CAPTURE_DEST_VRAM_B_0x00000, 16, 0);
		mCurFrameType = FRAME_TYPE_MAIN_FAR;
		//todo: remove the need for reallocation
		if (mSub3DCopyVAlarm != NULL)
			delete mSub3DCopyVAlarm;
		mSub3DCopyVAlarm = new OS::VAlarm();
		mSub3DCopyVAlarm->Set(96, 5, Game::OnSub3DCopyVAlarm, this);
	}
}

void Game::Finalize()
{
	//We don't have to free resources here, because that's done by using the group id
	delete mVRAMCopyVAlarm;
	delete mGameController;
}