#include <nitro.h>
#include <nnsys/gfd.h>
#include "core.h"
#include "util.h"
#include "terrain/TerrainManager.h"
#include "terrain/Map.h"
#include "TrackPieceEx.h"
#include "FlexTrack.h"

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
	G3_PushMtx();
	{
		G3_Translate(32 * FX32_ONE, 0, 32 * FX32_ONE);
		G3_Begin(GX_BEGIN_QUAD_STRIP);
		{
			G3_Normal(0, GX_FX16_FX10_MAX, 0);
			fx32 dist = 0;
			for (int i = 0; i < FLEXTRACK_NR_POINTS; i++)
			{
				VecFx32 normal = mCurveNormals[i];
				G3_PushMtx();
				{
					G3_Translate(mCurvePoints[i].x, mCurvePoints[i].y + (FX32_ONE / 32), mCurvePoints[i].z);
					G3_TexCoord(0, FX_Mul((8 << tex->nitroHeight) * dist, FLEXTRACK_INV_TRACK_WIDTH));
					G3_Vtx(-FX_Mul(normal.x, FLEXTRACK_TRACK_WIDTH) >> 1, 0, -FX_Mul(normal.z, FLEXTRACK_TRACK_WIDTH) >> 1);
					G3_TexCoord((8 << tex->nitroWidth) * FX32_ONE, FX_Mul((8 << tex->nitroHeight) * dist, FLEXTRACK_INV_TRACK_WIDTH));
					G3_Vtx(FX_Mul(normal.x, FLEXTRACK_TRACK_WIDTH) >> 1, 0, FX_Mul(normal.z, FLEXTRACK_TRACK_WIDTH) >> 1);
				}
				G3_PopMtx(1);
				if (i != (FLEXTRACK_NR_POINTS - 1))
					dist += VEC_Distance(&mCurvePoints[i], &mCurvePoints[i + 1]);
			}
		}
		G3_End();
	}
	G3_PopMtx(1);
}