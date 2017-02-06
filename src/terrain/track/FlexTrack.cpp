#include <nitro.h>
#include <nnsys/gfd.h>
#include "core.h"
#include "util.h"
#include "terrain/TerrainManager.h"
#include "terrain/Map.h"
#include "TrackPieceEx.h"
#include "FlexTrack.h"

static void interpolateBezierXZ(VecFx32* a, VecFx32* b, VecFx32* c, VecFx32* d, fx32 t, VecFx32* dst)
{
	fx32 invt = FX32_ONE - t;
	fx32 invt_squared = FX_Mul(invt, invt);
	fx32 invt_cubed = FX_Mul(invt_squared, invt);
	fx32 t_squared = FX_Mul(t, t);
	fx32 t_cubed = FX_Mul(t_squared, t);
	fx32 x =
		FX_Mul(a->x, invt_cubed) +
		FX_Mul(b->x, FX_Mul(3 * t, invt_squared)) +
		FX_Mul(c->x, FX_Mul(3 * t_squared, invt)) +
		FX_Mul(d->x, t_cubed);
	fx32 z =
		FX_Mul(a->z, invt_cubed) +
		FX_Mul(b->z, FX_Mul(3 * t, invt_squared)) +
		FX_Mul(c->z, FX_Mul(3 * t_squared, invt)) +
		FX_Mul(d->z, t_cubed);
	dst->x = x;
	dst->z = z;
}

static void cubicHermiteInterpolate(VecFx32* a, VecFx32* b, VecFx32* c, VecFx32* d, fx32 t, VecFx32* dst)
{
	fx32 coef1 = FX_Mul(FX_Mul(FX32_ONE - t, FX32_ONE - t), FX32_ONE - t);
	fx32 coef2 = (3 * FX_Mul(FX_Mul(t, t), t) -	6 * FX_Mul(t, t) + FX32_ONE * 4);
	fx32 coef3 = (3 * (t + FX_Mul(t, t) - FX_Mul(FX_Mul(t, t), t)) + FX32_ONE);
	fx32 coef4 = FX_Mul(FX_Mul(t, t), t);

	fx32 x =
		(FX_Mul(a->x, coef1) +
			FX_Mul(b->x, coef2) +
			FX_Mul(c->x, coef3) +
			FX_Mul(d->x, coef4)) / 6;
	fx32 y =
		(FX_Mul(a->y, coef1) +
			FX_Mul(b->y, coef2) +
			FX_Mul(c->y, coef3) +
			FX_Mul(d->y, coef4)) / 6;
	fx32 z =
		(FX_Mul(a->z, coef1) +
			FX_Mul(b->z, coef2) +
			FX_Mul(c->z, coef3) +
			FX_Mul(d->z, coef4)) / 6;

	dst->x = x;
	dst->y = y;
	dst->z = z;
}

static void cubicHermiteInterpolateDir(VecFx32* a, VecFx32* b, VecFx32* c, VecFx32* d, fx32 t, VecFx32* dst)
{
	fx32 coef1 = -3 * FX_Mul(t - FX32_ONE, t - FX32_ONE);
	fx32 coef2 = -(12 * t - 9 * FX_Mul(t, t));
	fx32 coef3 = 6 * t - 9 * FX_Mul(t, t) + 3 * FX32_ONE;
	fx32 coef4 = 3 * FX_Mul(t, t);

	fx32 x =
		(FX_Mul(a->x, coef1) +
			FX_Mul(b->x, coef2) +
			FX_Mul(c->x, coef3) +
			FX_Mul(d->x, coef4)) / 6;
	fx32 y =
		(FX_Mul(a->y, coef1) +
			FX_Mul(b->y, coef2) +
			FX_Mul(c->y, coef3) +
			FX_Mul(d->y, coef4)) / 6;
	fx32 z =
		(FX_Mul(a->z, coef1) +
			FX_Mul(b->z, coef2) +
			FX_Mul(c->z, coef3) +
			FX_Mul(d->z, coef4)) / 6;

	dst->x = x;
	dst->y = y;
	dst->z = z;
}

static fx32 cubicInterpolate1D(fx32 a, fx32 b, fx32 c, fx32 d, fx32 t)
{
	fx32 a0, a1, a2, a3, t2;

	t2 = FX_Mul(t, t);
	//a0 = d - c - a + b;
	//a1 = a - b - a0;
	//a2 = c - a;
	//a3 = b;
	a0 = -3 * a + 9 * b - 9 * c + 3 * d;
	a1 = 6 * a - 15*b + 12 * c - 3*d;
	a2 = -3*a + 3*c;
	a3 = 6 * b;

	return (FX_Mul(a0, FX_Mul(t, t2)) + FX_Mul(a1, t2) + FX_Mul(a2, t) + a3) / 6;
}

static fx32 cubicInterpolate1DDir(fx32 a, fx32 b, fx32 c, fx32 d, fx32 t)
{
	return c / 2 - a / 2 + 2 * FX_Mul(t, a - (5 * b) / 2 + 2 * c - d / 2) - 3 * FX_Mul(FX_Mul(t, t), a / 2 - (3 * b) / 2 + (3 * c) / 2 - d / 2);
	//return c - a + 2 * FX_Mul(t, 2 * a - 2 * b + c - d) - 3 * FX_Mul(FX_Mul(t, t), a - b + c - d);
}

static void cubicInterpolate(VecFx32* a, VecFx32* b, VecFx32* c, VecFx32* d, fx32 t, VecFx32* dst)
{
	dst->x = cubicInterpolate1D(a->x, b->x, c->x, d->x, t);
	dst->y = cubicInterpolate1D(a->y, b->y, c->y, d->y, t);
	dst->z = cubicInterpolate1D(a->z, b->z, c->z, d->z, t);
}

static void cubicInterpolateDir(VecFx32* a, VecFx32* b, VecFx32* c, VecFx32* d, fx32 t, VecFx32* dst)
{
	dst->x = cubicInterpolate1DDir(a->x, b->x, c->x, d->x, t);
	dst->y = cubicInterpolate1DDir(a->y, b->y, c->y, d->y, t);
	dst->z = cubicInterpolate1DDir(a->z, b->z, c->z, d->z, t);
}

void FlexTrack::Render(Map* map, TerrainManager* terrainManager)
{
	texture_t* tex = terrainManager->GetTrackTexture();
	G3_TexImageParam((GXTexFmt)tex->nitroFormat,       // use alpha texture
		GX_TEXGEN_TEXCOORD,    // use texcoord
		(GXTexSizeS)tex->nitroWidth,        // 16 pixels
		(GXTexSizeT)tex->nitroHeight,        // 16 pixels
		GX_TEXREPEAT_ST,     // no repeat
		GX_TEXFLIP_NONE,       // no flip
		GX_TEXPLTTCOLOR0_USE,  // use color 0 of the palette
		NNS_GfdGetTexKeyAddr(tex->texKey)     // the offset of the texture image
	);
	G3_TexPlttBase(NNS_GfdGetPlttKeyAddr(tex->plttKey), (GXTexFmt)tex->nitroFormat);
#define NR_POINTS	6
	VecFx32 points[NR_POINTS];
	VecFx32 normals[NR_POINTS];
	for (int i = 0; i < NR_POINTS; i++)
	{
		VecFx32 a = mPoints[0];
		if (mConnections[0] != NULL)
			mConnections[0]->GetConnectionPoint(mConnections[0]->GetOutPointId(mConnectionInPoints[0]), &a);
		VecFx32 b = mPoints[1];
		if (mConnections[1] != NULL)
			mConnections[1]->GetConnectionPoint(mConnections[1]->GetOutPointId(mConnectionInPoints[1]), &b);
		/*if (i == 0 && mConnections[0] == NULL)
		{
			points[i] = mPoints[0];
		}
		else if (i == (NR_POINTS - 1) && mConnections[1] == NULL)
		{
			points[i] = mPoints[1];
		}
		else
		{*/
			cubicInterpolate(&a, &mPoints[0], &mPoints[1], &b, i * FX32_ONE / (NR_POINTS - 1), &points[i]);
		//}
		points[i].y = map->GetYOnMap(points[i].x, points[i].z);
		VecFx32 dir;
		cubicInterpolateDir(&a, &mPoints[0], &mPoints[1], &b, i * FX32_ONE / (NR_POINTS - 1), &dir);
		dir.y = 0;
		VEC_Normalize(&dir, &dir);
		normals[i].x = -dir.z;
		normals[i].y = 0;
		normals[i].z = dir.x;
	}
	G3_PushMtx();
	{
		G3_Translate(32 * FX32_ONE, 0, 32 * FX32_ONE);
		G3_Begin(GX_BEGIN_QUAD_STRIP);
		{
			G3_Normal(0, GX_FX16_FX10_MAX, 0);
			fx32 dist = 0;
			for (int i = 0; i < NR_POINTS; i++)
			{
				VecFx32 normal = normals[i];
				G3_PushMtx();
				{
					G3_Translate(points[i].x, points[i].y + (FX32_ONE / 16), points[i].z);
					G3_TexCoord(0, (8 << tex->nitroHeight) * dist);
					G3_Vtx(-normal.x >> 1, 0, -normal.z >> 1);
					G3_TexCoord((8 << tex->nitroWidth) * FX32_ONE, (8 << tex->nitroHeight) * dist);
					G3_Vtx(normal.x >> 1, 0, normal.z >> 1);
				}
				G3_PopMtx(1);
				if(i != (NR_POINTS - 1))
					dist += VEC_Distance(&points[i], &points[i + 1]);
			}
		}
		G3_End();
	}
	G3_PopMtx(1);
}

fx32 FlexTrack::GetTrackLength(int inPoint)
{
	//todo
	VecFx32 diff;
	VEC_Subtract(&mPoints[0], &mPoints[1], &diff);
	return VEC_Mag(&diff);
}

void FlexTrack::CalculatePoint(int inPoint, fx32 progress, VecFx32* pPos, VecFx32* pDir, Map* map)
{
	if (inPoint == 1)
		progress = FX32_ONE - progress;

	VecFx32 a = mPoints[0];
	if (mConnections[0] != NULL)
		mConnections[0]->GetConnectionPoint(mConnections[0]->GetOutPointId(mConnectionInPoints[0]), &a);
	VecFx32 b = mPoints[1];
	if (mConnections[1] != NULL)
		mConnections[1]->GetConnectionPoint(mConnections[1]->GetOutPointId(mConnectionInPoints[1]), &b);
	cubicInterpolate(&a, &mPoints[0], &mPoints[1], &b, progress, pPos);
	cubicInterpolateDir(&a, &mPoints[0], &mPoints[1], &b, progress, pDir);

	/*if (inPoint == 1)
	{
		MtxFx33 rot;
		MTX_RotY33(&rot, FX32_SIN180, FX32_COS180);
		MTX_MultVec33(pDir, &rot, pDir);
	}*/
	if (inPoint == 1)
	{
		pDir->x = -pDir->x;
		pDir->z = -pDir->z;
	}

	//pDir->y = 0;
	VEC_Normalize(pDir, pDir);

	pPos->y = map->GetYOnMap(pPos->x, pPos->z);

	pPos->x += 32 * FX32_ONE;
	pPos->z += 32 * FX32_ONE;

	//pDir->x = 0;
	//pDir->y = 0;
	//pDir->z = FX32_ONE;
}