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
#include "TitleMenu2.h"

#define SWAP_BUFFERS_SORTMODE	GX_SORTMODE_MANUAL //AUTO
#define SWAP_BUFFERS_BUFFERMODE	GX_BUFFERMODE_Z

void TitleMenu2::Initialize(int arg)
{
	//load overlay
	LOAD_OVERLAY_ITCM(rendering_itcm);

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

	//G3X_SetEdgeColorTable(&sEdgeMarkingColorTable[0]);

	G3X_SetClearColor(GX_RGB(119 >> 3, 199 >> 3, 244 >> 3), 31, 0x7fff, 0, false);
	G3_ViewPort(0, 0, 255, 191);

	GX_SetDispSelect(GX_DISP_SELECT_MAIN_SUB);

	/*G2_SetWnd0InsidePlane(GX_WND_PLANEMASK_BG0 | GX_WND_PLANEMASK_BG1 | GX_WND_PLANEMASK_BG2 | GX_WND_PLANEMASK_BG3 | GX_WND_PLANEMASK_OBJ, TRUE);
	G2_SetWnd1InsidePlane(GX_WND_PLANEMASK_BG0 | GX_WND_PLANEMASK_BG1 | GX_WND_PLANEMASK_BG2 | GX_WND_PLANEMASK_BG3 | GX_WND_PLANEMASK_OBJ, TRUE);
	G2_SetWndOutsidePlane(GX_WND_PLANEMASK_BG0 | GX_WND_PLANEMASK_BG1 | GX_WND_PLANEMASK_BG2 | GX_WND_PLANEMASK_BG3 | GX_WND_PLANEMASK_OBJ, FALSE);
	G2_SetWnd0Position(0, 191 - 20, 255, 192);
	G2_SetWnd1Position(1, 191 - 20, 0, 192);
	G2_SetBlendBrightnessExt(GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_BG3 | GX_BLEND_PLANEMASK_BD, GX_BLEND_PLANEMASK_BG3 | GX_BLEND_PLANEMASK_BD, 0, 0, -6);
	GX_SetVisibleWnd(GX_WNDMASK_W0 | GX_WNDMASK_W1);*/
	G2_SetBlendAlpha(GX_BLEND_PLANEMASK_BG0, GX_BLEND_PLANEMASK_BG3 | GX_BLEND_PLANEMASK_BD, 16, 0);


	NNS_GfdResetLnkTexVramState();
	NNS_GfdResetLnkPlttVramState();

	//NNS_G2dInitOamManagerModule();
	//NNS_G2dGetNewOamManagerInstanceAsFastTransferMode(&mSubObjOamManager, 0, 128, NNS_G2D_OAMTYPE_SUB);

	GX_SetOBJVRamModeChar(GX_OBJVRAMMODE_CHAR_1D_32K);

	mFontData = Util_LoadFileToBuffer("/data/fonts/fot_rodin_bokutoh_pro_db_9pt.NFTR", NULL, false);
	MI_CpuClear8(&mFont, sizeof(mFont));
	NNS_G2dFontInitAuto(&mFont, mFontData);

	NNS_G2dCharCanvasInitForOBJ1D(&mCanvas, (uint8_t*)G2_GetOBJCharPtr(), 32, 2, NNS_G2D_CHARA_COLORMODE_16);
	NNS_G2dTextCanvasInit(&mTextCanvas, &mCanvas, &mFont, 0, 1);
	NNS_G2dCharCanvasClear(&mCanvas, 0);
	NNS_G2dTextCanvasDrawTextRect(
		&mTextCanvas, 0, 0, 256, 16, 1, NNS_G2D_VERTICALORIGIN_TOP | NNS_G2D_HORIZONTALORIGIN_LEFT | NNS_G2D_HORIZONTALALIGN_CENTER | NNS_G2D_VERTICALALIGN_MIDDLE, (NNSG2dChar*)L"© 2016-2017 Expand Productions");

	for (int i = 0; i < 16; i++)
	{
		int rnew = 4 + ((28 - 4) * i) / 15;
		int gnew = 4 + ((28 - 4) * i) / 15;
		int bnew = 4 + ((28 - 4) * i) / 15;
		((uint16_t*)HW_OBJ_PLTT)[i] = GX_RGB(rnew, gnew, bnew);
	}

	NNS_G2dArrangeOBJ1D((GXOamAttr*)HW_OAM, 32, 2, 0, 168 + 6, GX_OAM_COLORMODE_16, 0, NNS_G2D_OBJVRAMMODE_32K);

	Util_LoadTextureFromCard("/data/menu/title/titlelogolarge.ntft", "/data/menu/title/titlelogolarge.ntfp", mLogoLargeTexture.texKey, mLogoLargeTexture.plttKey);

	mGameController = new GameController();

	mGameController->mWagon->PutOnTrack(mGameController->mMap->GetFirstTrackPiece(), 138 * FX32_ONE/*146 * FX32_ONE + (FX32_HALF >> 1)*/);
	mGameController->mWagon->mDriving = true;

	mTSPlayer = new TitleSequencePlayer(mGameController, gTitleSequence);

	mVRAMCopyVAlarm = new OS::VAlarm();
	mVRAMCopyVAlarm->SetPeriodic(192 - 48, 5, TitleMenu2::OnVRAMCopyVAlarm, this);

	NNS_SndStrmHandleInit(&mMusicHandle);
	NNS_SndArcStrmStart(&mMusicHandle, STRM_TITLE, 0);
}

void TitleMenu2::OnVRAMCopyVAlarm()
{
	if (mRenderMode == GameController::RENDER_MODE_NEAR)
		mGameController->mMap->mTerrainTextureManager16->UpdateVramC();
	else
		mGameController->mMap->mTerrainTextureManager8->UpdateVramC();
}

void TitleMenu2::Render()
{
	Core_ReadInput();
	if (gKeys & PAD_BUTTON_START)
		Game::GotoMenu();
	G3X_Reset();
	mTSPlayer->Update();
	mGameController->Update();
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
		G3_TexImageParam(GX_TEXFMT_A3I5, GX_TEXGEN_TEXCOORD, GX_TEXSIZE_S256, GX_TEXSIZE_T64, GX_TEXREPEAT_NONE, GX_TEXFLIP_NONE, GX_TEXPLTTCOLOR0_USE, NNS_GfdGetTexKeyAddr(mLogoLargeTexture.texKey));
		G3_TexPlttBase(NNS_GfdGetPlttKeyAddr(mLogoLargeTexture.plttKey), GX_TEXFMT_A3I5);

		switch (mState)
		{
		case TITLE_MENU_STATE_LOGO_IN:
			Util_DrawSprite(27 * FX32_ONE, -64 * FX32_ONE + (8 * FX32_ONE - -64 * FX32_ONE) * mStateCounter / 29, 1024 * FX32_ONE, 202 * FX32_ONE, 64 * FX32_ONE);
			if (mStateCounter == 29)
			{
				mStateCounter = 0;
				mState = TITLE_MENU_STATE_LOGO_WAIT;
			}
			else
				mStateCounter++;
			break;
		case TITLE_MENU_STATE_LOGO_WAIT:
			Util_DrawSprite(27 * FX32_ONE, 8 * FX32_ONE, 1024 * FX32_ONE, 202 * FX32_ONE, 64 * FX32_ONE);
			if (mStateCounter == 79)
			{
				mStateCounter = 0;
				mState = TITLE_MENU_STATE_LOGO_SCALE;
			}
			else
				mStateCounter++;
			break;
		case TITLE_MENU_STATE_LOGO_SCALE:
			fx32 newX = 27 * FX32_ONE + (86 * FX32_ONE - 27 * FX32_ONE) * mStateCounter / 19;
			fx32 newY = 8 * FX32_ONE + (-10 * FX32_ONE - 8 * FX32_ONE) * mStateCounter / 19;
			fx32 newScale = (202 * FX32_ONE + (138 * FX32_ONE - 202 * FX32_ONE) * mStateCounter / 19) / 202;
			Util_DrawSpriteScaled(newX, newY, 1024 * FX32_ONE, 202 * FX32_ONE, 64 * FX32_ONE, newScale);
			if (mStateCounter == 19)
			{
				mStateCounter = 0;
				mState = TITLE_MENU_STATE_LOOP;
			}
			else
				mStateCounter++;
			break;
		case TITLE_MENU_STATE_LOOP:
			Util_DrawSpriteScaled(86 * FX32_ONE, -10 * FX32_ONE, 1024 * FX32_ONE, 202 * FX32_ONE, 64 * FX32_ONE, 138 * FX32_ONE / 202);
			break;
		}

		G3_PolygonAttr(0, GX_POLYGONMODE_MODULATE, GX_CULL_BACK, 0x30, 12, 0);
		G3_TexImageParam(GX_TEXFMT_NONE, GX_TEXGEN_NONE, GX_TEXSIZE_S256, GX_TEXSIZE_T64, GX_TEXREPEAT_NONE, GX_TEXFLIP_NONE, GX_TEXPLTTCOLOR0_USE, 0);
		G3_Color(GX_RGB(0, 0, 0));
		Util_DrawSprite(0, (192 - 21) * FX32_ONE, 1024 * FX32_ONE, 256 * FX32_ONE, 21 * FX32_ONE);
	}
	G3_SwapBuffers(SWAP_BUFFERS_SORTMODE, SWAP_BUFFERS_BUFFERMODE);
}

void TitleMenu2::VBlank()
{
	if (mRenderMode == GameController::RENDER_MODE_FAR)//mRenderState == 0)
	{
		GX_SetBankForLCDC(GX_GetBankForLCDC() | GX_VRAM_LCDC_B | GX_VRAM_LCDC_D);
		GX_SetGraphicsMode(GX_DISPMODE_VRAM_D, GX_BGMODE_0, GX_BG0_AS_3D);
		GX_SetVisiblePlane(GX_PLANEMASK_BG0);
		GX_SetCapture(GX_CAPTURE_SIZE_256x192, GX_CAPTURE_MODE_A, GX_CAPTURE_SRCA_3D, (GXCaptureSrcB)0, GX_CAPTURE_DEST_VRAM_B_0x00000, 16, 0);
		mRenderMode = GameController::RENDER_MODE_NEAR;
	}
	else if (mRenderMode == GameController::RENDER_MODE_NEAR)
	{
		GX_SetBankForLCDC(GX_GetBankForLCDC() | GX_VRAM_LCDC_D);
		GX_SetBankForBG(GX_VRAM_BG_128_B);
		//set this to GX_DISPMODE_VRAM_D to prevent flickering, but it will introduce one frame of lag (don't know if that really matters though)
		GX_SetGraphicsMode(GX_DISPMODE_GRAPHICS, GX_BGMODE_5, GX_BG0_AS_3D);
		GX_SetVisiblePlane(GX_PLANEMASK_BG0 | GX_PLANEMASK_BG3 | GX_PLANEMASK_OBJ);
		G2_SetBG3ControlDCBmp(GX_BG_SCRSIZE_DCBMP_256x256, GX_BG_AREAOVER_XLU, GX_BG_BMPSCRBASE_0x00000);
		G2_SetBG3Priority(3);
		G2_SetBG0Priority(0);
		GX_SetCapture(GX_CAPTURE_SIZE_256x192, GX_CAPTURE_MODE_A, GX_CAPTURE_SRCA_2D3D, (GXCaptureSrcB)0, GX_CAPTURE_DEST_VRAM_D_0x00000, 16, 0);
		mRenderMode = GameController::RENDER_MODE_FAR;
	}
}

void TitleMenu2::Finalize()
{
	NNS_SndArcStrmStop(&mMusicHandle, 0);
	delete mVRAMCopyVAlarm;
	delete mGameController;
}