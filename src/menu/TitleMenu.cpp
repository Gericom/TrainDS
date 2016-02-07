#include <nitro.h>
#include <nnsys/g2d.h>
#include <nnsys/gfd.h>
#include "core.h"
#include "util.h"
#include "Menu.h"
#include "TitleMenu.h"

#define STACK_SIZE     1024
#define THREAD1_PRIO   (OS_THREAD_LAUNCHER_PRIORITY - 6)
static void proc1(void *p1);
OSThread thread1;
u32 stack1[STACK_SIZE / sizeof(u32)];

BOOL flip_flag = TRUE;// Flip switch flag
BOOL swap = FALSE;

static void TitleMenu_VBlankIntr();

static GXOamAttr sOamBak[128];

static void setupSubOAM()
{
    int     i;
    int     x, y;
    int     idx = 0;

    GXS_SetOBJVRamModeBmp(GX_OBJVRAMMODE_BMP_2D_W256);

    for (i = 0; i < 128; ++i)
    {
        sOamBak[i].attr01 = 0;
        sOamBak[i].attr23 = 0;
    }

    for (y = 0; y < 192; y += 64)
    {
        for (x = 0; x < 256; x += 64, idx++)
        {
            G2_SetOBJAttr(&sOamBak[idx],
                          x,
                          y,
                          0,
                          GX_OAM_MODE_BITMAPOBJ,
                          FALSE,
                          GX_OAM_EFFECT_NONE,
                          GX_OAM_SHAPE_64x64, GX_OAM_COLOR_16, (y / 8) * 32 + (x / 8), 15, 0);
        }
    }

    DC_FlushRange(&sOamBak[0], sizeof(sOamBak));
    GXS_LoadOAM(&sOamBak[0], 0, sizeof(sOamBak));
}

void TitleMenu::Initialize(int arg)
{
	//if(arg == TITLEMENU_ARG_DONT_PLAY_INTRO)
	//{
	mState = TITLEMENU_STATE_MENU_IN;
	//}

	GX_SetBankForLCDC(GX_VRAM_LCDC_ALL);
	MI_CpuClearFast((void*)HW_LCDC_VRAM, HW_LCDC_VRAM_SIZE);
	GX_DisableBankForLCDC();

	MI_CpuFillFast((void*)HW_OAM, 192, HW_OAM_SIZE);   // clear OAM
	MI_CpuClearFast((void*)HW_PLTT, HW_PLTT_SIZE);     // clear the standard palette

	MI_CpuFillFast((void*)HW_DB_OAM, 192, HW_DB_OAM_SIZE);     // clear OAM
	MI_CpuClearFast((void*)HW_DB_PLTT, HW_DB_PLTT_SIZE);       // clear the standard palette

	OS_SetIrqFunction(OS_IE_V_BLANK, TitleMenu_VBlankIntr);

	OS_CreateThread(&thread1, proc1, NULL, stack1 + STACK_SIZE / sizeof(u32), STACK_SIZE, THREAD1_PRIO);
    OS_WakeupThreadDirect(&thread1);

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

	setupSubOAM();

	GX_SetBankForOBJ(GX_VRAM_OBJ_16_F);

	GX_SetDispSelect(GX_DISP_SELECT_MAIN_SUB);

	NNS_GfdResetLnkTexVramState();
	NNS_GfdResetLnkPlttVramState();

	NNS_G2dInitOamManagerModule();
	NNS_G2dGetNewOamManagerInstanceAsFastTransferMode(&mSubObjOamManager, 0, 128, NNS_G2D_OAMTYPE_MAIN);

	mFontData = Util_LoadFileToBuffer("/data/fonts/droid_sans_mono_10pt.NFTR", NULL);
	MI_CpuClear8(&mFont, sizeof(mFont));
	NNS_G2dFontInitAuto(&mFont, mFontData);

	mCellDataSub = Util_LoadFileToBuffer("/data/menu/title/title_obj.NCER", NULL);
	NNS_G2dGetUnpackedCellBank(mCellDataSub, &mCellDataSubBank);

	NNSG2dCharacterData* mCharDataSubUnpacked;
	void* mCharDataSub = Util_LoadFileToBuffer("/data/menu/title/title_obj.NCGR", NULL);
	NNS_G2dGetUnpackedCharacterData(mCharDataSub, &mCharDataSubUnpacked);
	NNS_G2dInitImageProxy(&mImageProxy);
	NNS_G2dLoadImage2DMapping(mCharDataSubUnpacked, 0, NNS_G2D_VRAM_TYPE_2DMAIN, &mImageProxy);
	NNS_FndFreeToExpHeap(mHeapHandle, mCharDataSub);

	NNSG2dPaletteData* mPalDataSubUnpacked;
	void* mPalDataSub = Util_LoadFileToBuffer("/data/menu/title/title_obj.NCLR", NULL);
    NNS_G2dInitImagePaletteProxy(&mImagePaletteProxy);
	NNS_G2dGetUnpackedPaletteData(mPalDataSub, &mPalDataSubUnpacked);
	NNS_G2dLoadPalette(mPalDataSubUnpacked, 0, NNS_G2D_VRAM_TYPE_2DMAIN, &mImagePaletteProxy);
	NNS_FndFreeToExpHeap(mHeapHandle, mPalDataSub);

	NNS_G2dCharCanvasInitForOBJ1D(&mCanvas, ((uint8_t*)G2_GetOBJCharPtr()) + 2048, 12, 2, NNS_G2D_CHARA_COLORMODE_16);
	NNS_G2dTextCanvasInit(&mTextCanvas, &mCanvas, &mFont, 0, 1);
	NNS_G2dCharCanvasClear(&mCanvas, 0);
	NNS_G2dTextCanvasDrawTextRect(
		&mTextCanvas, 0, 0, 96, 16, 4, NNS_G2D_VERTICALORIGIN_TOP | NNS_G2D_HORIZONTALORIGIN_LEFT | NNS_G2D_HORIZONTALALIGN_CENTER | NNS_G2D_VERTICALALIGN_MIDDLE, (NNSG2dChar*)L"Missions");

	mCanvas.charBase = ((uint8_t*)G2_GetOBJCharPtr()) + 2048 * 2;
	NNS_G2dTextCanvasDrawTextRect(
		&mTextCanvas, 0, 0, 96, 16, 4, NNS_G2D_VERTICALORIGIN_TOP | NNS_G2D_HORIZONTALORIGIN_LEFT | NNS_G2D_HORIZONTALALIGN_CENTER | NNS_G2D_VERTICALALIGN_MIDDLE, (NNSG2dChar*)L"Sandbox");

	mCanvas.charBase = ((uint8_t*)G2_GetOBJCharPtr()) + 2048 * 3;
	NNS_G2dTextCanvasDrawTextRect(
		&mTextCanvas, 0, 0, 64, 16, 4, NNS_G2D_VERTICALORIGIN_TOP | NNS_G2D_HORIZONTALORIGIN_LEFT | NNS_G2D_HORIZONTALALIGN_CENTER | NNS_G2D_VERTICALALIGN_MIDDLE, (NNSG2dChar*)L"Depot");

	mCanvas.charBase = ((uint8_t*)G2_GetOBJCharPtr()) + 2048 * 4;
	NNS_G2dTextCanvasDrawTextRect(
		&mTextCanvas, 0, 0, 64, 16, 4, NNS_G2D_VERTICALORIGIN_TOP | NNS_G2D_HORIZONTALORIGIN_LEFT | NNS_G2D_HORIZONTALALIGN_CENTER | NNS_G2D_VERTICALALIGN_MIDDLE, (NNSG2dChar*)L"Options");


	MI_CpuClear8(&mTmpSubOamBuffer[0], sizeof(mTmpSubOamBuffer));
	mSelButton = 0;
	mStateCounter = 0;
	mKeyTimeout = 0;
}

void TitleMenu::HandleKeys()
{
	if(!mKeyTimeout)
	{
		u16 keyData = PAD_Read();
		switch(mState)
		{
		case TITLEMENU_STATE_MENU_LOOP:
			if(keyData & PAD_KEY_DOWN)
			{
				if(mSelButton == 0 || mSelButton == 1) mSelButton++;
				else if(mSelButton == 2) mSelButton = 0;
				else if(mSelButton == 3) mSelButton = 0;
				else break;
				mKeyTimeout = 10;
			}
			else if(keyData & PAD_KEY_UP)
			{
				if(mSelButton == 1 || mSelButton == 2) mSelButton--;
				else if(mSelButton == 0) mSelButton = 2;
				else if(mSelButton == 3) mSelButton = 1;
				else break;
				mKeyTimeout = 10;
			}
			else if(keyData & PAD_KEY_LEFT || keyData & PAD_KEY_RIGHT)
			{
				if(mSelButton == 2) mSelButton = 3;
				else if(mSelButton == 3) mSelButton = 2;
				else break;
				mKeyTimeout = 10;
			}
			break;
		}
	}
	else mKeyTimeout--;
}

static void SetSwapBuffersflag(void)
{
    OSIntrMode old = OS_DisableInterrupts();    // interrupts disabled
    G3_SwapBuffers(GX_SORTMODE_AUTO, GX_BUFFERMODE_W);
    swap = TRUE;
    OS_RestoreInterrupts(old);
}

void TitleMenu::Render()
{
	HandleKeys();
	const NNSG2dCellData *pButtonLarge, *pButtonSmall, *pButtonLargeSel, *pButtonSmallSel;
	NNSG2dFVec2 trans;
	u16 numOamDrawn = 0;
	int i;
	switch(mState)
	{
	case TITLEMENU_STATE_INTRO:
		break;
	case TITLEMENU_STATE_MENU_IN:
		if(mStateCounter == 0)
		{
			NNS_SndStrmHandleInit(&mMusicHandle);
			NNS_SndArcStrmStart(&mMusicHandle, STRM_TITLE, 0);
		}
		pButtonLarge = NNS_G2dGetCellDataByIdx(mCellDataSubBank, 0);
		pButtonLargeSel = NNS_G2dGetCellDataByIdx(mCellDataSubBank, 2);
		pButtonSmall = NNS_G2dGetCellDataByIdx(mCellDataSubBank, 1);
		pButtonSmallSel = NNS_G2dGetCellDataByIdx(mCellDataSubBank, 3);
		if(mStateCounter < 30)
		{
			trans.x = 80 * FX32_ONE;
			trans.y = 192 * FX32_ONE + (76 - 192) * mStateCounter * FX32_ONE / 30;
			numOamDrawn += NNS_G2dMakeCellToOams(&mTmpSubOamBuffer[numOamDrawn], 128 - numOamDrawn, (mSelButton == 0 ? pButtonLargeSel : pButtonLarge), NULL, &trans, -1, FALSE);
			trans.y = 216 * FX32_ONE + (100 - 216) * mStateCounter * FX32_ONE / 30;
			numOamDrawn += NNS_G2dMakeCellToOams(&mTmpSubOamBuffer[numOamDrawn], 128 - numOamDrawn, (mSelButton == 1 ? pButtonLargeSel : pButtonLarge), NULL, &trans, -1, FALSE);
		}
		else
		{
			trans.x = 80 * FX32_ONE;
			trans.y = 76 * FX32_ONE;
			numOamDrawn += NNS_G2dMakeCellToOams(&mTmpSubOamBuffer[numOamDrawn], 128 - numOamDrawn, (mSelButton == 0 ? pButtonLargeSel : pButtonLarge), NULL, &trans, -1, FALSE);
			trans.y = 100 * FX32_ONE;
			numOamDrawn += NNS_G2dMakeCellToOams(&mTmpSubOamBuffer[numOamDrawn], 128 - numOamDrawn, (mSelButton == 1 ? pButtonLargeSel : pButtonLarge), NULL, &trans, -1, FALSE);
		}
		if(mStateCounter >= 30)
		{
			trans.x = -64 * FX32_ONE + (8 + 64) * (mStateCounter - 30) * FX32_ONE / 30;
			trans.y = 168 * FX32_ONE;
			numOamDrawn += NNS_G2dMakeCellToOams(&mTmpSubOamBuffer[numOamDrawn], 128 - numOamDrawn, (mSelButton == 2 ? pButtonSmallSel : pButtonSmall), NULL, &trans, -1, FALSE);
			trans.x = 256 * FX32_ONE + (184 - 256) * (mStateCounter - 30) * FX32_ONE / 30;
			numOamDrawn += NNS_G2dMakeCellToOams(&mTmpSubOamBuffer[numOamDrawn], 128 - numOamDrawn, (mSelButton == 3 ? pButtonSmallSel : pButtonSmall), NULL, &trans, -1, FALSE);
		}

		for(i = 0; i < numOamDrawn; i++)
		{
			G2_SetOBJPriority(&((GXOamAttr*)&mTmpSubOamBuffer[0])[i], 3);
		}
		if(mStateCounter < 30)
		{
			numOamDrawn += NNS_G2dArrangeOBJ1D((GXOamAttr*)&mTmpSubOamBuffer[numOamDrawn], 12, 2, 80, 192 + (76 - 192) * mStateCounter / 30, GX_OAM_COLORMODE_16, 64, NNS_G2D_OBJVRAMMODE_32K);
			numOamDrawn += NNS_G2dArrangeOBJ1D((GXOamAttr*)&mTmpSubOamBuffer[numOamDrawn], 12, 2, 80, 216 + (100 - 216) * mStateCounter / 30, GX_OAM_COLORMODE_16, 64 * 2, NNS_G2D_OBJVRAMMODE_32K);
		}
		else
		{
			numOamDrawn += NNS_G2dArrangeOBJ1D((GXOamAttr*)&mTmpSubOamBuffer[numOamDrawn], 12, 2, 80, 76, GX_OAM_COLORMODE_16, 64, NNS_G2D_OBJVRAMMODE_32K);
			numOamDrawn += NNS_G2dArrangeOBJ1D((GXOamAttr*)&mTmpSubOamBuffer[numOamDrawn], 12, 2, 80, 100, GX_OAM_COLORMODE_16, 64 * 2, NNS_G2D_OBJVRAMMODE_32K);
		}
		if(mStateCounter >= 30)
		{
			numOamDrawn += NNS_G2dArrangeOBJ1D((GXOamAttr*)&mTmpSubOamBuffer[numOamDrawn], 8, 2, -64 + (8 + 64) * (mStateCounter - 30) / 30, 168, GX_OAM_COLORMODE_16, 64 * 3, NNS_G2D_OBJVRAMMODE_32K);
			numOamDrawn += NNS_G2dArrangeOBJ1D((GXOamAttr*)&mTmpSubOamBuffer[numOamDrawn], 9, 2, 256 + (184 - 256) * (mStateCounter - 30) / 30, 168, GX_OAM_COLORMODE_16, 64 * 4, NNS_G2D_OBJVRAMMODE_32K);
		}

		NNS_G2dEntryOamManagerOam(&mSubObjOamManager, &mTmpSubOamBuffer[0], numOamDrawn);

		mStateCounter++;
		if(mStateCounter >= 60)
		{
			mState = TITLEMENU_STATE_MENU_LOOP;
			mStateCounter = 0;
		}
		break;
	case TITLEMENU_STATE_MENU_LOOP:
		pButtonLarge = NNS_G2dGetCellDataByIdx(mCellDataSubBank, 0);
		pButtonLargeSel = NNS_G2dGetCellDataByIdx(mCellDataSubBank, 2);
		pButtonSmall = NNS_G2dGetCellDataByIdx(mCellDataSubBank, 1);
		pButtonSmallSel = NNS_G2dGetCellDataByIdx(mCellDataSubBank, 3);
		trans.x = 80 * FX32_ONE;
		trans.y = 76 * FX32_ONE;
		numOamDrawn += NNS_G2dMakeCellToOams(&mTmpSubOamBuffer[numOamDrawn], 128 - numOamDrawn, (mSelButton == 0 ? pButtonLargeSel : pButtonLarge), NULL, &trans, -1, FALSE);
		trans.y = 100 * FX32_ONE;
		numOamDrawn += NNS_G2dMakeCellToOams(&mTmpSubOamBuffer[numOamDrawn], 128 - numOamDrawn, (mSelButton == 1 ? pButtonLargeSel : pButtonLarge), NULL, &trans, -1, FALSE);
		trans.x = 8 * FX32_ONE;
		trans.y = 168 * FX32_ONE;
		numOamDrawn += NNS_G2dMakeCellToOams(&mTmpSubOamBuffer[numOamDrawn], 128 - numOamDrawn, (mSelButton == 2 ? pButtonSmallSel : pButtonSmall), NULL, &trans, -1, FALSE);
		trans.x = 184 * FX32_ONE;
		numOamDrawn += NNS_G2dMakeCellToOams(&mTmpSubOamBuffer[numOamDrawn], 128 - numOamDrawn, (mSelButton == 3 ? pButtonSmallSel : pButtonSmall), NULL, &trans, -1, FALSE);

		for(i = 0; i < numOamDrawn; i++)
		{
			G2_SetOBJPriority(&((GXOamAttr*)&mTmpSubOamBuffer[0])[i], 3);
		}
		numOamDrawn += NNS_G2dArrangeOBJ1D((GXOamAttr*)&mTmpSubOamBuffer[numOamDrawn], 12, 2, 80, 76, GX_OAM_COLORMODE_16, 64, NNS_G2D_OBJVRAMMODE_32K);
		numOamDrawn += NNS_G2dArrangeOBJ1D((GXOamAttr*)&mTmpSubOamBuffer[numOamDrawn], 12, 2, 80, 100, GX_OAM_COLORMODE_16, 64 * 2, NNS_G2D_OBJVRAMMODE_32K);
		numOamDrawn += NNS_G2dArrangeOBJ1D((GXOamAttr*)&mTmpSubOamBuffer[numOamDrawn], 8, 2, 8, 168, GX_OAM_COLORMODE_16, 64 * 3, NNS_G2D_OBJVRAMMODE_32K);
		numOamDrawn += NNS_G2dArrangeOBJ1D((GXOamAttr*)&mTmpSubOamBuffer[numOamDrawn], 8, 2, 184, 168, GX_OAM_COLORMODE_16, 64 * 4, NNS_G2D_OBJVRAMMODE_32K);
		
		NNS_G2dEntryOamManagerOam(&mSubObjOamManager, &mTmpSubOamBuffer[0], numOamDrawn);
		break;
	case TITLEMENU_STATE_MENU_OUT:
		break;
	}
	G3X_Reset();
	//Do 3d stuff here
	SetSwapBuffersflag();
}

void TitleMenu::VBlank()
{
	NNS_G2dApplyOamManagerToHW(&mSubObjOamManager);
    NNS_G2dResetOamManagerBuffer(&mSubObjOamManager);
}

void TitleMenu::Finalize()
{
	NNS_FndFreeToExpHeap(mHeapHandle, mFontData);
	NNS_FndFreeToExpHeap(mHeapHandle, mCellDataSub);
}

static void TitleMenu_VBlankIntr()
{
	OS_SetIrqCheckFlag(OS_IE_V_BLANK);
	OS_WakeupThreadDirect(&thread1);
}

static void setupFrame2N(void)
{
    GX_SetDispSelect(GX_DISP_SELECT_MAIN_SUB);

    GX_ResetBankForSubOBJ();
    GX_SetBankForSubBG(GX_VRAM_SUB_BG_128_C);
    GX_SetBankForLCDC(GX_VRAM_LCDC_D);
    GX_SetCapture(GX_CAPTURE_SIZE_256x192,
                  GX_CAPTURE_MODE_A,
                  GX_CAPTURE_SRCA_2D3D, (GXCaptureSrcB)0, GX_CAPTURE_DEST_VRAM_D_0x00000, 16, 0);

    GX_SetGraphicsMode(GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BG0_AS_3D);
    GX_SetVisiblePlane(GX_PLANEMASK_BG0);
    G2_SetBG0Priority(3);

    GXS_SetGraphicsMode(GX_BGMODE_5);
    GXS_SetVisiblePlane(GX_PLANEMASK_BG2);
    G2S_SetBG2ControlDCBmp(GX_BG_SCRSIZE_DCBMP_256x256,
                           GX_BG_AREAOVER_XLU, GX_BG_BMPSCRBASE_0x00000);
    G2S_SetBG2Priority(3);
    G2S_BG2Mosaic(FALSE);
}

static void setupFrame2N_1(void)
{
    GX_SetDispSelect(GX_DISP_SELECT_SUB_MAIN);

    GX_ResetBankForSubBG();
    GX_SetBankForSubOBJ(GX_VRAM_SUB_OBJ_128_D);
    GX_SetBankForLCDC(GX_VRAM_LCDC_C);
    GX_SetCapture(GX_CAPTURE_SIZE_256x192,
                  GX_CAPTURE_MODE_A,
				  GX_CAPTURE_SRCA_2D3D, (GXCaptureSrcB)0, GX_CAPTURE_DEST_VRAM_C_0x00000, 16, 0);

    GX_SetGraphicsMode(GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BG0_AS_3D);
    GX_SetVisiblePlane(GX_PLANEMASK_BG0 | GX_PLANEMASK_OBJ);
    G2_SetBG0Priority(3);

    GXS_SetGraphicsMode(GX_BGMODE_5);
    GXS_SetVisiblePlane(GX_PLANEMASK_OBJ);
}

//Thread for double 3d
static void proc1(void *p1)
{
    while (1)
    {
		if (GX_GetVCount() <= 193)
		{
			OS_SpinWait(784);
		}
		if (!G3X_IsGeometryBusy() && swap)
		// If the rendering and swap operations are both finished, the upper and lower screens switch.
		{
			if (flip_flag)                 // flip switch (operation to switch the upper and lower screens)
			{
				setupFrame2N_1();
			}
			else
			{
				setupFrame2N();
			}
			swap = FALSE;
			flip_flag = !flip_flag;
		}
		OS_SleepThread(NULL);   // Puts this thread to sleep. It will next wake when OS_WakeupThreadDirect() is called.
    }
}