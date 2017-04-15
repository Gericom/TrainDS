#include <nitro.h>
#include <nnsys/g3d.h>
#include "core.h"
#include "Hemisphere.h"

static const VecFx32 SkyColor1 = { FX32_CONST(0.37), FX32_CONST(0.52), FX32_CONST(0.73) };
//#define SkyExponent1 8.5f

static const VecFx32 SkyColor2 = { FX32_CONST(0.89), FX32_CONST(0.96), FX32_CONST(1) };

static const VecFx32 SkyColor3 = { FX32_CONST(0.89), FX32_CONST(0.89), FX32_CONST(0.89) };

//#define SkyExponent2 3.0f

/*#define SkyIntensity FX32_ONE

static const VecFx32 SunColor = { FX32_CONST(1), FX32_CONST(0.99), FX32_CONST(0.87) };
#define SunIntensity (2 * FX32_ONE)

#define SunAlpha (550 * FX32_ONE)
#define SunBeta (1 * FX32_ONE)

static const VecFx32 SunVector = { FX32_CONST(0.269), FX32_CONST(0.615), FX32_CONST(0.740) };

#define SunAzimuth (20 * FX32_ONE)
#define SunAltitude (38 * FX32_ONE)*/

static GXRgb interpolateColor(fx16 y)
{
	//if (y < 0)
	//	y = 0;
	//y = FX_Sqrt(y);
	//return GX_RGB(28 + (((11 - 28) * y) >> 12), 30 + (((16 - 30) * y) >> 12), 31 + (((23 - 31) * y) >> 12));

	fx32 val = MATH_MIN(FX32_ONE, FX32_ONE - y);
	fx32 p1 = (FX32_ONE - FX_Mul(FX_Sqrt(val), /*FX_Mul(val, */FX_Mul(val, FX_Mul(val, FX_Mul(val, FX_Mul(val, FX_Mul(val, FX_Mul(val, val))))))))/*)*/ * 31;//FX_F32_TO_FX32(powf(FX_FX32_TO_F32(MATH_MIN(FX32_ONE, FX32_ONE - y)), SkyExponent1));
	val = MATH_MIN(FX32_ONE, FX32_ONE + y);
	fx32 p3 = (FX32_ONE - FX_Mul(val, FX_Mul(val, val))) * 31;//FX_F32_TO_FX32(powf(FX_FX32_TO_F32(MATH_MIN(FX32_ONE, FX32_ONE + y)), SkyExponent2));
	fx32 p2 = FX32_ONE * 31 - p1 - p3;

	VecFx32 result =
	{
		FX_Mul(SkyColor1.x, p1) + FX_Mul(SkyColor2.x, p2) + FX_Mul(SkyColor3.x, p3),
		FX_Mul(SkyColor1.y, p1) + FX_Mul(SkyColor2.y, p2) + FX_Mul(SkyColor3.y, p3),
		FX_Mul(SkyColor1.z, p1) + FX_Mul(SkyColor2.z, p2) + FX_Mul(SkyColor3.z, p3),
	};
	return GX_RGB(result.x >> 12, result.y >> 12, result.z >> 12);
}

Hemisphere::Hemisphere()
{
	const fx32 R = FX_Inv((HEMISPHERE_NR_RINGS - 1) * FX32_ONE);
	const fx32 S = FX_Inv((HEMISPHERE_NR_SECTORS - 1) * FX32_ONE);
	VecFx16* v = &mVtx[0];
	GXRgb* c = &mColors[0];
	for (int r = 0; r < HEMISPHERE_NR_RINGS; r++)
	{
		for (int s = 0; s < HEMISPHERE_NR_SECTORS; s++)
		{
			fx16 y = FX_SinIdx(FX_DEG_TO_IDX(-90 * FX32_ONE + 180 * r * R));
			fx16 x = FX_Mul(FX_CosIdx(FX_DEG_TO_IDX(2 * 180 * s * S)), FX_SinIdx(FX_DEG_TO_IDX(180 * r * R)));
			fx16 z = FX_Mul(FX_SinIdx(FX_DEG_TO_IDX(2 * 180 * s * S)), FX_SinIdx(FX_DEG_TO_IDX(180 * r * R)));
			v->x = x;
			v->y = y;
			v->z = z;
			*c++ = interpolateColor(y);
			v++;
		}
	}
}

void Hemisphere::Render()
{
	G3_Begin(GX_BEGIN_QUADS);
	for (int r = 0; r < HEMISPHERE_NR_RINGS - 1; r++)
	{
		for (int s = 0; s < HEMISPHERE_NR_SECTORS; s++)
		{
			G3_Color(mColors[r * HEMISPHERE_NR_SECTORS + s]);
			reg_G3_VTX_16 = GX_FX16PAIR(mVtx[r * HEMISPHERE_NR_SECTORS + s].x, mVtx[r * HEMISPHERE_NR_SECTORS + s].y);
			reg_G3_VTX_16 = mVtx[r * HEMISPHERE_NR_SECTORS + s].z;
			//G3_Vtx(mVtx[r * HEMISPHERE_NR_SECTORS + s].x, mVtx[r * HEMISPHERE_NR_SECTORS + s].y, mVtx[r * HEMISPHERE_NR_SECTORS + s].z);
			G3_Color(mColors[r * HEMISPHERE_NR_SECTORS + (s + 1)]);
			reg_G3_VTX_16 = GX_FX16PAIR(mVtx[r * HEMISPHERE_NR_SECTORS + (s + 1)].x, mVtx[r * HEMISPHERE_NR_SECTORS + (s + 1)].y);
			reg_G3_VTX_16 = mVtx[r * HEMISPHERE_NR_SECTORS + (s + 1)].z;
			//G3_Vtx(mVtx[r * HEMISPHERE_NR_SECTORS + (s + 1)].x, mVtx[r * HEMISPHERE_NR_SECTORS + (s + 1)].y, mVtx[r * HEMISPHERE_NR_SECTORS + (s + 1)].z);
			G3_Color(mColors[(r + 1) * HEMISPHERE_NR_SECTORS + (s + 1)]);
			reg_G3_VTX_16 = GX_FX16PAIR(mVtx[(r + 1) * HEMISPHERE_NR_SECTORS + (s + 1)].x, mVtx[(r + 1) * HEMISPHERE_NR_SECTORS + (s + 1)].y);
			reg_G3_VTX_16 = mVtx[(r + 1) * HEMISPHERE_NR_SECTORS + (s + 1)].z;
			//G3_Vtx(mVtx[(r + 1) * HEMISPHERE_NR_SECTORS + (s + 1)].x, mVtx[(r + 1) * HEMISPHERE_NR_SECTORS + (s + 1)].y, mVtx[(r + 1) * HEMISPHERE_NR_SECTORS + (s + 1)].z);
			G3_Color(mColors[(r + 1) * HEMISPHERE_NR_SECTORS + s]);
			reg_G3_VTX_16 = GX_FX16PAIR(mVtx[(r + 1) * HEMISPHERE_NR_SECTORS + s].x, mVtx[(r + 1) * HEMISPHERE_NR_SECTORS + s].y);
			reg_G3_VTX_16 = mVtx[(r + 1) * HEMISPHERE_NR_SECTORS + s].z;
			//G3_Vtx(mVtx[(r + 1) * HEMISPHERE_NR_SECTORS + s].x, mVtx[(r + 1) * HEMISPHERE_NR_SECTORS + s].y, mVtx[(r + 1) * HEMISPHERE_NR_SECTORS + s].z);
		}
	}
	G3_End();
}