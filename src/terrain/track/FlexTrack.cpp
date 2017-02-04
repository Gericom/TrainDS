#include <nitro.h>
#include <nnsys/gfd.h>
#include "core.h"
#include "util.h"
#include "terrain/TerrainManager.h"
#include "TrackPiece.h"
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

void FlexTrack::Render(TerrainManager* terrainManager)
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

	VecFx32 posdiff = 
	{
		(mEndPosition.x - mPosition.x),// * FX32_ONE,
		0,
		(mEndPosition.z - mPosition.z)// * FX32_ONE
	};
	VEC_Normalize(&posdiff, &posdiff);
	/*if(posdiff.x < 0)
		posdiff.x = (posdiff.x - FX32_HALF) / FX32_ONE * FX32_ONE;
	else
		posdiff.x = (posdiff.x + FX32_HALF) / FX32_ONE * FX32_ONE;
	if (posdiff.z < 0)
		posdiff.z = (posdiff.z - FX32_HALF) / FX32_ONE * FX32_ONE;
	else
		posdiff.z = (posdiff.z + FX32_HALF) / FX32_ONE * FX32_ONE;

	NOCASH_Printf("%d, %d\n", posdiff.x, posdiff.z);*/

	//divide the curve in 5 pieces, just increase t by 0.25 each time to get the points
	//the first (t = 0) and the last point (t = 1) don't need calculation as they're already know
#define NR_POINTS	2 //6
	VecFx32 points[NR_POINTS];
	for (int i = 0; i < NR_POINTS; i++)
	{
		if (i == 0)
		{
			points[i].x = mPosition.x;// *FX32_ONE + FX32_HALF;
			points[i].y = mPosition.y;// *TILE_HEIGHT;
			points[i].z = mPosition.z;// *FX32_ONE + FX32_HALF;
		}
		else if (i == (NR_POINTS - 1))
		{
			points[i].x = mEndPosition.x;// *FX32_ONE + FX32_HALF;
			points[i].y = mEndPosition.y;// *TILE_HEIGHT;
			points[i].z = mEndPosition.z;// *FX32_ONE + FX32_HALF;
		}
		else
		{
			VecFx32 center=
			{
				(mPosition.x + mEndPosition.x) / 2, // * FX32_HALF,
				mPosition.y,// * TILE_HEIGHT,
				(mPosition.z + mEndPosition.z) / 2 //* FX32_HALF,
			};
			VecFx32 p0;
			p0.x = mPosition.x;// *FX32_ONE + FX32_HALF;
			p0.y = mPosition.y;// *TILE_HEIGHT;
			p0.z = mPosition.z;// *FX32_ONE + FX32_HALF;
			VecFx32 p1 = center;//p0;
			//if(posdiff.x == 0)
			//	p1.x = p0.x;
			//else
			//	p1.z = p0.z;
			//p1.x += FX32_ONE;
			VecFx32 p3;
			p3.x = mEndPosition.x;// *FX32_ONE + FX32_HALF;
			p3.y = mEndPosition.y;// *TILE_HEIGHT;
			p3.z = mEndPosition.z;// *FX32_ONE + FX32_HALF;
			VecFx32 p2 = center;//p3;
			//if (posdiff.x == 0)
			//	p2.x = p3.x;
			//else
			//	p2.z = p3.z;
			//p2.x -= FX32_ONE;
			points[i].y = mPosition.y;// *TILE_HEIGHT;
			interpolateBezierXZ(&p0, &p1, &p2, &p3, i * FX32_ONE / (NR_POINTS - 1), &points[i]);
		}
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
				VecFx32 normal;
				if (i == 0)
				{
					//normal.x = 0;
					//normal.y = 0;
					//normal.z = FX32_ONE;
					normal.x = -posdiff.z;
					normal.y = 0;
					normal.z = posdiff.x;
				}
				else if (i == (NR_POINTS - 1))
				{
					//NOCASH_Printf("%d, %d, %d\n", posdiff.x, posdiff.y, posdiff.z);
					normal.x = -posdiff.z;
					normal.y = 0;
					normal.z = posdiff.x;
				}
				else
				{
					VecFx32 diff;
					VEC_Subtract(&points[i], &points[i - 1], &diff);
					diff.y = 0;
					VEC_Normalize(&diff, &diff);
					normal.x = -diff.z;
					normal.y = 0;
					normal.z = diff.x;
				}
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


	/*for (int i = 0; i < 5; i++)
	{
		G3_PushMtx();
		{
			G3_Translate(points[i].x, points[i].y + (FX32_ONE / 16), points[i].z);
			VecFx32 vtx[4] =
			{
				{ -FX32_HALF, 0, -FX32_HALF },
				{ -FX32_HALF, 0,  FX32_HALF },
				{ FX32_HALF, 0,  FX32_HALF },
				{ FX32_HALF, 0, -FX32_HALF }
			};
			G3_Begin(GX_BEGIN_QUADS);
			{
				G3_Normal(0, GX_FX16_FX10_MAX, 0);
				G3_TexCoord(0, (8 << tex->nitroHeight) * FX32_ONE);
				G3_Vtx(vtx[0].x, vtx[0].y, vtx[0].z);
				G3_TexCoord((8 << tex->nitroWidth) * FX32_ONE, (8 << tex->nitroHeight) * FX32_ONE);
				G3_Vtx(vtx[1].x, vtx[1].y, vtx[1].z);
				G3_TexCoord((8 << tex->nitroWidth) * FX32_ONE, 0);
				G3_Vtx(vtx[2].x, vtx[2].y, vtx[2].z);
				G3_TexCoord(0, 0);
				G3_Vtx(vtx[3].x, vtx[3].y, vtx[3].z);
			}
			G3_End();
		}
		G3_PopMtx(1);
	}*/
}

fx32 FlexTrack::GetNextDistance(fx32 linDist)
{
	return linDist;
}

void FlexTrack::CalculatePoint(VecFx32* pStartPos, VecFx32* pEndPos, VecFx32* pNextDir, fx32 progress, VecFx32* pPos, VecFx32* pDir)
{
	
}