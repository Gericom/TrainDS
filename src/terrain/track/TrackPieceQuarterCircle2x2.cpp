#include <nitro.h>
#include <nnsys/gfd.h>
#include "core.h"
#include "terrain/TerrainManager.h"
#include "TrackPiece.h"
#include "TrackPieceQuarterCircle2x2.h"

void TrackPieceQuarterCircle2x2::Render(TerrainManager* terrainManager)
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
	G3_PushMtx();
	{
		G3_Translate(x * FX32_ONE * SCENE_SCALE, (y * TILE_HEIGHT + (FX32_ONE >> 6)) * SCENE_SCALE, z * FX32_ONE * SCENE_SCALE);
		G3_Scale(SCENE_SCALE * FX32_ONE, SCENE_SCALE * FX32_ONE, SCENE_SCALE * FX32_ONE);
		switch (rot)
		{
		case TRACKPIECE_ROT_0:
			break;
		case TRACKPIECE_ROT_90:
			G3_Translate(0, 0, FX32_ONE);
			G3_RotY(FX32_SIN90, FX32_COS90);
			break;
		case TRACKPIECE_ROT_180:
			G3_Translate(FX32_ONE, 0, FX32_ONE);
			G3_RotY(FX32_SIN180, FX32_COS180);
			break;
		case TRACKPIECE_ROT_270:
			G3_Translate(FX32_ONE, 0, 0);
			G3_RotY(FX32_SIN270, FX32_COS270);
			break;
		}
		G3_Begin(GX_BEGIN_QUAD_STRIP);
		{
			G3_Normal(0, GX_FX16_FX10_MAX, 0);
			G3_TexCoord(0, (8 << tex->nitroHeight) * FX32_ONE);
			G3_Vtx(0, 0, 0);
			G3_TexCoord((8 << tex->nitroWidth) * FX32_ONE, (8 << tex->nitroHeight) * FX32_ONE);
			G3_Vtx(0, 0, FX32_ONE);

			G3_TexCoord(0, (8 << tex->nitroHeight) * FX32_HALF);
			G3_Vtx(1556, 0, -328);
			G3_TexCoord((8 << tex->nitroWidth) * FX32_ONE, (8 << tex->nitroHeight) * FX32_HALF);
			G3_Vtx(3154, 0, 3482);

			G3_TexCoord(0, 0);
			G3_Vtx(2908, 0, -1188);
			G3_TexCoord((8 << tex->nitroWidth) * FX32_ONE, 0);
			G3_Vtx(5775, 0, 1679);

			G3_TexCoord(0, -(8 << tex->nitroHeight) * FX32_HALF);
			G3_Vtx(3768, 0, -2540);
			G3_TexCoord((8 << tex->nitroWidth) * FX32_ONE, -(8 << tex->nitroHeight) * FX32_HALF);
			G3_Vtx(7578, 0, -842);

			G3_TexCoord(0, -(8 << tex->nitroHeight) * FX32_ONE);
			G3_Vtx(FX32_ONE, 0, -FX32_ONE);
			G3_TexCoord((8 << tex->nitroWidth) * FX32_ONE, -(8 << tex->nitroHeight) * FX32_ONE);
			G3_Vtx(2 * FX32_ONE, 0, -FX32_ONE);
		}
		G3_End();
	}
	G3_PopMtx(1);
}

fx32 TrackPieceQuarterCircle2x2::GetNextDistance(fx32 linDist)
{
	return FX_Mul32x64c(FX_Mul(linDist, FX32_SQRT1_2), FX64C_PI_2);
}

void TrackPieceQuarterCircle2x2::CalculatePoint(VecFx32* pStartPos, VecFx32* pEndPos, VecFx32* pNextDir, fx32 progress, VecFx32* pPos, VecFx32* pDir)
{
	u16 idx = FX_DEG_TO_IDX(progress * 90);
	fx32 sin, cos;
	fx32 sinfix, cosfix;
	if (rot == TRACKPIECE_ROT_0 || rot == TRACKPIECE_ROT_180)
	{
		cos = FX_SinIdx(idx);
		sin = FX_CosIdx(idx);
		sinfix = FX32_ONE - sin;
		cosfix = cos;
	}
	else
	{
		cos = FX_CosIdx(idx);
		sin = FX_SinIdx(idx);
		sinfix = sin;
		cosfix = FX32_ONE - cos;
	}
	pPos->x = pStartPos->x + FX_Mul(pEndPos->x - pStartPos->x, cosfix);
	pPos->y = pStartPos->y + FX_Mul(pEndPos->y - pStartPos->y, progress);
	pPos->z = pStartPos->z + FX_Mul(pEndPos->z - pStartPos->z, sinfix);
	pDir->y = pNextDir->y;
	if (rot == TRACKPIECE_ROT_0)//ai - b
	{
		pDir->x = sin;
		pDir->z = -cos;
	}
	else if (rot == TRACKPIECE_ROT_90)//-a - bi
	{
		pDir->x = -sin;
		pDir->z = -cos;
	}
	else if (rot == TRACKPIECE_ROT_180)//-ai + b
	{
		pDir->x = -sin;
		pDir->z = cos;
	}
	else if (rot == TRACKPIECE_ROT_270)//a + bi
	{
		pDir->x = sin;
		pDir->z = cos;
	}
	VEC_Normalize(pDir, pDir);
}