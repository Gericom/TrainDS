#include <nitro.h>
#include <nnsys/gfd.h>
#include "core.h"
#include "util.h"
#include "terrain/TerrainManager.h"
#include "TrackPiece.h"
#include "TrackPieceStraight1x1.h"

void TrackPieceStraight1x1::Render(TerrainManager* terrainManager)
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
		G3_Translate(x * FX32_ONE, y * TILE_HEIGHT + (FX32_ONE / 16)/* + (FX32_ONE >> 4)*/, z * FX32_ONE);
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
		VecFx32 vtx[4] =
		{
			{ 0, 0, 0 },
			{ 0, 0,  FX32_ONE },
			{ FX32_ONE, 0,  FX32_ONE },
			{ FX32_ONE, 0, 0 }
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
}

fx32 TrackPieceStraight1x1::GetNextDistance(fx32 linDist)
{
	return linDist;
}

void TrackPieceStraight1x1::CalculatePoint(VecFx32* pStartPos, VecFx32* pEndPos, VecFx32* pNextDir, fx32 progress, VecFx32* pPos, VecFx32* pDir)
{
	FX_Lerp(pStartPos, pEndPos, progress, pPos);
	*pDir = *pNextDir;
}