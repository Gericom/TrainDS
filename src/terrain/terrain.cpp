#include <nitro.h>
#include <nnsys/gfd.h>
#include "core.h"
#include "terrain.h"
#include "TerrainManager.h"

void tile_render(tile_t* tile)
{
	texture_t* tex = gTerrainManager->GetTerrainTexture(tile->groundType);
	G3_Color(GX_RGB(31,31,31));
	if(tex == NULL)
	{
		G3_TexImageParam(GX_TEXFMT_NONE, GX_TEXGEN_NONE, GX_TEXSIZE_S8, GX_TEXSIZE_T8, GX_TEXREPEAT_NONE, GX_TEXFLIP_NONE, GX_TEXPLTTCOLOR0_USE, 0);
	}
	else
	{
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

	}
	//if(tile->groundType == 0) G3_Color(GX_RGB(129 >> 3, 159 >> 3, 42 >> 3));
	//else if(tile->groundType == 1) G3_Color(GX_RGB(31,31,31));
	G3_Begin(GX_BEGIN_QUADS);
	{
		if(tex != NULL) G3_TexCoord(0, 0);
		G3_Vtx(0, 0, 0);
		if(tex != NULL) G3_TexCoord(0, (8 << tex->nitroHeight) * FX32_ONE);
		G3_Vtx(0, 0,  FX32_ONE);
		if(tex != NULL) G3_TexCoord((8 << tex->nitroWidth) * FX32_ONE, (8 << tex->nitroHeight) * FX32_ONE);
		G3_Vtx(FX32_ONE, 0,  FX32_ONE);
		if(tex != NULL) G3_TexCoord((8 << tex->nitroWidth) * FX32_ONE, 0);
		G3_Vtx(FX32_ONE, 0, 0);
	}
	G3_End();
}