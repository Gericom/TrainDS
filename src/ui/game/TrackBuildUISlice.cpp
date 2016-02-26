#include <nitro.h>
#include <nnsys/g2d.h>
#include <nnsys/fnd.h>
#include "core.h"
#include "menu\Menu.h"
#include "menu\Game.h"
#include "ui\UISlice.h"
#include "TrackBuildUISlice.h"

int TrackBuildUISlice::FindButtonByPoint(int x, int y)
{
	if(x >= 212 && x < (212 + 24) && y >= 17 && y < (17 + 24)) return TRACKBUILD_BUTTON_DIR_STRAIGHT;
	if(x >= 196 && x < (196 + 24) && y >= 45 && y < (45 + 24)) return TRACKBUILD_BUTTON_DIR_LEFT_BIG;
	if(x >= 228 && x < (228 + 24) && y >= 45 && y < (45 + 24)) return TRACKBUILD_BUTTON_DIR_RIGHT_BIG;
	if(x >= 196 && x < (196 + 24) && y >= 73 && y < (73 + 24)) return TRACKBUILD_BUTTON_DIR_LEFT_SMALL;
	if(x >= 228 && x < (228 + 24) && y >= 73 && y < (73 + 24)) return TRACKBUILD_BUTTON_DIR_RIGHT_SMALL;
	if(x >= 212 && x < (212 + 24) && y >= 118 && y < (118 + 24)) return TRACKBUILD_BUTTON_SLOPE_NONE;
	if(x >= 196 && x < (196 + 24) && y >= 146 && y < (146 + 24)) return TRACKBUILD_BUTTON_SLOPE_DOWN;
	if(x >= 228 && x < (228 + 24) && y >= 146 && y < (146 + 24)) return TRACKBUILD_BUTTON_SLOPE_UP;
	return TRACKBUILD_BUTTON_INVALID;
}

BOOL TrackBuildUISlice::OnPenDown(Menu* context, int x, int y)
{
	x -= mX;
	y -= mY;
	if(!(x >= 192 && x < 256 && y >= 0 && y < 192)) return FALSE;
	mDownButton = FindButtonByPoint(x, y);
	return TRUE;
}

BOOL TrackBuildUISlice::OnPenMove(Menu* context, int x, int y)
{
	x -= mX;
	y -= mY;
	if(!(x >= 192 && x < 256 && y >= 0 && y < 192)) return FALSE;
	return TRUE;
}

BOOL TrackBuildUISlice::OnPenUp(Menu* context, int x, int y)
{
	x -= mX;
	y -= mY;
	if(!(x >= 192 && x < 256 && y >= 0 && y < 192)) return FALSE;
	int button = FindButtonByPoint(x, y);
	if(button == mDownButton && button != TRACKBUILD_BUTTON_INVALID)//handle a click
	{
		if(button >= TRACKBUILD_BUTTON_SLOPE_NONE)
			mSlopeSelection = button;
		else mDirectionSelection = button;
	}
	return TRUE;
}

void TrackBuildUISlice::Render(Menu* context, NNSG2dOamManagerInstance* oamManager)
{
	Game* mGame = (Game*)context;
	MI_CpuClear8(&gOamTmpBuffer[0], sizeof(gOamTmpBuffer));
	u16 numOamDrawn = 0;
	NNSG2dFVec2 trans;
	//BG
	trans.x = 192 * FX32_ONE + mX * FX32_ONE;
	trans.y = 96 * FX32_ONE + mY * FX32_ONE;
	const NNSG2dCellData* pData = NNS_G2dGetCellDataByIdx(mGame->mTrackBuildCellDataBank, 0);
	numOamDrawn += NNS_G2dMakeCellToOams(&gOamTmpBuffer[numOamDrawn], 128 - numOamDrawn, pData, NULL, &trans, -1, FALSE);
	for(int i = 0; i < numOamDrawn; i++) 
		G2_SetOBJPriority(&gOamTmpBuffer[i], 3);
	//Straight
	trans.x = 216 * FX32_ONE + mX * FX32_ONE;
	trans.y = 13 * FX32_ONE + mY * FX32_ONE;
	pData = NNS_G2dGetCellDataByIdx(mGame->mTrackBuildCellDataBank, 9 + ((mDirectionSelection == TRACKBUILD_BUTTON_DIR_STRAIGHT) ? 1 : 0));
	numOamDrawn += NNS_G2dMakeCellToOams(&gOamTmpBuffer[numOamDrawn], 128 - numOamDrawn, pData, NULL, &trans, -1, FALSE);
	//Big Left
	trans.x = 200 * FX32_ONE + mX * FX32_ONE;
	trans.y = 41 * FX32_ONE + mY * FX32_ONE;
	pData = NNS_G2dGetCellDataByIdx(mGame->mTrackBuildCellDataBank, 1 + ((mDirectionSelection == TRACKBUILD_BUTTON_DIR_LEFT_BIG) ? 1 : 0));
	numOamDrawn += NNS_G2dMakeCellToOams(&gOamTmpBuffer[numOamDrawn], 128 - numOamDrawn, pData, NULL, &trans, -1, FALSE);
	//Big Right
	trans.x = 232 * FX32_ONE + mX * FX32_ONE;
	trans.y = 41 * FX32_ONE + mY * FX32_ONE;
	pData = NNS_G2dGetCellDataByIdx(mGame->mTrackBuildCellDataBank, 3 + ((mDirectionSelection == TRACKBUILD_BUTTON_DIR_RIGHT_BIG) ? 1 : 0));
	numOamDrawn += NNS_G2dMakeCellToOams(&gOamTmpBuffer[numOamDrawn], 128 - numOamDrawn, pData, NULL, &trans, -1, FALSE);
	//Small Left
	trans.x = 200 * FX32_ONE + mX * FX32_ONE;
	trans.y = 77 * FX32_ONE + mY * FX32_ONE;
	pData = NNS_G2dGetCellDataByIdx(mGame->mTrackBuildCellDataBank, 5 + ((mDirectionSelection == TRACKBUILD_BUTTON_DIR_LEFT_SMALL) ? 1 : 0));
	numOamDrawn += NNS_G2dMakeCellToOams(&gOamTmpBuffer[numOamDrawn], 128 - numOamDrawn, pData, NULL, &trans, -1, FALSE);
	//Small Right
	trans.x = 232 * FX32_ONE + mX * FX32_ONE;
	trans.y = 77 * FX32_ONE + mY * FX32_ONE;
	pData = NNS_G2dGetCellDataByIdx(mGame->mTrackBuildCellDataBank, 7 + ((mDirectionSelection == TRACKBUILD_BUTTON_DIR_RIGHT_SMALL) ? 1 : 0));
	numOamDrawn += NNS_G2dMakeCellToOams(&gOamTmpBuffer[numOamDrawn], 128 - numOamDrawn, pData, NULL, &trans, -1, FALSE);
	//No Slope
	trans.x = 208 * FX32_ONE + mX * FX32_ONE;
	trans.y = 122 * FX32_ONE + mY * FX32_ONE;
	pData = NNS_G2dGetCellDataByIdx(mGame->mTrackBuildCellDataBank, 15 + ((mSlopeSelection == TRACKBUILD_BUTTON_SLOPE_NONE) ? 1 : 0));
	numOamDrawn += NNS_G2dMakeCellToOams(&gOamTmpBuffer[numOamDrawn], 128 - numOamDrawn, pData, NULL, &trans, -1, FALSE);
	//Slope Down
	trans.x = 200 * FX32_ONE + mX * FX32_ONE;
	trans.y = 150 * FX32_ONE + mY * FX32_ONE;
	pData = NNS_G2dGetCellDataByIdx(mGame->mTrackBuildCellDataBank, 11 + ((mSlopeSelection == TRACKBUILD_BUTTON_SLOPE_DOWN) ? 1 : 0));
	numOamDrawn += NNS_G2dMakeCellToOams(&gOamTmpBuffer[numOamDrawn], 128 - numOamDrawn, pData, NULL, &trans, -1, FALSE);
	//Slope Up
	trans.x = 232 * FX32_ONE + mX * FX32_ONE;
	trans.y = 149 * FX32_ONE + mY * FX32_ONE;
	pData = NNS_G2dGetCellDataByIdx(mGame->mTrackBuildCellDataBank, 13 + ((mSlopeSelection == TRACKBUILD_BUTTON_SLOPE_UP) ? 1 : 0));
	numOamDrawn += NNS_G2dMakeCellToOams(&gOamTmpBuffer[numOamDrawn], 128 - numOamDrawn, pData, NULL, &trans, -1, FALSE);
	//Add
	trans.x = 193 * FX32_ONE + mX * FX32_ONE;
	trans.y = 166 * FX32_ONE + mY * FX32_ONE;
	pData = NNS_G2dGetCellDataByIdx(mGame->mTrackBuildCellDataBank, 17);
	numOamDrawn += NNS_G2dMakeCellToOams(&gOamTmpBuffer[numOamDrawn], 128 - numOamDrawn, pData, NULL, &trans, -1, FALSE);
	//Delete
	trans.x = 225 * FX32_ONE + mX * FX32_ONE;
	trans.y = 166 * FX32_ONE + mY * FX32_ONE;
	pData = NNS_G2dGetCellDataByIdx(mGame->mTrackBuildCellDataBank, 18);
	numOamDrawn += NNS_G2dMakeCellToOams(&gOamTmpBuffer[numOamDrawn], 128 - numOamDrawn, pData, NULL, &trans, -1, FALSE);

	u16 oldnrdrawn = numOamDrawn;
	//Direction
	numOamDrawn += NNS_G2dArrangeOBJ1D(&gOamTmpBuffer[numOamDrawn], 8, 2, 192, 0, GX_OAM_COLORMODE_16, 32 * 8, NNS_G2D_OBJVRAMMODE_32K);
	//Slope
	numOamDrawn += NNS_G2dArrangeOBJ1D(&gOamTmpBuffer[numOamDrawn], 8, 2, 192, 101, GX_OAM_COLORMODE_16, 32 * 9, NNS_G2D_OBJVRAMMODE_32K);
	for(int i = oldnrdrawn; i < numOamDrawn; i++)
		G2_SetOBJMode(&gOamTmpBuffer[i], GX_OAM_MODE_NORMAL, 3);

	NNS_G2dEntryOamManagerOam(oamManager, &gOamTmpBuffer[0], numOamDrawn);

	if(!mGame->mPicking)
	{
		G3_MtxMode(GX_MTXMODE_PROJECTION);
		{
			G3_Identity();
			G3_OrthoW(0, 192 * FX32_ONE, 0, 256 * FX32_ONE, 0 * FX32_ONE, 256 * FX32_ONE, 1 * FX32_ONE, NULL);
		}
		G3_MtxMode(GX_MTXMODE_TEXTURE);
		{
			G3_Identity();
		}
		G3_MtxMode(GX_MTXMODE_POSITION_VECTOR);
		{
			G3_TexImageParam(
				(GXTexFmt)mGame->mShadowTex.nitroFormat,
				GX_TEXGEN_TEXCOORD,
				(GXTexSizeS)mGame->mShadowTex.nitroWidth,
				(GXTexSizeT)mGame->mShadowTex.nitroHeight,
				GX_TEXREPEAT_T,
				GX_TEXFLIP_NONE,
				GX_TEXPLTTCOLOR0_USE,
				NNS_GfdGetTexKeyAddr(mGame->mShadowTex.texKey)
			);
			G3_TexPlttBase(NNS_GfdGetPlttKeyAddr(mGame->mShadowTex.plttKey), (GXTexFmt)mGame->mShadowTex.nitroFormat);

			G3_PolygonAttr(GX_LIGHTMASK_NONE, GX_POLYGONMODE_MODULATE, GX_CULL_BACK, 0, 31, GX_POLYGON_ATTR_MISC_NONE);
			G3_PushMtx();
			{
				G3_Identity();
				G3_Translate(((mX + 192 - 8) << FX32_SHIFT) + ((8 >> 1) << FX32_SHIFT), (mY << FX32_SHIFT) + ((192 >> 1) << FX32_SHIFT), 0 << FX32_SHIFT);
				G3_Scale(8 << FX32_SHIFT, 192 << FX32_SHIFT, FX32_ONE);

				G3_Color(GX_RGB(31, 31, 31));
				G3_Begin(GX_BEGIN_QUADS);
				{
					G3_TexCoord(0, 0);
					G3_Vtx(-FX32_HALF, -FX32_HALF, 0);
					G3_TexCoord(0, (8 << mGame->mShadowTex.nitroHeight) * FX32_ONE);
					G3_Vtx(-FX32_HALF, FX32_HALF, 0);
					G3_TexCoord((8 << mGame->mShadowTex.nitroWidth) * FX32_ONE, (8 << mGame->mShadowTex.nitroHeight) * FX32_ONE);
					G3_Vtx(FX32_HALF, FX32_HALF,  0);
					G3_TexCoord((8 << mGame->mShadowTex.nitroWidth) * FX32_ONE, 0);
					G3_Vtx(FX32_HALF, -FX32_HALF,  0);
				}
				G3_End();
			}
			G3_PopMtx(1);
		}
		G3_MtxMode(GX_MTXMODE_PROJECTION);
		{
			G3_RestoreMtx(0);
		}
	}
}