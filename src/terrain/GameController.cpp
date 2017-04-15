#include <nitro.h>
#include <nnsys/fnd.h>
#include <nnsys/g3d.h>
#include <math.h>
#include <cmath>
#include "core.h"
#include "util.h"
#include "GameController.h"

static const GXRgb sToonTable[32] =
{
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0,
	GX_RGB(31,31,31),
	GX_RGB(4, 4, 4)
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

void GameController::Render(RenderMode mode)
{
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
			G3X_SetClearColor(GX_RGB(168 >> 3, 209 >> 3, 255 >> 3), /*31*/0, 0x7fff, 0, false);
		G3X_SetShading(GX_SHADING_HIGHLIGHT);
		G3X_EdgeMarking(true);
		G3X_AntiAlias(true);
		if (mode == RENDER_MODE_FAR)
			G3X_SetFog(true, GX_FOGBLEND_COLOR_ALPHA, GX_FOGSLOPE_0x2000, 0x8000 - 0x2000);
		else
			G3X_SetFog(false, GX_FOGBLEND_ALPHA, GX_FOGSLOPE_0x0800, 0x8000 - 0xC00);
		//G3X_SetFog(/*true*/false, GX_FOGBLEND_COLOR_ALPHA, GX_FOGSLOPE_0x0400, 0x8000 - 0x100);
		//G3X_SetFogColor(GX_RGB(119 >> 3, 199 >> 3, 244 >> 3), 25);
		G3X_SetFogColor(GX_RGB(148 >> 3, 181 >> 3, 206 >> 3), 31);
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

	mCamera->Apply();
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

	if (mode != RENDER_MODE_PICKING)
	{
		if (mode == RENDER_MODE_FAR)
		{
			NNS_G3dGlbPerspectiveW(FX32_SIN30, FX32_COS30, (256 * 4096 / 192), 8 * 4096, /*35*/50 * 4096, 40960 * 4);
		}
		else
		{
			NNS_G3dGlbPerspectiveW(FX32_SIN30, FX32_COS30, (256 * 4096 / 192), 4096 >> 3, 10 * 4096, 40960 * 4);
		}
	}
	else
		NNS_G3dGlbPerspectiveW(FX32_SIN30, FX32_COS30, (256 * 4096 / 192), 4096 >> 3, 35 * 4096, 40960 * 4);

	VecFx32 bbmin, bbmax;
	calculateVisibleGrid(&bbmin, &bbmax);

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

	if (mode == RENDER_MODE_PICKING)
	{
		mPickingXStart = xstart;
		mPickingXEnd = xend;
		mPickingZStart = zstart;
	}

	G3_PushMtx();
	{
		VecFx32 camDir;
		mCamera->GetLookDirection(&camDir);
		mMap->Render(xstart, xend, zstart, zend, mode == RENDER_MODE_PICKING, &mCamera->mPosition, &camDir, (mode == RENDER_MODE_FAR ? 1 : 0));
		mWagon->Render();
		//skybox
		if (mode == RENDER_MODE_FAR)
		{
			NNS_G3dGlbPerspectiveW(FX32_SIN30, FX32_COS30, (256 * 4096 / 192), 8 * FX32_ONE, 64 * FX32_ONE, 40960 * 4);
			NNS_G3dGlbFlushP();
			NNS_G3dGeFlushBuffer();
			G3_PolygonAttr(0, GX_POLYGONMODE_MODULATE, GX_CULL_BACK, 0, 31, 0);
			G3_TexImageParam(GX_TEXFMT_NONE, GX_TEXGEN_NONE, GX_TEXSIZE_S8, GX_TEXSIZE_T8, GX_TEXREPEAT_NONE, GX_TEXFLIP_NONE, GX_TEXPLTTCOLOR0_USE, 0);
			G3_PushMtx();
			//G3_Identity();
			//VecFx32 dst;
			//mWagon->GetPosition(&dst);
			//G3_Translate(dst.x, dst.y, dst.z);
			G3_Translate(mCamera->mDestination.x, 0, mCamera->mDestination.z);
			G3_Scale(50 * FX32_ONE, 50 * FX32_ONE, 50 * FX32_ONE);
			mHemisphere->Render();
			G3_PopMtx(1);
		}
	}
	G3_PopMtx(1);
}