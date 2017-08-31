#include <nitro.h>
#include <nnsys/g2d.h>
#include <nnsys/g3d.h>
#include <nnsys/gfd.h>
#include "core.h"
#include "util.h"
#include "Menu.h"
#include "terrain/managers/TerrainTextureManager8.h"
#include "terrain/managers/TerrainTextureManager16.h"
#include "terrain/track/FlexTrack.h"
#include "engine/TitleSequence.h"
#include "engine/TitleSequencePlayer.h"
#include "Game.h"
#include "MultibootMenu.h"
#include "ui/layoutengine/Layout.h"
#include "ui/layoutengine/FontManager.h"
#include "Loader.h"
#include "TitleMenu.h"
#include "ui/layoutengine/PicturePane.h"

#define SWAP_BUFFERS_SORTMODE	GX_SORTMODE_MANUAL //AUTO
#define SWAP_BUFFERS_BUFFERMODE	GX_BUFFERMODE_Z

BOOL TitleMenu::CallBackAddOam(const GXOamAttr* pOam, u16 affineIndex, BOOL bDoubleAffine)
{
#pragma unused( bDoubleAffine )
	return NNS_G2dEntryOamManagerOamWithAffineIdx(&((TitleMenu*)gRunningMenu)->mSubObjOamManager, pOam, affineIndex);
}

u16 TitleMenu::CallBackAddAffine(const MtxFx22* mtx)
{
	return NNS_G2dEntryOamManagerAffine(&((TitleMenu*)gRunningMenu)->mSubObjOamManager, mtx);
}

void TitleMenu::Initialize(int arg)
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
		mGameController = new GameController(NULL);

		mGameController->mTrain->PutOnTrack(mGameController->mMap->GetFirstTrackPiece(), 1, /*118*/250 * FX32_ONE/*146 * FX32_ONE + (FX32_HALF >> 1)*/);
		mGameController->mTrain->SetDriving(true);

		mTSPlayer = new TitleSequencePlayer(mGameController, gTitleSequence);
	}
	loader->FinishLoad();
	loader->WaitFinish();
	delete loader;

	GX_SetBankForLCDC(GX_VRAM_LCDC_D);

	GX_SetBankForOBJ(GX_VRAM_OBJ_16_F);

	GX_SetBankForSubBG(GX_VRAM_SUB_BG_32_H);
	GX_SetBankForSubOBJ(GX_VRAM_SUB_OBJ_16_I);

	GX_SetGraphicsMode(GX_DISPMODE_GRAPHICS, GX_BGMODE_3, GX_BG0_AS_3D);
	GX_SetVisiblePlane(GX_PLANEMASK_BG0 | GX_PLANEMASK_OBJ);
	GXS_SetGraphicsMode(GX_BGMODE_0);
	GXS_SetVisiblePlane(GX_PLANEMASK_OBJ | GX_PLANEMASK_BG1);// | GX_PLANEMASK_BG3 | GX_PLANEMASK_OBJ);
	G2_SetBG0Priority(3);
	G2_SetBG3Priority(3);

	G3X_SetShading(GX_SHADING_HIGHLIGHT);
	G3X_AntiAlias(true);
	G3_SwapBuffers(SWAP_BUFFERS_SORTMODE, SWAP_BUFFERS_BUFFERMODE);

	G3X_AlphaTest(false, 0);                   // AlphaTest OFF
	G3X_AlphaBlend(true);                      // AlphaTest ON
	G3X_EdgeMarking(true);

	//G3X_SetEdgeColorTable(&sEdgeMarkingColorTable[0]);

	G3X_SetClearColor(GX_RGB(119 >> 3, 199 >> 3, 244 >> 3), 31, 0x7fff, 0, false);
	G3_ViewPort(0, 0, 255, 191);

	GX_SetDispSelect(GX_DISP_SELECT_MAIN_SUB);

	G2_SetBlendAlpha(GX_BLEND_PLANEMASK_BG0, GX_BLEND_PLANEMASK_BG3 | GX_BLEND_PLANEMASK_BD, 16, 0);

	GX_SetOBJVRamModeChar(GX_OBJVRAMMODE_CHAR_1D_32K);

	NNS_GfdInitVramTransferManager(mVramTransferTaskArray, TITLE_MENU_VRAM_TRANSFER_MANAGER_NR_TASKS);

	mFontManager = new FontManager();

	mFontData = Util_LoadLZ77FileToBuffer("/data/fonts/fot_rodin_bokutoh_pro_db_9pt.NFTR.lz", NULL, false);
	MI_CpuClear8(&mFont, sizeof(mFont));
	NNS_G2dFontInitAuto(&mFont, mFontData);

	mFontManager->RegisterFont("rodin_db_9", &mFont);

	mFontData2 = Util_LoadLZ77FileToBuffer("/data/fonts/fot_rodin_bokutoh_pro_b_13pt.NFTR.lz", NULL, false);
	MI_CpuClear8(&mFont2, sizeof(mFont2));
	NNS_G2dFontInitAuto(&mFont2, mFontData2);

	mFontManager->RegisterFont("rodin_b_13", &mFont2);

	NNS_G2dCharCanvasInitForOBJ1D(&mCanvas, (uint8_t*)G2_GetOBJCharPtr(), 32, 2, NNS_G2D_CHARA_COLORMODE_16);
	NNS_G2dTextCanvasInit(&mTextCanvas, &mCanvas, &mFont, 0, 1);
	NNS_G2dCharCanvasClear(&mCanvas, 0);
	NNS_G2dTextCanvasDrawTextRect(
		&mTextCanvas, 0, 0, 256, 16, 1, NNS_G2D_VERTICALORIGIN_TOP | NNS_G2D_HORIZONTALORIGIN_LEFT | NNS_G2D_HORIZONTALALIGN_CENTER | NNS_G2D_VERTICALALIGN_MIDDLE,
		(NNSG2dChar*)L"© 2016-2017 Expand Productions");

	for (int i = 0; i < 16; i++)
	{
		int rnew = 4 + ((28 - 4) * i) / 15;
		int gnew = 4 + ((28 - 4) * i) / 15;
		int bnew = 4 + ((28 - 4) * i) / 15;
		((uint16_t*)HW_OBJ_PLTT)[i] = GX_RGB(rnew, gnew, bnew);
	}

	NNS_G2dArrangeOBJ1D((GXOamAttr*)HW_OAM, 32, 2, 0, 168 + 6, GX_OAM_COLORMODE_16, 0, NNS_G2D_OBJVRAMMODE_32K);

	NNS_G2dInitOamManagerModule();
	NNS_G2dGetNewOamManagerInstanceAsFastTransferMode(&mSubObjOamManager, 0, 128, NNS_G2D_OAMTYPE_SUB);

	void* cData = Util_LoadFileToBuffer("/data/menu/title/titlelayout.ncer", NULL, FALSE);
	NNS_G2dGetUnpackedCellBank(cData, &mLayoutCellDataBank);

	NNSG2dCharacterData* charDataUnpacked;
	NNSG2dPaletteData* palDataUnpacked;

	void* mCharDataSub = Util_LoadFileToBuffer("/data/menu/title/titlelayout.ncgr", NULL, TRUE);
	NNS_G2dGetUnpackedCharacterData(mCharDataSub, &charDataUnpacked);
	u32 objDataLength = charDataUnpacked->szByte;
	NNS_G2dInitImageProxy(&mImageProxy);
	NNS_G2dLoadImage1DMapping(charDataUnpacked, 0, NNS_G2D_VRAM_TYPE_2DSUB, &mImageProxy);
	NNS_FndFreeToExpHeap(gHeapHandle, mCharDataSub);

	void* mPalDataSub = Util_LoadFileToBuffer("/data/menu/title/titlelayout.nclr", NULL, TRUE);
	NNS_G2dInitImagePaletteProxy(&mImagePaletteProxy);
	NNS_G2dGetUnpackedPaletteData(mPalDataSub, &palDataUnpacked);
	NNS_G2dLoadPalette(palDataUnpacked, 0, NNS_G2D_VRAM_TYPE_2DSUB, &mImagePaletteProxy);
	NNS_FndFreeToExpHeap(gHeapHandle, mPalDataSub);

	lyt_res_t* lytData = (lyt_res_t*)Util_LoadFileToBuffer("/data/menu/title/titlelayout.bnlyt", NULL, TRUE);
	u32 objDataOffset = objDataLength;
	mLayoutTest = new Layout(lytData, Layout::LAYOUT_SCREEN_SUB, mLayoutCellDataBank, mFontManager, objDataOffset);
	NNS_FndFreeToExpHeap(gHeapHandle, lytData);

	PicturePane* btn_missions = (PicturePane*)mLayoutTest->FindPaneByName("btn_missions");
	btn_missions->SetCell(NNS_G2dGetCellDataByIdx(mLayoutCellDataBank, 2));
	mButtons[0][0] = btn_missions;
	mButtons[1][0] = btn_missions;
	PicturePane* btn_sandbox = (PicturePane*)mLayoutTest->FindPaneByName("btn_sandbox");
	mButtons[0][1] = btn_sandbox;
	mButtons[1][1] = btn_sandbox;
	PicturePane* btn_depot = (PicturePane*)mLayoutTest->FindPaneByName("btn_depot");
	mButtons[0][2] = btn_depot;
	mButtons[1][2] = btn_depot;
	mButtons[0][3] = (PicturePane*)mLayoutTest->FindPaneByName("btn_clone");
	mButtons[1][3] = (PicturePane*)mLayoutTest->FindPaneByName("btn_options");

	NNS_G2dInitRenderer(&mOAMRender);
	NNS_G2dInitRenderSurface(&mOAMRenderSurface);

	mOAMRenderSurface.viewRect.posTopLeft.x = 0;
	mOAMRenderSurface.viewRect.posTopLeft.y = 0;
	mOAMRenderSurface.viewRect.sizeView.x = 256 * FX32_ONE;
	mOAMRenderSurface.viewRect.sizeView.y = 192 * FX32_ONE;
	mOAMRenderSurface.type = NNS_G2D_SURFACETYPE_SUB2D;

	mOAMRenderSurface.pFuncOamRegister = CallBackAddOam;
	mOAMRenderSurface.pFuncOamAffineRegister = CallBackAddAffine;

	NNS_G2dAddRendererTargetSurface(&mOAMRender, &mOAMRenderSurface);
	NNS_G2dSetRendererImageProxy(&mOAMRender, &mImageProxy, &mImagePaletteProxy);

	Util_LoadTextureFromCard("/data/menu/title/titlelogolarge_new_reduced_a.ntft", "/data/menu/title/titlelogolarge_new_reduced.ntfp", mLogoLargeTextureA.texKey, mLogoLargeTextureA.plttKey);
	Util_LoadTextureFromCard("/data/menu/title/titlelogolarge_new_reduced_b.ntft", NULL, mLogoLargeTextureB.texKey, mLogoLargeTextureB.plttKey);
	mLogoLargeTextureB.plttKey = mLogoLargeTextureA.plttKey;

	mVRAMCopyVAlarm = new OS::VAlarm();
	mVRAMCopyVAlarm->SetPeriodic(192 - 48, 5, TitleMenu::OnVRAMCopyVAlarm, this);

	if (!MB_IsMultiBootChild())
	{
		NNS_SndStrmHandleInit(&mMusicHandle);
		NNS_SndArcStrmStart(&mMusicHandle, STRM_TITLE, 0);
	}

	OS_SetIrqFunction(OS_IE_V_BLANK, VBlankIrqHandler);
}

void TitleMenu::OnVRAMCopyVAlarm()
{
	if (mRenderMode == GameController::RENDER_MODE_NEAR)
		mGameController->mMap->mTerrainTextureManager16->UpdateVramC();
	else
		mGameController->mMap->mTerrainTextureManager8->UpdateVramC();
}

#define KEY_HOLD_DELAY 4
#define KEY_REPEAT_FREQ 6

void TitleMenu::Render()
{
	Core_ReadInput();
	if (mSelectedButton == 1 && (gKeys & PAD_BUTTON_A))
		Game::GotoMenu();
	else if (mSelectedButton == 3 && mLeftRight == 0 && (gKeys & PAD_BUTTON_A))
		MultibootMenu::GotoMenu();

	u16 heldKeys = 0;
	if (gKeys != gOldKeys)
	{
		mKeyCounter = KEY_REPEAT_FREQ;
		mKeyDelay = KEY_HOLD_DELAY;
		heldKeys = gKeys & ~gOldKeys;
	}
	if (--mKeyCounter == 0)
	{
		mKeyCounter = KEY_REPEAT_FREQ;
		if (mKeyDelay > 0)
			mKeyDelay--;
		else
			heldKeys = gKeys;
	}

	if (heldKeys & (PAD_KEY_UP | PAD_KEY_DOWN))
	{
		if (mSelectedButton != 3)
			mButtons[mLeftRight][mSelectedButton]->SetCell(NNS_G2dGetCellDataByIdx(mLayoutCellDataBank, 0));
		else
			mButtons[mLeftRight][mSelectedButton]->SetCell(NNS_G2dGetCellDataByIdx(mLayoutCellDataBank, 1));
		if (heldKeys & PAD_KEY_UP)
		{			
			if (mSelectedButton == 0)
				mSelectedButton = 3;
			else
				mSelectedButton--;
		}
		else if (heldKeys & PAD_KEY_DOWN)
		{
			if (mSelectedButton == 3)
				mSelectedButton = 0;
			else
				mSelectedButton++;
		}
		if (mSelectedButton != 3)
			mButtons[mLeftRight][mSelectedButton]->SetCell(NNS_G2dGetCellDataByIdx(mLayoutCellDataBank, 2));
		else
			mButtons[mLeftRight][mSelectedButton]->SetCell(NNS_G2dGetCellDataByIdx(mLayoutCellDataBank, 3));
	}
	if (mSelectedButton == 3 && (heldKeys & (PAD_KEY_LEFT | PAD_KEY_RIGHT)))
	{
		mButtons[mLeftRight][mSelectedButton]->SetCell(NNS_G2dGetCellDataByIdx(mLayoutCellDataBank, 1));
		mLeftRight = !mLeftRight;
		mButtons[mLeftRight][mSelectedButton]->SetCell(NNS_G2dGetCellDataByIdx(mLayoutCellDataBank, 3));
	}
	G3X_Reset();
	if (mRenderMode == GameController::RENDER_MODE_FAR)
	{
		OS_Printf("%d fps\n", 60 / mPassedFrameCounter);
		mTSPlayer->Update(mPassedFrameCounter);
		mGameController->Update(mPassedFrameCounter);
		mPassedFrameCounter = 0;
	}
	NNS_G3dGlbSetViewPort(0, 0, 255, 191);
	mGameController->Render(mRenderMode);
	if (mRenderMode == GameController::RENDER_MODE_NEAR)
	{
		//Do some 2d with the 3d engine when needed (AKA, fucking up matrices)
		G3_MtxMode(GX_MTXMODE_PROJECTION);
		{
			G3_Identity();
			G3_OrthoW(FX32_ONE * 0, FX32_ONE * 192, FX32_ONE * 0, FX32_ONE * 256, FX32_ONE * -1024, FX32_ONE * 1024, FX32_ONE * 1024, NULL);
		}
		G3_MtxMode(GX_MTXMODE_TEXTURE);
		{
			G3_Identity();
		}
		G3_MtxMode(GX_MTXMODE_POSITION_VECTOR);
		G3_Identity();
		G3_Color(GX_RGB(31, 31, 31));

		G3_PolygonAttr(0, GX_POLYGONMODE_MODULATE, GX_CULL_BACK, 0x30, 31, 0);
		
		G3_TexPlttBase(NNS_GfdGetPlttKeyAddr(mLogoLargeTextureA.plttKey), GX_TEXFMT_A3I5);

		switch (mState)
		{
		case TITLE_MENU_STATE_LOGO_IN:
			G3_TexImageParam(GX_TEXFMT_A3I5, GX_TEXGEN_TEXCOORD, GX_TEXSIZE_S128, GX_TEXSIZE_T128, GX_TEXREPEAT_NONE, GX_TEXFLIP_NONE, GX_TEXPLTTCOLOR0_USE, NNS_GfdGetTexKeyAddr(mLogoLargeTextureA.texKey));
			Util_DrawSprite(32 * FX32_ONE, -83 * FX32_ONE + (16 * FX32_ONE - -83 * FX32_ONE) * mStateCounter / 29, 1024 * FX32_ONE, 128 * FX32_ONE, 83 * FX32_ONE);
			G3_TexImageParam(GX_TEXFMT_A3I5, GX_TEXGEN_TEXCOORD, GX_TEXSIZE_S64, GX_TEXSIZE_T128, GX_TEXREPEAT_NONE, GX_TEXFLIP_NONE, GX_TEXPLTTCOLOR0_USE, NNS_GfdGetTexKeyAddr(mLogoLargeTextureB.texKey));
			Util_DrawSprite((32 + 128) * FX32_ONE, -83 * FX32_ONE + (16 * FX32_ONE - -83 * FX32_ONE) * mStateCounter / 29, 1024 * FX32_ONE, 64 * FX32_ONE, 83 * FX32_ONE);
			if (mStateCounter == 29)
			{
				mStateCounter = 0;
				mState = TITLE_MENU_STATE_LOGO_WAIT;
			}
			else
				mStateCounter++;
			break;
		case TITLE_MENU_STATE_LOGO_WAIT:
			G3_TexImageParam(GX_TEXFMT_A3I5, GX_TEXGEN_TEXCOORD, GX_TEXSIZE_S128, GX_TEXSIZE_T128, GX_TEXREPEAT_NONE, GX_TEXFLIP_NONE, GX_TEXPLTTCOLOR0_USE, NNS_GfdGetTexKeyAddr(mLogoLargeTextureA.texKey));
			Util_DrawSprite(32 * FX32_ONE, 16 * FX32_ONE, 1024 * FX32_ONE, 128 * FX32_ONE, 83 * FX32_ONE);
			G3_TexImageParam(GX_TEXFMT_A3I5, GX_TEXGEN_TEXCOORD, GX_TEXSIZE_S64, GX_TEXSIZE_T128, GX_TEXREPEAT_NONE, GX_TEXFLIP_NONE, GX_TEXPLTTCOLOR0_USE, NNS_GfdGetTexKeyAddr(mLogoLargeTextureB.texKey));
			Util_DrawSprite((32 + 128) * FX32_ONE, 16 * FX32_ONE, 1024 * FX32_ONE, 64 * FX32_ONE, 83 * FX32_ONE);
			if (mStateCounter == 79)
			{
				mStateCounter = 0;
				mState = TITLE_MENU_STATE_LOGO_SCALE;
			}
			else
				mStateCounter++;
			break;
		case TITLE_MENU_STATE_LOGO_SCALE:
			G3_TexImageParam(GX_TEXFMT_A3I5, GX_TEXGEN_TEXCOORD, GX_TEXSIZE_S128, GX_TEXSIZE_T128, GX_TEXREPEAT_NONE, GX_TEXFLIP_NONE, GX_TEXPLTTCOLOR0_USE, NNS_GfdGetTexKeyAddr(mLogoLargeTextureA.texKey));
			fx32 newX = 32 * FX32_ONE + (105 * FX32_ONE - 32 * FX32_ONE) * mStateCounter / 19;
			fx32 newY = 16 * FX32_ONE + (-11 * FX32_ONE - 16 * FX32_ONE) * mStateCounter / 19;
			fx32 newScale = (192 * FX32_ONE + (122 * FX32_ONE - 192 * FX32_ONE) * mStateCounter / 19) / 192;
			Util_DrawSpriteScaled(newX, newY, 1024 * FX32_ONE, 128 * FX32_ONE, 83 * FX32_ONE, newScale);
			G3_TexImageParam(GX_TEXFMT_A3I5, GX_TEXGEN_TEXCOORD, GX_TEXSIZE_S64, GX_TEXSIZE_T128, GX_TEXREPEAT_NONE, GX_TEXFLIP_NONE, GX_TEXPLTTCOLOR0_USE, NNS_GfdGetTexKeyAddr(mLogoLargeTextureB.texKey));
			Util_DrawSpriteScaled(newX + 128 * newScale + ((64 * FX32_ONE - (64 * newScale)) >> 1), newY, 1024 * FX32_ONE, 64 * FX32_ONE, 83 * FX32_ONE, newScale);

			if (mStateCounter == 19)
			{
				mStateCounter = 0;
				mState = TITLE_MENU_STATE_LOOP;
			}
			else
				mStateCounter++;
			break;
		case TITLE_MENU_STATE_LOOP:
			G3_TexImageParam(GX_TEXFMT_A3I5, GX_TEXGEN_TEXCOORD, GX_TEXSIZE_S128, GX_TEXSIZE_T128, GX_TEXREPEAT_NONE, GX_TEXFLIP_NONE, GX_TEXPLTTCOLOR0_USE, NNS_GfdGetTexKeyAddr(mLogoLargeTextureA.texKey));
			Util_DrawSpriteScaled(105 * FX32_ONE, -11 * FX32_ONE, 1024 * FX32_ONE, 128 * FX32_ONE, 83 * FX32_ONE, 122 * FX32_ONE / 192);
			G3_TexImageParam(GX_TEXFMT_A3I5, GX_TEXGEN_TEXCOORD, GX_TEXSIZE_S64, GX_TEXSIZE_T128, GX_TEXREPEAT_NONE, GX_TEXFLIP_NONE, GX_TEXPLTTCOLOR0_USE, NNS_GfdGetTexKeyAddr(mLogoLargeTextureB.texKey));
			Util_DrawSpriteScaled(105 * FX32_ONE + 93 * FX32_ONE, -11 * FX32_ONE, 1024 * FX32_ONE, 64 * FX32_ONE, 83 * FX32_ONE, 122 * FX32_ONE / 192);
			break;
		}

		G3_PolygonAttr(0, GX_POLYGONMODE_MODULATE, GX_CULL_BACK, 0x30, 12, 0);
		G3_TexImageParam(GX_TEXFMT_NONE, GX_TEXGEN_NONE, GX_TEXSIZE_S256, GX_TEXSIZE_T64, GX_TEXREPEAT_NONE, GX_TEXFLIP_NONE, GX_TEXPLTTCOLOR0_USE, 0);
		G3_Color(GX_RGB(0, 0, 0));
		Util_DrawSprite(0, (192 - 21) * FX32_ONE, 1024 * FX32_ONE, 256 * FX32_ONE, 21 * FX32_ONE);
	}
	SetSwapBuffersFlag();
	NNS_G2dBeginRendering(&mOAMRender);
	{
		mLayoutTest->Render();
	}
	NNS_G2dEndRendering();
}

void TitleMenu::SetSwapBuffersFlag()
{
	OSIntrMode old = OS_DisableInterrupts();
	G3_SwapBuffers(SWAP_BUFFERS_SORTMODE, SWAP_BUFFERS_BUFFERMODE);
	mSwap = true;
	OS_RestoreInterrupts(old);
}

//on every vblank
void TitleMenu::VBlankIrq()
{
	OS_SetIrqCheckFlag(OS_IE_V_BLANK);
	mPassedFrameCounter++;
	if (mRenderMode == GameController::RENDER_MODE_FAR)//mRenderState == 0)
	{
		GX_SetBankForLCDC(GX_GetBankForLCDC() | GX_VRAM_LCDC_B | GX_VRAM_LCDC_D);
		mGameController->mDisplayFlare = true;//(((GXRgb*)HW_LCDC_VRAM_B)[mGameController->mSunY * 256 + mGameController->mSunX] & 0x7FFF) == mGameController->mSunColorMatch;
		GX_SetGraphicsMode(GX_DISPMODE_VRAM_B, GX_BGMODE_0, GX_BG0_AS_3D);
		GX_SetVisiblePlane(GX_PLANEMASK_BG0);
	}
	else if (mRenderMode == GameController::RENDER_MODE_NEAR)
	{
		GX_SetBankForLCDC(GX_GetBankForLCDC() | GX_VRAM_LCDC_B);
		GX_SetBankForTex(GX_VRAM_TEX_012_ACD);
		//GX_SetBankForBG(GX_VRAM_BG_128_D);
		//set this to GX_DISPMODE_VRAM_D to prevent flickering, but it will introduce one frame of lag (don't know if that really matters though)
		if (!G3X_IsGeometryBusy() && mSwap)
			GX_SetGraphicsMode(GX_DISPMODE_GRAPHICS, GX_BGMODE_5, GX_BG0_AS_3D);
		else
			GX_SetGraphicsMode(GX_DISPMODE_VRAM_B, GX_BGMODE_5, GX_BG0_AS_3D);
		GX_SetVisiblePlane(GX_PLANEMASK_BG0 /*| GX_PLANEMASK_BG3*/ | GX_PLANEMASK_OBJ);
		//G2_SetBG3ControlDCBmp(GX_BG_SCRSIZE_DCBMP_256x256, GX_BG_AREAOVER_XLU, GX_BG_BMPSCRBASE_0x00000);
		//G2_SetBG3Priority(3);
		G2_SetBG0Priority(0);
	}
	NNS_GfdDoVramTransfer();
	mSwap = false;
}

void TitleMenu::VBlank()
{
	NNS_G2dApplyOamManagerToHW(&mSubObjOamManager);
	NNS_G2dResetOamManagerBuffer(&mSubObjOamManager);
	if (mRenderMode == GameController::RENDER_MODE_FAR)//mRenderState == 0)
	{
		GX_SetCapture(GX_CAPTURE_SIZE_256x192, GX_CAPTURE_MODE_A, GX_CAPTURE_SRCA_3D, (GXCaptureSrcB)0, GX_CAPTURE_DEST_VRAM_D_0x00000, 16, 0);
		mRenderMode = GameController::RENDER_MODE_NEAR;
	}
	else if (mRenderMode == GameController::RENDER_MODE_NEAR)
	{
		GX_SetCapture(GX_CAPTURE_SIZE_256x192, GX_CAPTURE_MODE_A, GX_CAPTURE_SRCA_2D3D, (GXCaptureSrcB)0, GX_CAPTURE_DEST_VRAM_B_0x00000, 16, 0);
		mRenderMode = GameController::RENDER_MODE_FAR;
	}
}

void TitleMenu::Finalize()
{
	if (!MB_IsMultiBootChild())
		NNS_SndArcStrmStop(&mMusicHandle, 0);
	delete mVRAMCopyVAlarm;
	delete mGameController;
}