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
#include "ui/components/Button.h"
#include "engine/cameras/Camera.h"
#include "engine/cameras/LookAtCamera.h"
#include "engine/cameras/FreeRoamCamera.h"
#include "inih/INIReader.h"
#include "tools/DragTool.h"
#include "tools/AddTrackTool.h"
#include "vehicles/Wagon.h"
#include "Game.h"
#include "Loader.h"

#define SWAP_BUFFERS_SORTMODE	GX_SORTMODE_MANUAL //AUTO
#define SWAP_BUFFERS_BUFFERMODE	GX_BUFFERMODE_Z

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

void Game::Initialize(int arg)
{
	//load overlay
	LOAD_OVERLAY_ITCM(rendering_itcm);

	G3X_Init();
	G3X_InitMtxStack();
	GX_SetBankForTex(GX_VRAM_TEX_012_ABC);
	GX_SetBankForTexPltt(GX_VRAM_TEXPLTT_0123_E);

	NNS_GfdResetLnkTexVramState();
	NNS_GfdResetLnkPlttVramState();

	Loader* loader = new Loader();
	loader->BeginLoad();
	{
		mCamera = new FreeRoamCamera();
		mGameController = new GameController(mCamera);
		mGameController->mTrain->PutOnTrack(mGameController->mMap->GetFirstTrackPiece(), 1, 260 * FX32_ONE);
	}
	loader->FinishLoad();
	loader->WaitFinish();
	delete loader;

	GX_SetBankForLCDC(GX_VRAM_LCDC_D);

	GX_SetBankForOBJ(GX_VRAM_OBJ_16_F);
	GX_SetBankForBG(GX_VRAM_BG_16_G);

	GX_SetBankForSubBG(GX_VRAM_SUB_BG_32_H);
	GX_SetBankForSubOBJ(GX_VRAM_SUB_OBJ_16_I);

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

	NNS_G2dInitOamManagerModule();
	NNS_G2dGetNewOamManagerInstanceAsFastTransferMode(&mSubObjOamManager, 0, 128, NNS_G2D_OAMTYPE_SUB);

	mCellDataMain = Util_LoadFileToBuffer("/data/game/BottomButtonsOAM.NCER", NULL, FALSE);
	NNS_G2dGetUnpackedCellBank(mCellDataMain, &mCellDataMainBank);

	mCellDataSub = Util_LoadFileToBuffer("/data/game/OBJ.NCER", NULL, FALSE);
	NNS_G2dGetUnpackedCellBank(mCellDataSub, &mCellDataSubBank);

	NNSG2dCharacterData* charDataUnpacked;
	NNSG2dPaletteData* palDataUnpacked;
	NNSG2dScreenData* screenDataUnpacked;

	void* mCharDataSub = Util_LoadFileToBuffer("/data/game/OAM.NCGR", NULL, TRUE);
	NNS_G2dGetUnpackedCharacterData(mCharDataSub, &charDataUnpacked);
	NNS_G2dInitImageProxy(&mImageProxyMain);
	NNS_G2dLoadImage1DMapping(charDataUnpacked, 0, NNS_G2D_VRAM_TYPE_2DMAIN, &mImageProxyMain);
	NNS_FndFreeToExpHeap(gHeapHandle, mCharDataSub);

	void* mPalDataSub = Util_LoadFileToBuffer("/data/game/OAM.NCLR", NULL, TRUE);
	NNS_G2dInitImagePaletteProxy(&mImagePaletteProxyMain);
	NNS_G2dGetUnpackedPaletteData(mPalDataSub, &palDataUnpacked);
	NNS_G2dLoadPalette(palDataUnpacked, 0, NNS_G2D_VRAM_TYPE_2DMAIN, &mImagePaletteProxyMain);
	NNS_FndFreeToExpHeap(gHeapHandle, mPalDataSub);

	mCharDataSub = Util_LoadLZ77FileToBuffer("/data/game/IngameOBJ.NCGR.lz", NULL, TRUE);
	NNS_G2dGetUnpackedCharacterData(mCharDataSub, &charDataUnpacked);
	NNS_G2dInitImageProxy(&mImageProxy);
	NNS_G2dLoadImage2DMapping(charDataUnpacked, 0, NNS_G2D_VRAM_TYPE_2DSUB, &mImageProxy);
	NNS_FndFreeToExpHeap(gHeapHandle, mCharDataSub);
	
	mPalDataSub = Util_LoadFileToBuffer("/data/game/IngameOBJ.NCLR", NULL, TRUE);
	NNS_G2dInitImagePaletteProxy(&mImagePaletteProxy);
	NNS_G2dGetUnpackedPaletteData(mPalDataSub, &palDataUnpacked);
	NNS_G2dLoadPalette(palDataUnpacked, 0, NNS_G2D_VRAM_TYPE_2DSUB, &mImagePaletteProxy);
	NNS_FndFreeToExpHeap(gHeapHandle, mPalDataSub);
	
	void* mScreenDataSub = Util_LoadLZ77FileToBuffer("/data/game/BG.NSCR.lz", NULL, TRUE);
	NNS_G2dGetUnpackedScreenData(mScreenDataSub, &screenDataUnpacked);
	mCharDataSub = Util_LoadLZ77FileToBuffer("/data/game/IngameBG.NCGR.lz", NULL, TRUE);
	NNS_G2dGetUnpackedCharacterData(mCharDataSub, &charDataUnpacked);
	mPalDataSub = Util_LoadFileToBuffer("/data/game/IngameBG.NCLR", NULL, TRUE);
	NNS_G2dGetUnpackedPaletteData(mPalDataSub, &palDataUnpacked);
	NNS_G2dBGSetup(NNS_G2D_BGSELECT_SUB0, screenDataUnpacked, charDataUnpacked, palDataUnpacked, GX_BG_SCRBASE_0x0800, GX_BG_CHARBASE_0x00000);
	NNS_FndFreeToExpHeap(gHeapHandle, mScreenDataSub);
	NNS_FndFreeToExpHeap(gHeapHandle, mCharDataSub);
	NNS_FndFreeToExpHeap(gHeapHandle, mPalDataSub);

	MI_CpuClear8(&mTmpSubOamBuffer[0], sizeof(mTmpSubOamBuffer));	

	reg_G2_BLDCNT = 0x2801;

	mSubFontData = Util_LoadLZ77FileToBuffer("/data/fonts/fot_rodin_bokutoh_pro_db_9pt.NFTR.lz", NULL, false);
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

	mSubFontData2 = Util_LoadLZ77FileToBuffer("/data/fonts/fot_rodin_bokutoh_pro_b_13pt.NFTR.lz", NULL, false);
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

	mUIManager = new UIManager(UIManager::UI_MANAGER_SCREEN_MAIN);
	mUIManager->AddUIComponent(new Button(0, 0, 20, 20, NNS_G2dGetCellDataByIdx(mCellDataMainBank, 0), NNS_G2dGetCellDataByIdx(mCellDataMainBank, 1)));
	mUIManager->AddUIComponent(new Button(0, 20, 20, 20, NNS_G2dGetCellDataByIdx(mCellDataMainBank, 2), NNS_G2dGetCellDataByIdx(mCellDataMainBank, 3)));
	mUIManager->AddUIComponent(new Button(0, 40, 20, 20, NNS_G2dGetCellDataByIdx(mCellDataMainBank, 4), NNS_G2dGetCellDataByIdx(mCellDataMainBank, 5)));
	mUIManager->RegisterPenCallbacks(OnPenDown, OnPenMove, OnPenUp, this);

	NNS_SndArcLoadSeqArc(SEQ_TRAIN, gSndHeapHandle);
	NNS_SndArcLoadBank(BANK_TRAIN, gSndHeapHandle);

	mPickingCallback = NULL;

	mGameController->mTrain->GetFirstWagon()->GetPosition(&mGameController->mCamera->mDestination);
	//mCamera->mDestination.x = 373680;
	//mCamera->mDestination.y = 0;
	//mCamera->mDestination.z = 1153947;
	VecFx32 camRot = { /*-1259520, 86016, 0 };//*//*-604160, 90112, 0 };//*/0, 22 * FX32_ONE, 0 };
	mCamera->SetRotation(&camRot);

	NNS_G3dGlbPerspectiveW(FX32_SIN30, FX32_COS30, (256 * 4096 / 192), 4096 >> 3, /*64*//*18*//*24*//*31*/8 * 4096, 40960 * 4);

	InvalidateSub3D();

	mDragTool = new AddTrackTool(this);

	((vu16*)HW_BG_PLTT)[1] = 0x0421;
	((vu16*)HW_BG_PLTT)[2] = 0x0000;

	((vu32*)HW_BG_VRAM)[0] = 0x12121212;
	((vu32*)HW_BG_VRAM)[1] = 0x21212121;
	((vu32*)HW_BG_VRAM)[2] = 0x12121212;
	((vu32*)HW_BG_VRAM)[3] = 0x21212121;
	((vu32*)HW_BG_VRAM)[4] = 0x12121212;
	((vu32*)HW_BG_VRAM)[5] = 0x21212121;
	((vu32*)HW_BG_VRAM)[6] = 0x12121212;
	((vu32*)HW_BG_VRAM)[7] = 0x21212121;

	G2_SetBlendAlpha(GX_BLEND_PLANEMASK_BG1, GX_BLEND_PLANEMASK_BG0, 8, 16);
	G2_SetBG0Priority(3);
	G2_SetBG1Control(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x0800, GX_BG_CHARBASE_0x00000, GX_BG_EXTPLTT_01);

	mVRAMCopyVAlarm = new OS::VAlarm();
	mVRAMCopyVAlarm->SetPeriodic(192 - 48, 5, Game::OnVRAMCopyVAlarm, this);

	OS_SetIrqFunction(OS_IE_V_BLANK, VBlankIrqHandler);
}

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
		mPickingRequested = false;
		mPickingState = PICKING_STATE_RENDERING;
		mCurFrameType = FRAME_TYPE_MAIN_PICKING;
	}
}

void Game::OnPenDown(int x, int y)
{
	mDragTool->OnPenDown(x, y);
}

void Game::OnPenMove(int x, int y)
{
	mDragTool->OnPenMove(x, y);
}

int state = 0;
int firstX;
int firstZ;
FlexTrack* piece;

void Game::OnPenUp(int x, int y)
{
	mDragTool->OnPenUp(x, y);
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
	G3X_Reset();
	if (mSub3DInvalidated && mLastFrameType == FRAME_TYPE_MAIN_NEAR)
	{
		mSub3DInvalidated = false;
		mCurFrameType = FRAME_TYPE_SUB;
	}

	HandlePickingEarly();

	mUIManager->ProcessInput();
	Core_ReadInput();
	u16 keyData = gKeys;

	HandlePickingLate();

	if (mCurFrameType == FRAME_TYPE_MAIN_FAR)
	{
		if (keyData & PAD_BUTTON_B)
			mGameController->mTrain->SetDriving(true);
		else
			mGameController->mTrain->SetDriving(false);
		if (!mKeyTimer)
		{
			if (keyData & PAD_BUTTON_L && keyData & PAD_BUTTON_R)
			{
				GX_SetDispSelect((GX_GetDispSelect() == GX_DISP_SELECT_MAIN_SUB ? GX_DISP_SELECT_SUB_MAIN : GX_DISP_SELECT_MAIN_SUB));
				mKeyTimer = 10;
			}
			if (keyData & PAD_BUTTON_X)
			{
				mGameController->mMap->SetGridEnabled(!mGameController->mMap->GetGridEnabled());
				mKeyTimer = 5;
			}
			if (keyData & PAD_BUTTON_SELECT)
			{
				if (mGameController->mMap->GetFirstTrackPiece())
				{
					mGameController->mTrain->PutOnTrack(mGameController->mMap->GetFirstTrackPiece(), 1, 60 * FX32_ONE);
					mTrainMode = true;
				}
				mKeyTimer = 5;
			}
		}
		else
			mKeyTimer--;

		VecFx32 camRot;
		mCamera->GetRotation(&camRot);

		if (!mDebugKeyTimer)
		{
			if (keyData & PAD_BUTTON_DEBUG)
			{
				NNS_FndDumpHeap(gHeapHandle);
				//OS_Printf("Camdata: dst(%d, %d, %d), rot(%d, %d, %d)\n", mGameController->mCamera->mDestination.x, mGameController->mCamera->mDestination.y, mGameController->mCamera->mDestination.z, camRot.x, camRot.y, camRot.z);
				mDebugKeyTimer = 30;
			}
		}
		else
			mDebugKeyTimer--;

		if ((keyData & PAD_BUTTON_A) || mTrainMode)
		{
			if (keyData & PAD_KEY_LEFT)
				camRot.x += (FX32_ONE * mPassedFrameCounter) >> 1;
			else if (keyData & PAD_KEY_RIGHT)
				camRot.x -= (FX32_ONE * mPassedFrameCounter) >> 1;
			if (keyData & PAD_KEY_UP)
				camRot.y += (FX32_ONE * mPassedFrameCounter) >> 2;
			else if (keyData & PAD_KEY_DOWN)
				camRot.y -= (FX32_ONE * mPassedFrameCounter) >> 2;
			if (keyData & PAD_BUTTON_L && !(keyData & PAD_BUTTON_R))
				mCamera->MoveY(-(FX32_ONE * mPassedFrameCounter) >> 5);
			else if (keyData & PAD_BUTTON_R && !(keyData & PAD_BUTTON_L))
				mCamera->MoveY((FX32_ONE * mPassedFrameCounter) >> 5);
		}
		else
		{
			if (keyData & PAD_KEY_LEFT)
				mCamera->MoveX(-(FX32_ONE * mPassedFrameCounter) / 25);// 24);
			else if (keyData & PAD_KEY_RIGHT)
				mCamera->MoveX((FX32_ONE * mPassedFrameCounter) / 25);// 24);
			if (keyData & PAD_KEY_UP)
				mCamera->MoveZ((FX32_ONE * mPassedFrameCounter) / 25);// 24);
			else if (keyData & PAD_KEY_DOWN)
				mCamera->MoveZ(-(FX32_ONE * mPassedFrameCounter) / 25);// 24);
			if (keyData & PAD_BUTTON_L && !(keyData & PAD_BUTTON_R))
				camRot.x -= (FX32_ONE * mPassedFrameCounter) >> 1;
			else if (keyData & PAD_BUTTON_R && !(keyData & PAD_BUTTON_L))
				camRot.x += (FX32_ONE * mPassedFrameCounter) >> 1;
		}
		mCamera->SetRotation(&camRot);

		if (keyData & PAD_BUTTON_START)
			Game::GotoMenu();	

		OS_Printf("%d fps\n", 60 / mPassedFrameCounter);
		mGameController->Update(mPassedFrameCounter);
		mPassedFrameCounter = 0;
		if (mTrainMode)
		{
			mGameController->mTrain->GetFirstWagon()->GetPosition(&mGameController->mCamera->mDestination);
			mCamera->mCamDistance = FX32_CONST(1.25f);
			mCamera->mDestination.y += FX32_CONST(0.2f);
		}
	}
	if (mCurFrameType != FRAME_TYPE_SUB)
	{
		NNS_G3dGlbSetViewPort(0, 0, 255, 191);
		if (mCurFrameType == FRAME_TYPE_MAIN_PICKING)
			mGameController->Render(GameController::RENDER_MODE_PICKING);
		else
		{
			OSTick tstart = OS_GetTick();
			mGameController->Render((mCurFrameType == FRAME_TYPE_MAIN_FAR ? GameController::RENDER_MODE_FAR : GameController::RENDER_MODE_NEAR));
			OSTick diff = OS_GetTick() - tstart;
			OS_Printf("Frame %d Time: %d us\n", mCurFrameType, OS_TicksToMicroSeconds(diff));
		}
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
		mGameController->mTrain->GetPosition(&cam.mDestination);
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
		NNS_G3dGlbPerspectiveW(FX32_SIN30, FX32_COS30, (128 * 4096 / 96), 4096 >> 4, 10 * 4096, 40960 * 4);
		NNS_G3dGlbFlushP();
		NNS_G3dGeFlushBuffer();
		G3_PushMtx();
		{
			G3_Translate(-32 * FX32_ONE, 0, -32 * FX32_ONE);
			G3_Translate(0, 0, FX32_CONST(-0.2f));
			mGameController->mTrain->Render();
		}
		G3_PopMtx(1);
	}
	//InvalidateSub3D();
	//char result[64];
	//MI_CpuClear8(result, sizeof(result));
	//OS_Printf("%d: %d\n", mCurFrameType, G3X_GetPolygonListRamCount());
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
	SetSwapBuffersFlag();
	//sub screen oam
	NNSG2dFVec2 trans;
	int numOamDrawn = 0;
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
	MI_DmaCopy32Async(0, (void*)HW_LCDC_VRAM_D, (void*)(HW_DB_BG_VRAM + 0x2000), 128 * 96 * 2, NULL, NULL);
}

void Game::SetSwapBuffersFlag()
{
	OSIntrMode old = OS_DisableInterrupts();
	G3_SwapBuffers(SWAP_BUFFERS_SORTMODE, SWAP_BUFFERS_BUFFERMODE);
	mSwap = true;
	OS_RestoreInterrupts(old);
}

//on every vblank
void Game::VBlankIrq()
{
	OS_SetIrqCheckFlag(OS_IE_V_BLANK);
	mPassedFrameCounter++;
	//handle it as early as possible to prevent problems with the shared vram d
	HandlePickingVBlank();
	if (mCurFrameType == FRAME_TYPE_MAIN_PICKING)//mPickingState == PICKING_STATE_RENDERING)
	{
		GX_SetBankForLCDC(GX_GetBankForLCDC() | GX_VRAM_LCDC_B | GX_VRAM_LCDC_D);
		GX_SetGraphicsMode(GX_DISPMODE_VRAM_B, GX_BGMODE_0, GX_BG0_AS_3D);
	}
	else if (mCurFrameType == FRAME_TYPE_MAIN_FAR)//mRenderState == 0)
	{
		GX_SetBankForLCDC(GX_GetBankForLCDC() | GX_VRAM_LCDC_B | GX_VRAM_LCDC_D);
		mGameController->mDisplayFlare = true;//(((GXRgb*)HW_LCDC_VRAM_B)[mGameController->mSunY * 256 + mGameController->mSunX] & 0x7FFF) == mGameController->mSunColorMatch;
		GX_SetGraphicsMode(GX_DISPMODE_VRAM_B, GX_BGMODE_0, GX_BG0_AS_3D);
		GX_SetVisiblePlane(GX_PLANEMASK_BG0 | GX_PLANEMASK_BG1);
	}
	else if (mCurFrameType == FRAME_TYPE_MAIN_NEAR)
	{
		GX_SetBankForLCDC(GX_GetBankForLCDC() | GX_VRAM_LCDC_B);
		GX_SetBankForTex(GX_VRAM_TEX_012_ACD);
		//GX_SetBankForBG(GX_VRAM_BG_128_B);
		if (!G3X_IsGeometryBusy() && mSwap)
			GX_SetGraphicsMode(GX_DISPMODE_GRAPHICS, GX_BGMODE_5, GX_BG0_AS_3D);
		else
			GX_SetGraphicsMode(GX_DISPMODE_VRAM_B, GX_BGMODE_5, GX_BG0_AS_3D);
		GX_SetVisiblePlane(GX_PLANEMASK_BG0 /*| GX_PLANEMASK_BG3*/ | GX_PLANEMASK_OBJ);
		//G2_SetBG3ControlDCBmp(GX_BG_SCRSIZE_DCBMP_256x256, GX_BG_AREAOVER_XLU, GX_BG_BMPSCRBASE_0x00000);
		//G2_SetBG3Priority(3);
		//G2_SetBG0Priority(0);
	}
	else if (mCurFrameType == FRAME_TYPE_SUB)
	{
		GX_SetBankForLCDC(GX_GetBankForLCDC() | GX_VRAM_LCDC_B | GX_VRAM_LCDC_D);
		GX_SetGraphicsMode(GX_DISPMODE_VRAM_B, GX_BGMODE_0, GX_BG0_AS_3D);
	}
	mSwap = false;
}

//after wait for vblank and VBlankIrq
void Game::VBlank()
{
	mLastFrameType = mCurFrameType;
	mUIManager->ProcessVBlank();
	NNS_G2dApplyOamManagerToHW(&mSubObjOamManager);
	NNS_G2dResetOamManagerBuffer(&mSubObjOamManager);
	if (mCurFrameType == FRAME_TYPE_MAIN_PICKING)//mPickingState == PICKING_STATE_RENDERING)
	{
		//Capture the picking data
		GX_SetCapture(GX_CAPTURE_SIZE_256x192, GX_CAPTURE_MODE_A, GX_CAPTURE_SRCA_3D, (GXCaptureSrcB)0, GX_CAPTURE_DEST_VRAM_D_0x00000, 16, 0);
		mCurFrameType = FRAME_TYPE_MAIN_FAR;
	}
	else if (mCurFrameType == FRAME_TYPE_MAIN_FAR)//mRenderState == 0)
	{
		GX_SetCapture(GX_CAPTURE_SIZE_256x192, GX_CAPTURE_MODE_A, /*GX_CAPTURE_SRCA_3D*/GX_CAPTURE_SRCA_2D3D, (GXCaptureSrcB)0, GX_CAPTURE_DEST_VRAM_D_0x00000, 16, 0);
		mCurFrameType = FRAME_TYPE_MAIN_NEAR;
	}
	else if (mCurFrameType == FRAME_TYPE_MAIN_NEAR)
	{
		GX_SetCapture(GX_CAPTURE_SIZE_256x192, GX_CAPTURE_MODE_A, GX_CAPTURE_SRCA_2D3D, (GXCaptureSrcB)0, GX_CAPTURE_DEST_VRAM_B_0x00000, 16, 0);
		mCurFrameType = FRAME_TYPE_MAIN_FAR;
	}
	else if (mCurFrameType == FRAME_TYPE_SUB)
	{
		GX_SetCapture(GX_CAPTURE_SIZE_128x128, GX_CAPTURE_MODE_A, GX_CAPTURE_SRCA_3D, (GXCaptureSrcB)0, GX_CAPTURE_DEST_VRAM_D_0x00000, 16, 0);
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