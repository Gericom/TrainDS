#include <nitro.h>
#include <nnsys/fnd.h>
#include <nnsys/g3d.h>
#include <math.h>
#include <cmath>
#include "core.h"
#include "util.h"
#include "engine/objects/Water.h"
#include "TerrainManager.h"
#include "GameController.h"

static const GXRgb sToonTable[32] =
{
	//GX_RGB(0, 0, 0), GX_RGB(1, 1, 1), GX_RGB(2, 2, 2), GX_RGB(3, 3, 3),
	//GX_RGB(4, 4, 4), GX_RGB(5, 5, 5), GX_RGB(6, 6, 6), GX_RGB(7, 7, 7),
	//GX_RGB(8, 8, 8), GX_RGB(9, 9, 9), GX_RGB(10, 10, 10), GX_RGB(11, 11, 11),
	//GX_RGB(12, 12, 12), GX_RGB(13, 13, 13), GX_RGB(14, 14, 14), GX_RGB(15, 15, 15),
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	GX_RGB(0, 0, 0), GX_RGB(1, 1, 1), GX_RGB(2, 2, 2), GX_RGB(3, 3, 3),
	GX_RGB(4, 4, 4), GX_RGB(5, 5, 5), GX_RGB(6, 6, 6), GX_RGB(7, 7, 7),
	//GX_RGB(8, 8, 8), GX_RGB(9, 9, 9), GX_RGB(10, 10, 10), GX_RGB(11, 11, 11),
	//GX_RGB(12, 12, 12), GX_RGB(13, 13, 13), GX_RGB(14, 14, 14), GX_RGB(15, 15, 15)
	//GX_RGB(12, 12, 12), GX_RGB(13, 13, 13), GX_RGB(14, 14, 14), GX_RGB(15, 15, 15),
	//GX_RGB(16, 16, 16), GX_RGB(17, 17, 17), GX_RGB(18, 18, 18), GX_RGB(19, 19, 19),
	//GX_RGB(20, 20, 20), GX_RGB(21, 21, 21), GX_RGB(22, 22, 22), GX_RGB(23, 23, 23),
	//GX_RGB(24, 24, 24), GX_RGB(25, 25, 25), GX_RGB(26, 26, 26), GX_RGB(27, 27, 27),
	//GX_RGB(28, 28, 28), GX_RGB(29, 29, 29), GX_RGB(30, 30, 30), GX_RGB(31, 31, 31)
	//0, 0, 0, 0, 0, 0, 0, 0,
	//0, 0, 0, 0, 0, 0, 0, 0,
	//0, 0, 0, 0, 0, 0, 0, 0,
	//0, 0, 0, 0, 0, 0,
	//GX_RGB(31,31,31),
	//GX_RGB(4, 4, 4)
};

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

static void calculateVisibleGrid(VecFx32* bbmin, VecFx32* bbmax)
{
	int y = 0;

	//calculate the corners of the frustum box
	VecFx32 far_top_left;
	VecFx32 near_top_left;
	NNS_G3dScrPosToWorldLine(0, 0, &near_top_left, &far_top_left);

	VecFx32 far_top_right;
	VecFx32 near_top_right;
	NNS_G3dScrPosToWorldLine(255, 0, &near_top_right, &far_top_right);

	VecFx32 far_bottom_left;
	VecFx32 near_bottom_left;
	NNS_G3dScrPosToWorldLine(0, 191, &near_bottom_left, &far_bottom_left);

	VecFx32 far_bottom_right;
	VecFx32 near_bottom_right;
	NNS_G3dScrPosToWorldLine(255, 191, &near_bottom_right, &far_bottom_right);

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

	*bbmin = min;
	*bbmax = max;
}

void GameController::RenderFlare()
{
	if (!mDisplayFlare || mSunX < 0 || mSunX >= 256 || mSunY < 0 || mSunY >= 192 || mFlareAlpha <= 0)
		return;
	//Do some 2d with the 3d engine when needed (AKA, fucking up matrices)
	G3_MtxMode(GX_MTXMODE_PROJECTION);
	{
		G3_Identity();
		G3_OrthoW(FX32_ONE * 0, FX32_ONE * 192, FX32_ONE * 0, FX32_ONE * 256, FX32_ONE * 0, FX32_ONE * 1024, 40960 * 4, NULL);
	}
	G3_MtxMode(GX_MTXMODE_TEXTURE);
	{
		G3_Identity();
	}
	G3_MtxMode(GX_MTXMODE_POSITION_VECTOR);
	G3_Identity();
	G3_Color(GX_RGB(31, 31, 31));	

	//lense flare
	G3_MtxMode(GX_MTXMODE_TEXTURE);
	G3_Scale(2 * FX32_ONE, 2 * FX32_ONE, FX32_ONE);
	G3_MtxMode(GX_MTXMODE_POSITION_VECTOR);

	VecFx32 centerline =
	{
		(128 - mSunX) * FX32_ONE,
		(96 - mSunY) * FX32_ONE,
		0
	};
	fx32 length = VEC_Mag(&centerline);
	VEC_Normalize(&centerline, &centerline);

	texture_t* tex = mMap->mTerrainManager->GetFlareTexture();
	G3_TexImageParam((GXTexFmt)tex->nitroFormat,       // use alpha texture
		GX_TEXGEN_TEXCOORD,    // use texcoord
		(GXTexSizeS)tex->nitroWidth,        // 16 pixels
		(GXTexSizeT)tex->nitroHeight,        // 16 pixels
		GX_TEXREPEAT_ST,     // no repeat
		GX_TEXFLIP_ST,       // no flip
		GX_TEXPLTTCOLOR0_USE,  // use color 0 of the palette
		NNS_GfdGetTexKeyAddr(tex->texKey)     // the offset of the texture image
	);
	G3_TexPlttBase(NNS_GfdGetPlttKeyAddr(tex->plttKey), (GXTexFmt)tex->nitroFormat);

	G3_Color(GX_RGB(mSunColorMatch & 0x1F, ((mSunColorMatch >> 5) & 0x1F) * 24 / 32, ((mSunColorMatch >> 10) & 0x1F) >> 1));

	G3_PolygonAttr(0, GX_POLYGONMODE_MODULATE, GX_CULL_BACK, 58, mFlareAlpha, 0);
	Util_DrawSpriteScaled(FX_Mul(centerline.x, -length / 2) + 128 * FX32_ONE - 16 * FX32_ONE, FX_Mul(centerline.y, -length / 2) + 96 * FX32_ONE - 16 * FX32_ONE, -1 * FX32_ONE, 32 * FX32_ONE, 32 * FX32_ONE, FX32_CONST(0.5 * 2));

	G3_Color(GX_RGB((mSunColorMatch & 0x1F) >> 1, ((mSunColorMatch >> 5) & 0x1F) * 24 / 32, (mSunColorMatch >> 10) & 0x1F));

	G3_PolygonAttr(0, GX_POLYGONMODE_MODULATE, GX_CULL_BACK, 56, mFlareAlpha, 0);
	Util_DrawSpriteScaled(FX_Mul(centerline.x, -length / 4) + 128 * FX32_ONE - 16 * FX32_ONE, FX_Mul(centerline.y, -length / 4) + 96 * FX32_ONE - 16 * FX32_ONE, -1 * FX32_ONE, 32 * FX32_ONE, 32 * FX32_ONE, FX32_CONST(0.25 * 2));
	G3_PolygonAttr(0, GX_POLYGONMODE_MODULATE, GX_CULL_BACK, 55, mFlareAlpha, 0);
	Util_DrawSpriteScaled(FX_Mul(centerline.x, -FX_Div(length, FX32_CONST(5.5))) + 128 * FX32_ONE - 16 * FX32_ONE, FX_Mul(centerline.y, -FX_Div(length, FX32_CONST(5.5))) + 96 * FX32_ONE - 16 * FX32_ONE, -1 * FX32_ONE, 32 * FX32_ONE, 32 * FX32_ONE, FX32_CONST(0.25 * 2));

	G3_Color(GX_RGB((mSunColorMatch & 0x1F) >> 1, (mSunColorMatch >> 5) & 0x1F, ((mSunColorMatch >> 10) & 0x1F) >> 1));
	G3_PolygonAttr(0, GX_POLYGONMODE_MODULATE, GX_CULL_BACK, 60, mFlareAlpha, 0);
	Util_DrawSpriteScaled(FX_Mul(centerline.x, length / 8) + 128 * FX32_ONE - 16 * FX32_ONE, FX_Mul(centerline.y, length / 8) + 96 * FX32_ONE - 16 * FX32_ONE, -1 * FX32_ONE, 32 * FX32_ONE, 32 * FX32_ONE, FX32_CONST(1.5));

	G3_PolygonAttr(0, GX_POLYGONMODE_MODULATE, GX_CULL_BACK, 54, mFlareAlpha, 0);
	Util_DrawSpriteScaled(FX_Mul(centerline.x, length / 3) + 128 * FX32_ONE - 16 * FX32_ONE, FX_Mul(centerline.y, length / 3) + 96 * FX32_ONE - 16 * FX32_ONE, -1 * FX32_ONE, 32 * FX32_ONE, 32 * FX32_ONE, FX32_CONST(0.25 * 2));

	G3_Color(GX_RGB(mSunColorMatch & 0x1F, ((mSunColorMatch >> 5) & 0x1F) * 24 / 32, ((mSunColorMatch >> 10) & 0x1F) >> 1));
	G3_PolygonAttr(0, GX_POLYGONMODE_MODULATE, GX_CULL_BACK, 57, mFlareAlpha, 0);
	Util_DrawSpriteScaled(FX_Mul(centerline.x, length / 2) + 128 * FX32_ONE - 16 * FX32_ONE, FX_Mul(centerline.y, length / 2) + 96 * FX32_ONE - 16 * FX32_ONE, -1 * FX32_ONE, 32 * FX32_ONE, 32 * FX32_ONE, FX32_CONST(0.5 * 2));

	G3_Color(GX_RGB((mSunColorMatch & 0x1F) >> 1, (mSunColorMatch >> 5) & 0x1F, ((mSunColorMatch >> 10) & 0x1F) >> 1));
	G3_PolygonAttr(0, GX_POLYGONMODE_MODULATE, GX_CULL_BACK, 59, mFlareAlpha, 0);
	Util_DrawSpriteScaled(FX_Mul(centerline.x, length) + 128 * FX32_ONE - 16 * FX32_ONE, FX_Mul(centerline.y, length) + 96 * FX32_ONE - 16 * FX32_ONE, -1 * FX32_ONE, 32 * FX32_ONE, 32 * FX32_ONE, FX32_CONST(2.0));

	G3_MtxMode(GX_MTXMODE_TEXTURE);
	G3_Identity();
	G3_MtxMode(GX_MTXMODE_POSITION_VECTOR);
}

void GameController::Render(RenderMode mode)
{
	if (!mCamera)
		return;
	if (mode == RENDER_MODE_PICKING)
	{
		reg_G3X_DISP3DCNT = reg_G3X_DISP3DCNT & ~REG_G3X_DISP3DCNT_TME_MASK;
		G3X_SetClearColor(0, /*31*/0, 0x7fff, 0, false);
		G3X_EdgeMarking(false);
		G3X_AntiAlias(false);
		G3X_SetFog(false, GX_FOGBLEND_COLOR_ALPHA, GX_FOGSLOPE_0x0020, 0);
	}
	else
	{
		reg_G3X_DISP3DCNT = reg_G3X_DISP3DCNT | REG_G3X_DISP3DCNT_TME_MASK;
		if (mode == RENDER_MODE_FAR)
			G3X_SetClearColor(GX_RGB(168 >> 3, 209 >> 3, 255 >> 3), 31, 0x7fff, 0, true);
		else
			G3X_SetClearColor(GX_RGB(168 >> 3, 209 >> 3, 255 >> 3), 31, 0x7fff, 0, false);
		G3X_SetShading(GX_SHADING_HIGHLIGHT);
		G3X_EdgeMarking(true);
		G3X_AntiAlias(true);
		if (mode == RENDER_MODE_FAR)
			G3X_SetFog(true, GX_FOGBLEND_COLOR_ALPHA, GX_FOGSLOPE_0x2000, 0x8000 - 0x2000);
		else
			G3X_SetFog(false, GX_FOGBLEND_ALPHA, GX_FOGSLOPE_0x0200, 0x8000 - 0x200);

		//G3X_SetFog(/*true*/false, GX_FOGBLEND_COLOR_ALPHA, GX_FOGSLOPE_0x0400, 0x8000 - 0x100);
		//G3X_SetFogColor(GX_RGB(119 >> 3, 199 >> 3, 244 >> 3), 25);
		//if (mode == RENDER_MODE_FAR)
			G3X_SetFogColor(mFogColor, 31);
		//else
		//	G3X_SetFogColor(GX_RGB(148 >> 3, 181 >> 3, 206 >> 3), 0);
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

	if (mode == RENDER_MODE_NEAR)
	{
		//Do some 2d with the 3d engine when needed (AKA, fucking up matrices)
		G3_MtxMode(GX_MTXMODE_PROJECTION);
		{
			G3_Identity();
			G3_OrthoW(FX32_ONE * 0, FX32_ONE * 192, FX32_ONE * 0, FX32_ONE * 256, FX32_ONE * 0, FX32_ONE * 1024, 40960 * 4, NULL);
		}
		G3_MtxMode(GX_MTXMODE_TEXTURE);
		{
			G3_Identity();
		}
		G3_MtxMode(GX_MTXMODE_POSITION_VECTOR);
		G3_Identity();
		G3_Color(GX_RGB(31, 31, 31));

		G3_TexImageParam(GX_TEXFMT_DIRECT, GX_TEXGEN_TEXCOORD, GX_TEXSIZE_S256, GX_TEXSIZE_T256, GX_TEXREPEAT_NONE, GX_TEXFLIP_NONE, GX_TEXPLTTCOLOR0_USE, 0x40000);
		G3_PolygonAttr(0, GX_POLYGONMODE_MODULATE, GX_CULL_BACK, 10, 30, 0);
		Util_DrawSprite(0 * FX32_ONE, 0 * FX32_ONE, -1024 * FX32_ONE, 256 * FX32_ONE, 192 * FX32_ONE);
		G3_PolygonAttr(0, GX_POLYGONMODE_MODULATE, GX_CULL_BACK, 8, 30, 0);
		Util_DrawSprite(0 * FX32_ONE, 0 * FX32_ONE, -1024 * FX32_ONE, 256 * FX32_ONE, 192 * FX32_ONE);
	}

	mCamera->Apply();
	VecFx32 camDir;
	mCamera->GetLookDirection(&camDir);
	NNS_G3dGlbPolygonAttr(GX_LIGHTMASK_0, GX_POLYGONMODE_MODULATE, GX_CULL_BACK, 0, 31, GX_POLYGON_ATTR_MISC_FOG | GX_POLYGON_ATTR_MISC_FAR_CLIPPING);
	NNS_G3dGlbLightVector(GX_LIGHTID_0, mLightDirection.x, mLightDirection.y, mLightDirection.z);
	NNS_G3dGlbLightColor(GX_LIGHTID_0, mLightColor);
	if (mode == RENDER_MODE_PICKING)
	{
		NNS_G3dGlbMaterialColorDiffAmb(GX_RGB(0, 0, 0), GX_RGB(0, 0, 0), false);
		NNS_G3dGlbMaterialColorSpecEmi(GX_RGB(0, 0, 0), GX_RGB(0, 0, 0), false);
	}
	else
	{
		NNS_G3dGlbMaterialColorDiffAmb(GX_RGB(31, 31, 31), /*GX_RGB(5, 5, 5)*/GX_RGB(10, 10, 10), false);
		NNS_G3dGlbMaterialColorSpecEmi(/*GX_RGB(3, 3, 3)*/GX_RGB(1, 1, 1), GX_RGB(0, 0, 0), false);
	}

	fx32 newDist = 0;
	if (mode != RENDER_MODE_PICKING)
	{
		VecFx32 up = { 0, FX32_ONE, 0 };
		fx32 camY = mCamera->mPosition.y - mMap->GetYOnMap(mCamera->mPosition.x, mCamera->mPosition.z);
		u16 ang = FX_AcosIdx(VEC_DotProduct(&camDir, &up));
		if (ang < FX_DEG_TO_IDX(90 * FX32_ONE))
			ang = FX_DEG_TO_IDX(90 * FX32_ONE);
		fx32 cosang = FX_CosIdx(ang - FX_DEG_TO_IDX(90 * FX32_ONE));
		cosang = FX_Mul(cosang, cosang);
		//OS_Printf("ang: %d\n", FX_IDX_TO_DEG(ang) >> 12);
		newDist = camY + FX_Mul(cosang, 50 * 4096);
		if (mode == RENDER_MODE_FAR)
		{
			if (newDist < 8 * 4096)
				newDist = 8 * 4096;
			NNS_G3dGlbPerspectiveW(FX32_SIN30, FX32_COS30, (256 * 4096 / 192), 4 * 4096, /*35*//*50 * 4096*/newDist, 40960 * 4);
		}
		else
		{
			if (newDist > 10 * 4096)
				newDist = 10 * 4096;
			if (newDist < 2 * 4096)
				newDist = 2 * 4096;
			NNS_G3dGlbPerspectiveW(FX32_SIN30, FX32_COS30, (256 * 4096 / 192), 4096 >> 3, /*10 * 4096*/newDist, 40960 * 4);
		}
	}
	else
		NNS_G3dGlbPerspectiveW(FX32_SIN30, FX32_COS30, (256 * 4096 / 192), 4096 >> 3, 35 * 4096, 40960 * 4);

	VecFx32 bbmin, bbmax;
	calculateVisibleGrid(&bbmin, &bbmax);

	int xstart = (bbmin.x - 2 * FX32_ONE - FX32_HALF) / FX32_ONE + 32;
	int zstart = (bbmin.z - 2 * FX32_ONE - FX32_HALF) / FX32_ONE + 32;

	int xend = (bbmax.x + 2 * FX32_ONE + FX32_HALF) / FX32_ONE + 32;
	int zend = (bbmax.z + 2 * FX32_ONE + FX32_HALF) / FX32_ONE + 32;

	int xstart2 = xstart;
	int xend2 = xend;
	int zstart2 = zstart;
	int zend2 = zend;

	if (mode == RENDER_MODE_FAR)
	{
		if (newDist > 17 * FX32_ONE)
		{
			NNS_G3dGlbPerspectiveW(FX32_SIN30, FX32_COS30, (256 * 4096 / 192), 14 * 4096, 17 * FX32_ONE, 40960 * 4);
			VecFx32 bbmin2, bbmax2;
			calculateVisibleGrid(&bbmin2, &bbmax2);
			xstart2 = (bbmin2.x - 2 * FX32_ONE - FX32_HALF) / FX32_ONE + 32;
			zstart2 = (bbmin2.z - 2 * FX32_ONE - FX32_HALF) / FX32_ONE + 32;

			xend2 = (bbmax2.x + 2 * FX32_ONE + FX32_HALF) / FX32_ONE + 32;
			zend2 = (bbmax2.z + 2 * FX32_ONE + FX32_HALF) / FX32_ONE + 32;
		}
		NNS_G3dGlbPerspectiveW(FX32_SIN30, FX32_COS30, (256 * 4096 / 192), 4 * 4096, 50 * FX32_ONE, 40960 * 4);
	}

	NNS_G3dGlbFlushP();
	NNS_G3dGeFlushBuffer();

	if (mode == RENDER_MODE_PICKING)
	{
		mPickingXStart = xstart;
		mPickingXEnd = xend;
		mPickingZStart = zstart;
	}

	G3_PushMtx();
	{
		mMap->Render(xstart, xend, zstart, zend, xstart2, xend2, zstart2, zend2, mode == RENDER_MODE_PICKING, &mCamera->mPosition, &camDir, (mode == RENDER_MODE_FAR ? 1 : 0));
		G3_PushMtx();
		{
			G3_Translate(-32 * FX32_ONE, 0, -32 * FX32_ONE);
			mWagon->Render();
		}
		G3_PopMtx(1);
		//skybox
		if (mode == RENDER_MODE_FAR)
		{
			if (gKeys & PAD_BUTTON_X)
				mTOTDController->Update();

			NNS_G3dGlbPerspectiveW(FX32_SIN30, FX32_COS30, (256 * 4096 / 192), 8 * FX32_ONE, 56 * FX32_ONE, 40960 * 4);
			NNS_G3dGlbFlushP();
			NNS_G3dGeFlushBuffer();
			G3_PolygonAttr(0, GX_POLYGONMODE_MODULATE, GX_CULL_BACK, 62, 30, 0);
			G3_TexImageParam(GX_TEXFMT_NONE, GX_TEXGEN_NONE, GX_TEXSIZE_S8, GX_TEXSIZE_T8, GX_TEXREPEAT_NONE, GX_TEXFLIP_NONE, GX_TEXPLTTCOLOR0_USE, 0);
			G3_PushMtx();
			{
				G3_Translate(mCamera->mDestination.x, 0, mCamera->mDestination.z);
				G3_PushMtx();
				{
					G3_Scale(50 * FX32_ONE, 50 * FX32_ONE, 50 * FX32_ONE);
					mHemisphere->Render();
				}
				G3_PopMtx(1);
				//sun
				G3_Translate(mSunPosition.x, mSunPosition.y, mSunPosition.z);

				Util_SetupBillboardMatrix();
				G3_Translate(0, -4 * FX32_ONE, 2 * FX32_ONE);

				VecFx32 sunDir;
				VEC_Normalize(&mSunPosition, &sunDir);
				fx32 alpha = VEC_DotProduct(&sunDir, &camDir);
				if (alpha < 0)
				{
					mSunX = -1;
					mSunY = -1;
					mFlareAlpha = 0;
				}
				else
				{
					mFlareAlpha = ((FX_Mul(alpha, alpha) - FX32_CONST(0.3)) * 31) >> 12;
					if (mFlareAlpha < 0)
						mFlareAlpha = 0;
					NNS_G3dLocalOriginToScrPos(&mSunX, &mSunY);
				}

				G3_Scale(6 * FX32_ONE, 6 * FX32_ONE, 6 * FX32_ONE);

				int r = (mLightColor & 0x1F) * 2;
				if (r > 31)
					r = 31;
				int g = ((mLightColor >> 5) & 0x1F) * 2;
				if (g > 31)
					g = 31;
				int b = ((mLightColor >> 10) & 0x1F) * 2;
				if (b > 31)
					b = 31;

				mSunColorMatch = GX_RGB(r, g, b);

				G3_Color(GX_RGB(r, g, b));

				texture_t* tex = mMap->mTerrainManager->GetSunTexture();
				G3_TexImageParam((GXTexFmt)tex->nitroFormat,       // use alpha texture
					GX_TEXGEN_TEXCOORD,    // use texcoord
					(GXTexSizeS)tex->nitroWidth,        // 16 pixels
					(GXTexSizeT)tex->nitroHeight,        // 16 pixels
					GX_TEXREPEAT_NONE,     // no repeat
					GX_TEXFLIP_NONE,       // no flip
					GX_TEXPLTTCOLOR0_USE,  // use color 0 of the palette
					NNS_GfdGetTexKeyAddr(tex->texKey)     // the offset of the texture image
				);
				G3_TexPlttBase(NNS_GfdGetPlttKeyAddr(tex->plttKey), (GXTexFmt)tex->nitroFormat);

				G3_PolygonAttr(0, GX_POLYGONMODE_MODULATE, GX_CULL_BACK, 0, 31, 0);
				G3_Begin(GX_BEGIN_QUADS);
				{
					G3_TexCoord(0, 0);
					G3_Vtx(-FX32_HALF, FX32_HALF, 0);
					G3_TexCoord(0, 32 * FX32_ONE);
					G3_Vtx(-FX32_HALF, -FX32_HALF, 0);
					G3_TexCoord(32 * FX32_ONE, 32 * FX32_ONE);
					G3_Vtx(FX32_HALF, -FX32_HALF, 0);
					G3_TexCoord(32 * FX32_ONE, 0);
					G3_Vtx(FX32_HALF, FX32_HALF, 0);
				}
				G3_End();
			}
			G3_PopMtx(1);
		}
	}
	G3_PopMtx(1);
	if (mode == RENDER_MODE_NEAR)
		RenderFlare();
	mMap->UpdateResourceCounter();
}