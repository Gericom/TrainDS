#include "common.h"


#include "util.h"
#include "terrain/TerrainManager.h"
#include "RCT2Tree1.h"

#define TREE_TEX_S		(10 * FX32_ONE)
#define TREE_TEX_T		(120 * FX32_ONE)
#define TREE_TEX_WIDTH	(44 * FX32_ONE)
#define TREE_TEX_HEIGHT	(136 * FX32_ONE)

void RCT2Tree1::Render(TerrainManager* terrainManager)
{
	texture_t* tex = terrainManager->GetRCT2TreeTexture();
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
	G3_PushMtx();
	{
		G3_Translate(mPosition.x * FX32_ONE + FX32_HALF, mPosition.y * 1/* + (FX32_ONE >> 4)*/, mPosition.z * FX32_ONE + FX32_HALF);
		Util_SetupBillboardYMatrix();
		const VecFx32 vtx[4] =
		{
			{ -FX32_HALF, 0, 0 },
			{ FX32_HALF, 0, 0 },
			{ FX32_HALF, 3 * FX32_ONE,  0 },
			{ -FX32_HALF, 3 * FX32_ONE,  0 }
		};
		G3_Begin(GX_BEGIN_QUADS);
		{
			G3_Normal(0, GX_FX16_FX10_MAX, 0);
			G3_TexCoord(TREE_TEX_S, TREE_TEX_T + TREE_TEX_HEIGHT);
			G3_Vtx(vtx[0].x, vtx[0].y, vtx[0].z);
			G3_TexCoord(TREE_TEX_S + TREE_TEX_WIDTH, TREE_TEX_T + TREE_TEX_HEIGHT);
			G3_Vtx(vtx[1].x, vtx[1].y, vtx[1].z);
			G3_TexCoord(TREE_TEX_S + TREE_TEX_WIDTH, TREE_TEX_T);
			G3_Vtx(vtx[2].x, vtx[2].y, vtx[2].z);
			G3_TexCoord(TREE_TEX_S, TREE_TEX_T);
			G3_Vtx(vtx[3].x, vtx[3].y, vtx[3].z);
		}
		G3_End();
	}
	G3_PopMtx(1);
}