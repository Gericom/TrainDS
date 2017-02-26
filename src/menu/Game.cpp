#include <nitro.h>
#include <nnsys/g3d.h>
#include <stdlib.h>
#include "core.h"
#include "util.h"
#include "Menu.h"
#include "terrain/terrain.h"
#include "terrain/Map.h"
#include "terrain/TerrainManager.h"
#include "terrain/track/TrackPieceEx.h"
#include "terrain/track/FlexTrack.h"
#include "terrain/scenery/RCT2Tree1.h"
#include "terrain/managers/TerrainTextureManager.h"
#include "engine/PathWorker.h"
#include "vehicles/train.h"
#include "ui/UIManager.h"
#include "engine/Camera.h"
#include "engine/LookAtCamera.h"
#include "engine/ThirdPersonCamera.h"
#include "engine/FreeRoamCamera.h"
#include "inih/INIReader.h"
#include "terrain/managers/SfxManager.h"
#include "tools/DragTool.h"
#include "tools/AddTrackTool.h"
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

	GX_SetBankForLCDC(GX_VRAM_LCDC_D);

	GX_SetBankForOBJ(GX_VRAM_OBJ_16_F);

	G3X_Init();
	G3X_InitMtxStack();
	GX_SetBankForTex(GX_VRAM_TEX_012_ABC);
	GX_SetBankForTexPltt(GX_VRAM_TEXPLTT_0123_E);

	GX_SetGraphicsMode(GX_DISPMODE_GRAPHICS, GX_BGMODE_3, GX_BG0_AS_3D);
	GX_SetVisiblePlane(GX_PLANEMASK_BG0 | GX_PLANEMASK_OBJ);
	GXS_SetGraphicsMode(GX_BGMODE_0);
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

	//mTerrainManager = new TerrainManager();
	mMap = new Map();
	//mMap->GenerateLandscape();
	//mMap->GenerateTrees();

	//MI_CpuClearFast(&sDummyMap[0][0], sizeof(sDummyMap));

	/*for (int y = 0; y < 64; y++)
	{
		for (int x = 0; x < 64; x++)
		{
			if (y == 32 - 3 && x > 32 - 2 && x < 32)
			{
				mMap->mTiles[y][x].groundType = 0;//1;
				mMap->mTiles[y][x].y = 1;
			}
			else if (y == 32 - 3 && x > 32 - 2 && x > 32) mMap->mTiles[y][x].groundType = 0;//1;
			else if (y == 32 - 2 && x > 32 - 2 && x < 32)
			{
				mMap->mTiles[y][x].ltCorner = TILE_CORNER_UP;
				mMap->mTiles[y][x].rtCorner = TILE_CORNER_UP;
				mMap->mTiles[y][x].lbCorner = TILE_CORNER_FLAT;
				mMap->mTiles[y][x].rbCorner = TILE_CORNER_FLAT;
			}
			else if (y == 32 - 4 && x > 32 - 2 && x < 32)
			{
				mMap->mTiles[y][x].ltCorner = TILE_CORNER_FLAT;
				mMap->mTiles[y][x].rtCorner = TILE_CORNER_FLAT;
				mMap->mTiles[y][x].lbCorner = TILE_CORNER_UP;
				mMap->mTiles[y][x].rbCorner = TILE_CORNER_UP;
			}
		}
	}*/
	/*mMap->mTiles[32 - 3][32].ltCorner = TILE_CORNER_UP;
	mMap->mTiles[32 - 3][32].lbCorner = TILE_CORNER_UP;
	mMap->mTiles[32 - 3][32].groundType = 0;//1;
	mMap->mTiles[32 - 4][32].lbCorner = TILE_CORNER_UP;
	mMap->mTiles[32 - 2][32].ltCorner = TILE_CORNER_UP;
	mMap->mTiles[32 - 2][32 - 2].rtCorner = TILE_CORNER_UP;
	mMap->mTiles[32 - 4][32 - 2].rbCorner = TILE_CORNER_UP;
	mMap->mTiles[32 - 3][32 - 2].rtCorner = TILE_CORNER_UP;
	mMap->mTiles[32 - 3][32 - 2].rbCorner = TILE_CORNER_UP;*/
	/*sDummyPieces[0] = new TrackPieceStraight1x1(32 - 1, 0, 32 - 1, TRACKPIECE_ROT_0);
	sDummyPieces[1] = new TrackPieceQuarterCircle2x2(32, 0, 32 - 1, TRACKPIECE_ROT_0);
	sDummyPieces[2] = new TrackPieceStraight1x1(32 + 1, 0, 32 - 3, TRACKPIECE_ROT_90);
	sDummyPieces[3] = new TrackPieceStraight1x1(32 + 1, 0, 32 - 4, TRACKPIECE_ROT_90);
	sDummyPieces[4] = new TrackPieceQuarterCircle2x2(32 + 1, 0, 32 - 5, TRACKPIECE_ROT_90);
	sDummyPieces[5] = new TrackPieceStraight1x1(32 - 1, 0, 32 - 6, TRACKPIECE_ROT_180);
	sDummyPieces[6] = new TrackPieceQuarterCircle2x2(32 - 2, 0, 32 - 6, TRACKPIECE_ROT_180);
	sDummyPieces[7] = new TrackPieceStraight1x1(32 - 3, 0, 32 - 4, TRACKPIECE_ROT_270);
	sDummyPieces[8] = new TrackPieceStraight1x1(32 - 3, 0, 32 - 3, TRACKPIECE_ROT_270);
	sDummyPieces[9] = new TrackPieceQuarterCircle2x2(32 - 3, 0, 32 - 2, TRACKPIECE_ROT_270);*/


	/*for (int i = 0; i < 20; i++)
	{
		mMap->mTiles[32 - 3][2 + i].ltCorner = TILE_CORNER_UP;
		mMap->mTiles[32 - 3][2 + i].rtCorner = TILE_CORNER_UP;
		mMap->mTiles[32 - 4][2 + i].y = 1;
		mMap->mTiles[32 - 4][2 + i].ltCorner = TILE_CORNER_UP;
		mMap->mTiles[32 - 4][2 + i].rtCorner = TILE_CORNER_UP;
		mMap->mTiles[32 - 5][2 + i].y = 2;
		mMap->mTiles[32 - 6][2 + i].y = 2;
		mMap->mTiles[32 - 7][2 + i].y = 2;
		mMap->mTiles[32 - 8][2 + i].y = 2;
		mMap->mTiles[32 - 9][2 + i].y = 2;
		mMap->mTiles[32 - 10][2 + i].y = 2;
	}*/
	VecFx32 a = { 64 * FX32_ONE - 32 * FX32_ONE, 0, 2 * FX32_ONE - 32 * FX32_ONE };
	VecFx32 b = { 64 * FX32_ONE - 32 * FX32_ONE, 0, (2 + 10) * FX32_ONE - 32 * FX32_ONE };
	sDummyPieces[0] = new FlexTrack(&a, &b);

	/*for (int i = 0; i < 120; i++)
	{
		sDummyPieces[i] = new TrackPieceStraight1x1(32 + 32, 0, 2 + i, TRACKPIECE_ROT_90);
		//mMap->AddTrackPiece(new TrackPieceStraight1x1(31 + 32, 0, 2 + i, TRACKPIECE_ROT_90));
	}

	for (int i = 0; i < 120; i++)
	{
		sDummyPieces[i]->mPrev[0] = (i == 0) ? NULL : sDummyPieces[i - 1];
		sDummyPieces[i]->mPrev[1] = sDummyPieces[i]->mPrev[2] = sDummyPieces[i]->mPrev[3] = NULL;
		sDummyPieces[i]->mNext[0] = (i == 119) ? NULL : sDummyPieces[i + 1];
		sDummyPieces[i]->mNext[1] = sDummyPieces[i]->mNext[2] = sDummyPieces[i]->mNext[3] = NULL;
	}

	for (int i = 0; i < 120; i++)
	{
		//mMap->AddTrackPiece(sDummyPieces[i]);
	}*/

	//mMap->AddSceneryObject(new RCT2Tree1(32 - 1, 1, 32 - 3, 0));

	mTrain.firstPart = &mTrainPart;
	mTrain.isDriving = FALSE;
	mTrain.firstPart->pathWorker1 = new PathWorker(sDummyPieces[0], 0, 0, mMap);
	mTrain.firstPart->pathWorker2 = new PathWorker(sDummyPieces[0], 0, FX32_ONE, mMap);
	mTrain.firstPart->next = NULL;

	mLocModel = (NNSG3dResFileHeader*)Util_LoadFileToBuffer("/data/locomotives/atsf_f7/low.nsbmd", NULL, false);
	NNS_G3dResDefaultSetup(mLocModel);
	NNSG3dResFileHeader* mLocTextures = (NNSG3dResFileHeader*)Util_LoadFileToBuffer("/data/locomotives/atsf_f7/low.nsbtx", NULL, true);
	NNS_G3dResDefaultSetup(mLocTextures);
	NNSG3dResMdl* model = NNS_G3dGetMdlByIdx(NNS_G3dGetMdlSet(mLocModel), 0);
	NNS_G3dMdlSetMdlLightEnableFlagAll(model, GX_LIGHTMASK_0);
	NNS_G3dMdlSetMdlDiffAll(model, GX_RGB(21, 21, 21));
	NNS_G3dMdlSetMdlAmbAll(model, GX_RGB(15, 15, 15));
	NNS_G3dMdlSetMdlSpecAll(model, GX_RGB(0, 0, 0));
	NNS_G3dMdlSetMdlEmiAll(model, GX_RGB(0, 0, 0));
	NNS_G3dMdlSetMdlFogEnableFlagAll(model, true);
	NNSG3dResTex* tex = NNS_G3dGetTex(mLocTextures);
	NNS_G3dBindMdlSet(NNS_G3dGetMdlSet(mLocModel), tex);
	NNS_G3dRenderObjInit(&mTrain.firstPart->renderObj, model);
	NNS_FndFreeToExpHeap(gHeapHandle, mLocTextures);

	GX_SetOBJVRamModeChar(GX_OBJVRAMMODE_CHAR_1D_32K);

	mFontData = Util_LoadFileToBuffer("/data/fonts/droid_sans_mono_10pt.NFTR", NULL, false);
	MI_CpuClear8(&mFont, sizeof(mFont));
	NNS_G2dFontInitAuto(&mFont, mFontData);

	NNS_G2dCharCanvasInitForOBJ1D(&mCanvas, (uint8_t*)G2_GetOBJCharPtr(), 8, 4, NNS_G2D_CHARA_COLORMODE_16);
	NNS_G2dTextCanvasInit(&mTextCanvas, &mCanvas, &mFont, 0, 1);
	NNS_G2dCharCanvasClear(&mCanvas, 0);
	NNS_G2dTextCanvasDrawTextRect(
		&mTextCanvas, 0, 0, 64, 32, 1, NNS_G2D_VERTICALORIGIN_TOP | NNS_G2D_HORIZONTALORIGIN_LEFT | NNS_G2D_HORIZONTALALIGN_CENTER | NNS_G2D_VERTICALALIGN_MIDDLE, (NNSG2dChar*)L"Tri's Test");

	NNS_G2dInitOamManagerModule();

	for(int i = 0; i < 16; i++)
		((uint16_t*)HW_OBJ_PLTT)[i] = 0x7FFF;
	((uint16_t*)HW_OBJ_PLTT)[0] = 0;

	G2_SetOBJAttr(&GXOamAttrArray[0], 0, 0, 0, GX_OAM_MODE_NORMAL, FALSE, GX_OAM_EFFECT_NONE, GX_OAM_SHAPE_64x32, GX_OAM_COLORMODE_16, 0, 0, 0);

	mUIManager = new UIManager(this);
	//mTrackBuildUISlice = new TrackBuildUISlice();
	//mUIManager->AddSlice(mTrackBuildUISlice);
	mUIManager->RegisterPenCallbacks(Game::OnPenDown, Game::OnPenMove, Game::OnPenUp);

	NNS_SndArcLoadSeqArc(SEQ_TRAIN, gSndHeapHandle);
	NNS_SndArcLoadBank(BANK_TRAIN, gSndHeapHandle);
	NNS_SndArcPlayerStartSeqArc(&mTrain.trackSoundHandle, SEQ_TRAIN, TRAIN_TRACK);

	mPickingCallback = NULL;

	//mCamera = new ThirdPersonCamera();//LookAtCamera();
	//mCamera->mTrain = &mTrain;
	Train_UpdatePos(&mTrain);
	mCamera = new FreeRoamCamera();
	mCamera->mDestination = mTrain.firstPart->position;
	mCamera->mDestination.x -= 32 * FX32_ONE;
	mCamera->mDestination.z -= 32 * FX32_ONE;
	VecFx32 camRot = { 0, 22 * FX32_ONE, 0 };
	mCamera->SetRotation(&camRot);
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

	NNS_G3dGlbPerspectiveW(FX32_SIN30, FX32_COS30, (256 * 4096 / 192), 4096 >> 3, /*64*//*18*//*24*//*31*/8 * 4096, 40960 * 4);
	setup_normals();

	mSfxManager = new SfxManager();
	mDragTool = new AddTrackTool(this);

	OS_CreateVAlarm(&mVRAMCopyVAlarm);
	OS_SetPeriodicVAlarm(&mVRAMCopyVAlarm, 192 - 48, 5, Game::OnVRAMCopyVAlarm, this);
}

static int mVRAMReadyLine;

void Game::OnVRAMCopyVAlarm()
{
	mMap->mTerrainTextureManager->UpdateVramC();
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
		mPickingResult = ((picking_result_t*)HW_LCDC_VRAM_D)[mPickingPointX + mPickingPointY * 256];
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
	if (mPickingState == PICKING_STATE_READY && mPickingRequested)
	{
		reg_G3X_DISP3DCNT = reg_G3X_DISP3DCNT & ~REG_G3X_DISP3DCNT_TME_MASK;
		G3X_SetClearColor(0, 31, 0x7fff, 0, FALSE);
		G3X_EdgeMarking(FALSE);
		G3X_AntiAlias(FALSE);
		G3X_SetFog(FALSE, GX_FOGBLEND_COLOR_ALPHA, GX_FOGSLOPE_0x0020, 0);
		mPickingRequested = false;
		mPickingState = PICKING_STATE_RENDERING;
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

void Game::Render()
{
	HandlePickingEarly();

	mUIManager->ProcessInput();
	u16 keyData = PAD_Read();

	HandlePickingLate();

	if (keyData & PAD_BUTTON_B)
	{
		mTrain.isDriving = TRUE;
		mTrain.isDrivingBackwards = FALSE;
	}
	else
		mTrain.isDriving = FALSE;

	/*if (keyData & PAD_BUTTON_A)
	{
		mTrain.isDriving = TRUE;
		mTrain.isDrivingBackwards = FALSE;
	}
	else if (keyData & PAD_BUTTON_B)
	{
		mTrain.isDriving = TRUE;
		mTrain.isDrivingBackwards = TRUE;
	}
	else *///mTrain.isDriving = FALSE;
	if (!mKeyTimer)
	{
		if (keyData & PAD_BUTTON_X)
		{
			mMap->SetGridEnabled(!mMap->GetGridEnabled());
			mKeyTimer = 10;
		}
		if (keyData & PAD_BUTTON_Y)
		{
			mAntiAliasEnabled = !mAntiAliasEnabled;
			mKeyTimer = 10;
		}
		if (keyData & PAD_BUTTON_SELECT)
		{
			if (mMap->GetFirstTrackPiece() != NULL)
			{
				mTrain.firstPart->pathWorker1 = new PathWorker(mMap->GetFirstTrackPiece(), 0, 0, mMap);
				mTrain.firstPart->pathWorker2 = new PathWorker(mMap->GetFirstTrackPiece(), 0, FX32_ONE, mMap);
			}
			mKeyTimer = 10;
		}
	}
	else
		mKeyTimer--;
	/*if (keyData & PAD_KEY_LEFT)
		mCamera->mTheta -= FX32_ONE;
	else if (keyData & PAD_KEY_RIGHT)
		mCamera->mTheta += FX32_ONE;
	if (keyData & PAD_KEY_UP)
		mCamera->mPhi += FX32_ONE >> 5;
	else if (keyData & PAD_KEY_DOWN)
		mCamera->mPhi -= FX32_ONE >> 5;
	mCamera->mTheta %= 360 * FX32_ONE;
	if (mCamera->mPhi < 0)
		mCamera->mPhi = 0;
	else if (mCamera->mPhi > 10 * FX32_ONE)
		mCamera->mPhi = 10 * FX32_ONE;
	if (keyData & PAD_BUTTON_L && mCamera->mRadius < 4 * FX32_ONE)
		mCamera->mRadius += FX32_ONE >> 5;
	else if (keyData & PAD_BUTTON_R && mCamera->mRadius > FX32_HALF)
		mCamera->mRadius -= FX32_ONE >> 5;
	if (mCamera->mRadius < FX32_ONE + FX32_HALF)
		mCamera->mRadius = FX32_ONE + FX32_HALF;*/

	VecFx32 camRot;
	mCamera->GetRotation(&camRot);
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
		if (keyData & PAD_BUTTON_L)
			mCamera->MoveY(-FX32_ONE >> 5);
		else if (keyData & PAD_BUTTON_R)
			mCamera->MoveY(FX32_ONE >> 5);
	}
	else
	{
		if (keyData & PAD_KEY_LEFT)
			mCamera->MoveX(-FX32_ONE / 50);// 24);
		else if (keyData & PAD_KEY_RIGHT)
			mCamera->MoveX(FX32_ONE / 50);// 24);
		if (keyData & PAD_KEY_UP)
			mCamera->MoveZ(FX32_ONE / 50);// 24);
		else if (keyData & PAD_KEY_DOWN)
			mCamera->MoveZ(-FX32_ONE / 50);// 24);
		if (keyData & PAD_BUTTON_L)
			camRot.x -= FX32_ONE >> 1;
		else if (keyData & PAD_BUTTON_R)
			camRot.x += FX32_ONE >> 1;
	}

	mCamera->SetRotation(&camRot);

	if (keyData & PAD_BUTTON_START)
		Game::GotoMenu();
	G3X_Reset();
	if (mPickingState != PICKING_STATE_RENDERING)
	{
		reg_G3X_DISP3DCNT = reg_G3X_DISP3DCNT | REG_G3X_DISP3DCNT_TME_MASK;
		G3X_SetClearColor(GX_RGB(119 >> 3, 199 >> 3, 244 >> 3), 31, 0x7fff, 0, true);
		G3X_SetShading(GX_SHADING_HIGHLIGHT);
		G3X_EdgeMarking(true);
		G3X_AntiAlias(mAntiAliasEnabled);
		G3X_SetFog(true, GX_FOGBLEND_COLOR_ALPHA, GX_FOGSLOPE_0x0400, 0x8000 - 0x100);
		G3X_SetFogColor(GX_RGB(119 >> 3, 199 >> 3, 244 >> 3), 25);
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
	Train_UpdatePos(&mTrain);
#ifdef FIRST_PERSON
	mCamera->mPosition.x = tpos.x - 8 * FX32_ONE;// + 0.4 * dir.x;
	mCamera->mPosition.y = tpos.y + FX32_HALF + (FX32_HALF >> 1);// + 0.4 * dir.y;
	mCamera->mPosition.z = tpos.z - 8 * FX32_ONE;// + 0.4 * dir.z;
	mCamera->mDestination.x = tpos.x - 8 * FX32_ONE + 4 * dir.x;
	mCamera->mDestination.y = tpos.y + FX32_HALF + 4 * dir.y;// + 1 * FX32_ONE;
	mCamera->mDestination.z = tpos.z - 8 * FX32_ONE + 4 * dir.z;
#endif
	mCamera->Apply();

	Train_UpdateSound(&mTrain, mCamera);

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
	if (mPickingState == PICKING_STATE_RENDERING)
	{
		NNS_G3dGlbMaterialColorDiffAmb(GX_RGB(0, 0, 0), GX_RGB(0, 0, 0), false);
		NNS_G3dGlbMaterialColorSpecEmi(GX_RGB(0, 0, 0), GX_RGB(0, 0, 0), false);
	}
	else
	{
		NNS_G3dGlbMaterialColorDiffAmb(GX_RGB(31, 31, 31), /*GX_RGB(5, 5, 5)*/GX_RGB(10, 10, 10), false);
		NNS_G3dGlbMaterialColorSpecEmi(/*GX_RGB(3, 3, 3)*/GX_RGB(1, 1, 1), GX_RGB(0, 0, 0), false);
		//NNS_G3dGlbMaterialColorDiffAmb(GX_RGB(20, 12, 3), GX_RGB(5, 5, 5), FALSE);
		//NNS_G3dGlbMaterialColorSpecEmi(GX_RGB(31, 26, 22), GX_RGB(0, 0, 0), FALSE);
		//NNS_G3dGlbMaterialColorDiffAmb(GX_RGB(31, 31, 31), GX_RGB(31, 31, 31), FALSE);
		//NNS_G3dGlbMaterialColorSpecEmi(GX_RGB(5, 5, 5), GX_RGB(0, 0, 0), FALSE);
	}

	//lod0
	NNS_G3dGlbPerspectiveW(FX32_SIN30, FX32_COS30, (256 * 4096 / 192), 4096 >> 3, /*64*//*18*//*24*//*31*/10 * 4096, 40960 * 4);

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

	NOCASH_Printf("lod0: (%d-%d, %d-%d)", xstart, xend, zstart, zend);

	//what actually visible is
	NNS_G3dGlbPerspectiveW(FX32_SIN30, FX32_COS30, (256 * 4096 / 192), 4096 >> 3, /*64*//*18*//*24*/30 * 4096, 40960 * 4);

	CalculateVisibleGrid(&bbmin, &bbmax);

	int xstart2 = (bbmin.x - 2 * FX32_ONE - FX32_HALF) / FX32_ONE + 32;
	xstart2 = MATH_CLAMP(xstart2, 0, 128);
	int zstart2 = (bbmin.z - 2 * FX32_ONE - FX32_HALF) / FX32_ONE + 32;
	zstart2 = MATH_CLAMP(zstart2, 0, 128);

	int xend2 = (bbmax.x + 2 * FX32_ONE + FX32_HALF) / FX32_ONE + 32;
	xend2 = MATH_CLAMP(xend2, 0, 128);
	int zend2 = (bbmax.z + 2 * FX32_ONE + FX32_HALF) / FX32_ONE + 32;
	zend2 = MATH_CLAMP(zend2, 0, 128);

	NOCASH_Printf("lod1: (%d-%d, %d-%d)", xstart2, xend2, zstart2, zend2);



	NNS_G3dGlbFlushP();
	NNS_G3dGeFlushBuffer();

	//NOCASH_Printf("x: %d; %d", xstart, xend);
	//NOCASH_Printf("z: %d; %d", zstart, zend);

	if (mPickingState == PICKING_STATE_RENDERING)
	{
		mPickingXStart = xstart;
		mPickingXEnd = xend;
		mPickingZStart = zstart;
	}

	G3_PushMtx();
	{
		VecFx32 camDir;
		mCamera->GetLookDirection(&camDir);
		mMap->Render(xstart, xend, zstart, zend, xstart2, xend2, zstart2, zend2, mPickingState == PICKING_STATE_RENDERING, mSelectedMapX, mSelectedMapZ, &mCamera->mPosition, &camDir);
		NNS_G3dGePushMtx();
		{
			NNS_G3dGeTranslateVec(&mTrain.firstPart->position);
			//calculate rotation matrix
			VecFx32 up = { 0, FX32_ONE, 0 };
			VecFx32 dir = mTrain.firstPart->direction;

			dir.y = -dir.y;

			VecFx32 xaxis;
			VEC_CrossProduct(&up, &dir, &xaxis);
			VEC_Normalize(&xaxis, &xaxis);

			VecFx32 yaxis;
			VEC_CrossProduct(&dir, &xaxis, &yaxis);
			VEC_Normalize(&yaxis, &yaxis);

			MtxFx33 rot2;

			rot2._00 = xaxis.x;
			rot2._01 = yaxis.x;
			rot2._02 = dir.x;

			rot2._10 = xaxis.y;
			rot2._11 = yaxis.y;
			rot2._12 = dir.y;

			rot2._20 = xaxis.z;
			rot2._21 = yaxis.z;
			rot2._22 = dir.z;

			MTX_Inverse33(&rot2, &rot2);

			NNS_G3dGeMultMtx33(&rot2);

			NNS_G3dGeMtxMode(GX_MTXMODE_POSITION);
			NNS_G3dGeScale(FX32_ONE / 13, FX32_ONE / 13, FX32_ONE / 13);
			NNS_G3dGeMtxMode(GX_MTXMODE_POSITION_VECTOR);

			if (mPickingState == PICKING_STATE_RENDERING)
			{
				NNS_G3dMdlSetMdlDiffAll(mTrain.firstPart->renderObj.resMdl, GX_RGB(0, 0, 0));
				NNS_G3dMdlSetMdlAmbAll(mTrain.firstPart->renderObj.resMdl, GX_RGB(0, 0, 0));
				NNS_G3dMdlSetMdlSpecAll(mTrain.firstPart->renderObj.resMdl, GX_RGB(0, 0, 0));
				NNS_G3dMdlSetMdlEmiAll(mTrain.firstPart->renderObj.resMdl, PICKING_COLOR(PICKING_TYPE_TRAIN, 0 + 1));
			}
			else
			{
				if (mSelectedTrain == 0) NNS_G3dMdlSetMdlDiffAll(mTrain.firstPart->renderObj.resMdl, sDiffSelectionColorTable[1]);
				else NNS_G3dMdlSetMdlDiffAll(mTrain.firstPart->renderObj.resMdl, GX_RGB(21, 21, 21));
				if (mSelectedTrain == 0) NNS_G3dMdlSetMdlAmbAll(mTrain.firstPart->renderObj.resMdl, sAmbSelectionColorTable[1]);
				else NNS_G3dMdlSetMdlAmbAll(mTrain.firstPart->renderObj.resMdl, GX_RGB(15, 15, 15));
				NNS_G3dMdlSetMdlSpecAll(mTrain.firstPart->renderObj.resMdl, GX_RGB(0, 0, 0));
				NNS_G3dMdlSetMdlEmiAll(mTrain.firstPart->renderObj.resMdl, GX_RGB(0, 0, 0));
				if (mSelectedTrain == 0) NNS_G3dMdlSetMdlPolygonIDAll(mTrain.firstPart->renderObj.resMdl, 8);
				else NNS_G3dMdlSetMdlPolygonIDAll(mTrain.firstPart->renderObj.resMdl, 0);
			}
			NNS_G3dDraw(&mTrain.firstPart->renderObj);
		}
		NNS_G3dGePopMtx(1);
		NNS_G3dGeFlushBuffer();
	}
	G3_PopMtx(1);
	mUIManager->Render();
	char result[64];
	MI_CpuClear8(result, sizeof(result));
	OS_SPrintf(result, "%d;%d;%d", G3X_GetVtxListRamCount(), G3X_GetPolygonListRamCount(), mVRAMReadyLine);
	u16 result2[64];
	MI_CpuClear8(result2, sizeof(result2));
	for (int i = 0; i < sizeof(result); i++)
	{
		result2[i] = result[i];
	}
	NNS_G2dCharCanvasClear(&mCanvas, 0);
	NNS_G2dTextCanvasDrawTextRect(
		&mTextCanvas, 0, 0, 64, 32, 1, NNS_G2D_VERTICALORIGIN_TOP | NNS_G2D_HORIZONTALORIGIN_LEFT | NNS_G2D_HORIZONTALALIGN_CENTER | NNS_G2D_VERTICALALIGN_MIDDLE, (NNSG2dChar*)result2);// (NNSG2dChar*)L"Tri's Test");
	G3_SwapBuffers(SWAP_BUFFERS_SORTMODE, SWAP_BUFFERS_BUFFERMODE);
	mSfxManager->Update();
}

void Game::VBlank()
{
	//handle it as early as possible to prevent problems with the shared vram d
	HandlePickingVBlank();
	if (mPickingState == PICKING_STATE_RENDERING)
	{
		GX_SetGraphicsMode(GX_DISPMODE_VRAM_D, GX_BGMODE_0, GX_BG0_AS_3D);
		//Capture the picking data
		GX_SetCapture(GX_CAPTURE_SIZE_256x192, GX_CAPTURE_MODE_A, GX_CAPTURE_SRCA_3D, (GXCaptureSrcB)0, GX_CAPTURE_DEST_VRAM_D_0x00000, 16, 0);
	}
	else
	{
		GX_SetGraphicsMode(GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BG0_AS_3D);
		GX_SetVisiblePlane(GX_PLANEMASK_BG0 | GX_PLANEMASK_OBJ);
		//capture to be able to react as fast as possible on a touch (we use this image to hide the picking)
		GX_SetCapture(GX_CAPTURE_SIZE_256x192, GX_CAPTURE_MODE_A, GX_CAPTURE_SRCA_2D3D, (GXCaptureSrcB)0, GX_CAPTURE_DEST_VRAM_D_0x00000, 16, 0);
	}
	//mUIManager->VBlankProc();
}

void Game::Finalize()
{
	//We don't have to free resources here, because that's done by using the group id
	OS_CancelVAlarm(&mVRAMCopyVAlarm);
}