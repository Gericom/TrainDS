#include "common.h"


#include "util.h"
#include "terrain/TerrainManager.h"
#include "terrain/Map.h"
#include "TrackPieceEx.h"
#include "FlexTrack.h"

void FlexTrack::Render()
{
	texture_t* tex = mMap->mTerrainManager->GetTrackTexture();
	G3_TexImageParam((GXTexFmt)tex->nitroFormat,       // use alpha texture
		GX_TEXGEN_TEXCOORD,    // use texcoord
		(GXTexSizeS)tex->nitroWidth,        // 16 pixels
		(GXTexSizeT)tex->nitroHeight,        // 16 pixels
		GX_TEXREPEAT_ST,     // no repeat
		GX_TEXFLIP_NONE,       // no flip
		GX_TEXPLTTCOLOR0_USE,  // use color 0 of the palette
		NNS_GfdGetTexKeyAddr(tex->texKey)     // the offset of the texture image
	);
	fx32 texWidth = (8 << tex->nitroWidth) * FX32_ONE;
	fx32 texHeight = (8 << tex->nitroHeight) * FLEXTRACK_INV_TRACK_WIDTH;
	G3_TexPlttBase(NNS_GfdGetPlttKeyAddr(tex->plttKey), (GXTexFmt)tex->nitroFormat);
	G3_PolygonAttr(GX_LIGHTMASK_0, GX_POLYGONMODE_MODULATE, GX_CULL_BACK, 0, 31, GX_POLYGON_ATTR_MISC_FOG | GX_POLYGON_ATTR_MISC_FAR_CLIPPING);
	G3_MaterialColorDiffAmb(GX_RGB(31, 31, 31), GX_RGB(10, 10, 10), false);
	G3_MaterialColorSpecEmi(GX_RGB(1, 1, 1), GX_RGB(0, 0, 0), false);
	G3_PushMtx();
	{
		//G3_Translate(32 * FX32_ONE, 0, 32 * FX32_ONE);
		G3_Begin(GX_BEGIN_QUAD_STRIP);
		{
			G3_Normal(0, GX_FX16_FX10_MAX, 0);
			fx32 dist = 0;
			for (int i = 0; i < FLEXTRACK_NR_POINTS; i++)
			{
				VecFx32 normal = mCurveNormals[i];
				G3_PushMtx();
				{
					G3_Translate(mCurvePoints[i].x, mCurvePoints[i].y, mCurvePoints[i].z);

					reg_G3_TEXCOORD = GX_PACK_TEXCOORD_PARAM(0, FX_Mul(texHeight, dist) - 2048 * 4096);

					reg_G3_VTX_16 = (-FX_Mul(normal.x, FLEXTRACK_TRACK_WIDTH) >> 1) & 0xFFFF;
					reg_G3_VTX_16 = (-FX_Mul(normal.z, FLEXTRACK_TRACK_WIDTH) >> 1) & 0xFFFF;

					reg_G3_TEXCOORD = GX_PACK_TEXCOORD_PARAM(texWidth, FX_Mul(texHeight, dist) - 2048 * 4096);

					reg_G3_VTX_16 = (FX_Mul(normal.x, FLEXTRACK_TRACK_WIDTH) >> 1) & 0xFFFF;
					reg_G3_VTX_16 = (FX_Mul(normal.z, FLEXTRACK_TRACK_WIDTH) >> 1) & 0xFFFF;
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