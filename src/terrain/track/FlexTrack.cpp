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

void FlexTrack::RenderMarkers(TerrainManager* terrainManager)
{
	texture_t* tex = terrainManager->GetTrackMarkerTexture();
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
	G3_PolygonAttr(GX_LIGHTMASK_0, GX_POLYGONMODE_MODULATE, GX_CULL_BACK, 60, 31, GX_POLYGON_ATTR_MISC_FOG | GX_POLYGON_ATTR_MISC_FAR_CLIPPING);
	G3_PushMtx();
	{
		G3_Translate(32 * FX32_ONE, 0, 32 * FX32_ONE);
		G3_MtxMode(GX_MTXMODE_TEXTURE);
		G3_Translate((8 << tex->nitroWidth) * FX32_HALF, (8 << tex->nitroHeight) * FX32_HALF, 0);
		G3_RotZ(FX_SinIdx(FX_DEG_TO_IDX(terrainManager->mTrackMarkerRotation)), FX_CosIdx(FX_DEG_TO_IDX(terrainManager->mTrackMarkerRotation)));
		G3_Translate(-(8 << tex->nitroWidth) * FX32_HALF, -(8 << tex->nitroHeight) * FX32_HALF, 0);
		{
			MtxFx44 mtx;
			MTX_Identity44(&mtx);
			mtx._22 = 16 * FX32_ONE;
			mtx._33 = 16 * FX32_ONE;
			G3_MultMtx44(&mtx);
		}
		G3_MtxMode(GX_MTXMODE_POSITION_VECTOR);

		G3_Begin(GX_BEGIN_QUADS);
		{
			G3_Color(GX_RGB(31, 31, 31));
			for (int i = 0; i < 2; i++)
			{
				G3_PushMtx();
				{
					fx32 y = mMap->GetYOnMap(mPoints[i].x, mPoints[i].z);
					G3_Translate(mPoints[i].x, y + FX32_ONE / 32 + FX32_ONE / 64, mPoints[i].z);
					//G3_TexCoord(0, 0);
					//G3_Vtx(-normal.x >> 1, 0, -normal.z >> 1);
					//G3_TexCoord((8 << tex->nitroWidth) * FX32_ONE, (8 << tex->nitroHeight) * dist);
					//G3_Vtx(normal.x >> 1, 0, normal.z >> 1);
					G3_TexCoord(0, 0);
					G3_Vtx(-FLEXTRACK_TRACK_WIDTH >> 1, 0, -FLEXTRACK_TRACK_WIDTH >> 1);
					G3_TexCoord(0, (8 << tex->nitroHeight) * FX32_ONE);
					G3_Vtx(-FLEXTRACK_TRACK_WIDTH >> 1, 0, FLEXTRACK_TRACK_WIDTH >> 1);
					G3_TexCoord((8 << tex->nitroWidth) * FX32_ONE, (8 << tex->nitroHeight) * FX32_ONE);
					G3_Vtx(FLEXTRACK_TRACK_WIDTH >> 1, 0, FLEXTRACK_TRACK_WIDTH >> 1);
					G3_TexCoord((8 << tex->nitroWidth) * FX32_ONE, 0);
					G3_Vtx(FLEXTRACK_TRACK_WIDTH >> 1, 0, -FLEXTRACK_TRACK_WIDTH >> 1);
				}
				G3_PopMtx(1);
			}
		}
		G3_End();

		G3_MtxMode(GX_MTXMODE_TEXTURE);
		G3_Identity();
		G3_MtxMode(GX_MTXMODE_POSITION_VECTOR);
	}
	G3_PopMtx(1);
	G3_PolygonAttr(GX_LIGHTMASK_0, GX_POLYGONMODE_MODULATE, GX_CULL_BACK, 0, 31, GX_POLYGON_ATTR_MISC_FOG | GX_POLYGON_ATTR_MISC_FAR_CLIPPING);
}

fx32 FlexTrack::GetTrackLength(int inPoint)
{
	//todo
	//VecFx32 diff;
	//VEC_Subtract(&mPoints[0], &mPoints[1], &diff);
	//return VEC_Mag(&diff);
	/*VecFx32 a = mPoints[0];
	if (mConnections[0] != NULL)
		mConnections[0]->GetConnectionPoint(mConnections[0]->GetOutPointId(mConnectionInPoints[0]), &a);
	VecFx32 b = mPoints[1];
	if (mConnections[1] != NULL)
		mConnections[1]->GetConnectionPoint(mConnections[1]->GetOutPointId(mConnectionInPoints[1]), &b);

	VecFx32 prev;
	fx32 len = 0;
	for (int i = 0; i < FLEXTRACK_NR_POINTS; i++)
	{
		VecFx32 point;
		cubicInterpolate(&a, &mPoints[0], &mPoints[1], &b, i * FX32_ONE / (FLEXTRACK_NR_POINTS - 1), &point);
		point.y = map->GetYOnMap(point.x, point.z);
		if (i != 0)
			len += VEC_Distance(&point, &prev);
		prev = point;
	}
	return len;*/
	return mCurveLength;
}

void FlexTrack::CalculatePoint(int inPoint, fx32 progress, VecFx32* pPos, VecFx32* pDir)
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

	pPos->y = mMap->GetYOnMap(pPos->x, pPos->z);

	pPos->x += 32 * FX32_ONE;
	pPos->z += 32 * FX32_ONE;

	//pDir->x = 0;
	//pDir->y = 0;
	//pDir->z = FX32_ONE;
}

void FlexTrack::Invalidate()
{
	VecFx32 a = mPoints[0];
	if (mConnections[0] != NULL)
		mConnections[0]->GetConnectionPoint(mConnections[0]->GetOutPointId(mConnectionInPoints[0]), &a);
	VecFx32 b = mPoints[1];
	if (mConnections[1] != NULL)
		mConnections[1]->GetConnectionPoint(mConnections[1]->GetOutPointId(mConnectionInPoints[1]), &b);

	fx32 len = 0;
	for (int i = 0; i < FLEXTRACK_NR_POINTS; i++)
	{
		cubicInterpolate(&a, &mPoints[0], &mPoints[1], &b, i * FX32_ONE / (FLEXTRACK_NR_POINTS - 1), &mCurvePoints[i]);
		mCurvePoints[i].y = mMap->GetYOnMap(mCurvePoints[i].x, mCurvePoints[i].z);
		VecFx32 dir;
		cubicInterpolateDir(&a, &mPoints[0], &mPoints[1], &b, i * FX32_ONE / (FLEXTRACK_NR_POINTS - 1), &dir);
		dir.y = 0;
		VEC_Normalize(&dir, &dir);
		mCurveNormals[i].x = -dir.z;
		mCurveNormals[i].y = 0;
		mCurveNormals[i].z = dir.x;
		if (i != 0)
			len += VEC_Distance(&mCurvePoints[i], &mCurvePoints[i - 1]);
	}
	mCurveLength = len;
}