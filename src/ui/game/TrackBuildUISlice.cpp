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
	return TRACKBUILD_BUTTON_INVALID;
}

BOOL TrackBuildUISlice::OnPenDown(Menu* context, int x, int y)
{
	if(!(x > (mX + 192) && x < (mX + 256) && y > mY && y < (mY + 192))) return FALSE;
	mDownButton = FindButtonByPoint(x, y);
	return TRUE;
}

BOOL TrackBuildUISlice::OnPenMove(Menu* context, int x, int y)
{
	if(!(x > (mX + 192) && x < (mX + 256) && y > mY && y < (mY + 192))) return FALSE;
	return TRUE;
}

BOOL TrackBuildUISlice::OnPenUp(Menu* context, int x, int y)
{
	if(!(x > (mX + 192) && x < (mX + 256) && y > mY && y < (mY + 192))) return FALSE;
	int button = FindButtonByPoint(x, y);
	if(button == mDownButton && button != TRACKBUILD_BUTTON_INVALID)//handle a click
	{
		switch(button)
		{

		}
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
	pData = NNS_G2dGetCellDataByIdx(mGame->mTrackBuildCellDataBank, 10);
	numOamDrawn += NNS_G2dMakeCellToOams(&gOamTmpBuffer[numOamDrawn], 128 - numOamDrawn, pData, NULL, &trans, -1, FALSE);
	//Big Left
	trans.x = 200 * FX32_ONE + mX * FX32_ONE;
	trans.y = 41 * FX32_ONE + mY * FX32_ONE;
	pData = NNS_G2dGetCellDataByIdx(mGame->mTrackBuildCellDataBank, 1);
	numOamDrawn += NNS_G2dMakeCellToOams(&gOamTmpBuffer[numOamDrawn], 128 - numOamDrawn, pData, NULL, &trans, -1, FALSE);
	//Big Right
	trans.x = 232 * FX32_ONE + mX * FX32_ONE;
	trans.y = 41 * FX32_ONE + mY * FX32_ONE;
	pData = NNS_G2dGetCellDataByIdx(mGame->mTrackBuildCellDataBank, 3);
	numOamDrawn += NNS_G2dMakeCellToOams(&gOamTmpBuffer[numOamDrawn], 128 - numOamDrawn, pData, NULL, &trans, -1, FALSE);
	//Small Left
	trans.x = 200 * FX32_ONE + mX * FX32_ONE;
	trans.y = 77 * FX32_ONE + mY * FX32_ONE;
	pData = NNS_G2dGetCellDataByIdx(mGame->mTrackBuildCellDataBank, 5);
	numOamDrawn += NNS_G2dMakeCellToOams(&gOamTmpBuffer[numOamDrawn], 128 - numOamDrawn, pData, NULL, &trans, -1, FALSE);
	//Small Right
	trans.x = 232 * FX32_ONE + mX * FX32_ONE;
	trans.y = 77 * FX32_ONE + mY * FX32_ONE;
	pData = NNS_G2dGetCellDataByIdx(mGame->mTrackBuildCellDataBank, 7);
	numOamDrawn += NNS_G2dMakeCellToOams(&gOamTmpBuffer[numOamDrawn], 128 - numOamDrawn, pData, NULL, &trans, -1, FALSE);
	//No Slope
	trans.x = 208 * FX32_ONE + mX * FX32_ONE;
	trans.y = 122 * FX32_ONE + mY * FX32_ONE;
	pData = NNS_G2dGetCellDataByIdx(mGame->mTrackBuildCellDataBank, 16);
	numOamDrawn += NNS_G2dMakeCellToOams(&gOamTmpBuffer[numOamDrawn], 128 - numOamDrawn, pData, NULL, &trans, -1, FALSE);
	//Slope Down
	trans.x = 200 * FX32_ONE + mX * FX32_ONE;
	trans.y = 150 * FX32_ONE + mY * FX32_ONE;
	pData = NNS_G2dGetCellDataByIdx(mGame->mTrackBuildCellDataBank, 11);
	numOamDrawn += NNS_G2dMakeCellToOams(&gOamTmpBuffer[numOamDrawn], 128 - numOamDrawn, pData, NULL, &trans, -1, FALSE);
	//Slope Up
	trans.x = 232 * FX32_ONE + mX * FX32_ONE;
	trans.y = 149 * FX32_ONE + mY * FX32_ONE;
	pData = NNS_G2dGetCellDataByIdx(mGame->mTrackBuildCellDataBank, 13);
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
		G3_MtxMode(GX_MTXMODE_TEXTURE);
		{
			G3_Identity();
		}
		G3_MtxMode(GX_MTXMODE_POSITION_VECTOR);
		{
			//G3_Identity();
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
			VecFx32 near1, near2, near3, near4;
			NNS_G3dScrPosToWorldLine(192 - 8, 0, &near1, NULL);
			NNS_G3dScrPosToWorldLine(192 - 8, 192, &near2, NULL);
			NNS_G3dScrPosToWorldLine(192, 192, &near3, NULL);
			NNS_G3dScrPosToWorldLine(192, 0, &near4, NULL);
			VEC_Subtract(&near2, &near1, &near2);
			VEC_Subtract(&near3, &near1, &near3);
			VEC_Subtract(&near4, &near1, &near4);

			G3_PolygonAttr(GX_LIGHTMASK_NONE, GX_POLYGONMODE_MODULATE, GX_CULL_BACK, 0, 31, GX_POLYGON_ATTR_MISC_NONE);
			G3_PushMtx();
			{
				G3_Translate(near1.x, near1.y, near1.z);
				G3_Color(GX_RGB(31, 31, 31));
				G3_Begin(GX_BEGIN_QUADS);
				{
					G3_TexCoord(0, 0);
					G3_Vtx(0, 0, 0);
					G3_TexCoord(0, (8 << mGame->mShadowTex.nitroHeight) * FX32_ONE);
					G3_Vtx(near2.x, near2.y, near2.z);
					G3_TexCoord((8 << mGame->mShadowTex.nitroWidth) * FX32_ONE, (8 << mGame->mShadowTex.nitroHeight) * FX32_ONE);
					G3_Vtx(near3.x, near3.y, near3.z);
					G3_TexCoord((8 << mGame->mShadowTex.nitroWidth) * FX32_ONE, 0);
					G3_Vtx(near4.x, near4.y, near4.z);
				}
				G3_End();
			}
			G3_PopMtx(1);
		}
	}
}