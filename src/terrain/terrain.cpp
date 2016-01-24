#include <nitro.h>
#include <nnsys/gfd.h>
#include "core.h"
#include "terrain.h"
#include "TerrainManager.h"

static fx32 tile_corner_to_y[4] = {0, TILE_HEIGHT, -TILE_HEIGHT, 0};

//TODO: Make a map class or something that requires a terrainmanager in its constructor,
//		and make that class include this function
void tile_render(tile_t* tile, TerrainManager* terrainManager)
{
	texture_t* tex = terrainManager->GetTerrainTexture(tile->groundType);
	//G3_Color(GX_RGB(31,31,31));
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
	G3_PushMtx();
	{
		G3_Translate(0, tile->y * TILE_HEIGHT, 0);
		//Can we do it with one quad?
		//if(tile->ltCorner + tile->rtCorner + tile->lbCorner + tile->rbCorner == 0 ||
		//	(tile->ltCorner == TILE_CORNER_UP && tile->rbCorner == TILE_CORNER_DOWN && tile->rtCorner + tile->lbCorner == 0) ||
		//	(tile->ltCorner == TILE_CORNER_DOWN && tile->rbCorner == TILE_CORNER_UP && tile->rtCorner + tile->lbCorner == 0) ||
		//	(tile->rtCorner == TILE_CORNER_DOWN && tile->lbCorner == TILE_CORNER_UP && tile->ltCorner + tile->rbCorner == 0) ||
		//	(tile->rtCorner == TILE_CORNER_UP && tile->lbCorner == TILE_CORNER_DOWN && tile->ltCorner + tile->rbCorner == 0))
		//{
			G3_Begin(GX_BEGIN_QUADS);
			{
				if(tile->ltCorner + tile->rtCorner + tile->lbCorner + tile->rbCorner == 0)
					G3_Normal(0, GX_FX16_FX10_MAX, 0);
				else if(tile->ltCorner == TILE_CORNER_UP && tile->rtCorner == TILE_CORNER_UP && tile->lbCorner + tile->rbCorner == 0)
					G3_Normal(0, 497, 124);
				else if(tile->lbCorner == TILE_CORNER_UP && tile->rbCorner == TILE_CORNER_UP && tile->ltCorner + tile->rtCorner == 0)
					G3_Normal(0, 497, -124);
				if(tex != NULL) G3_TexCoord(0, 0);
				G3_Vtx(0, tile_corner_to_y[tile->ltCorner], 0);
				if(tex != NULL) G3_TexCoord(0, (8 << tex->nitroHeight) * FX32_ONE);
				G3_Vtx(0, tile_corner_to_y[tile->lbCorner],  FX32_ONE);
				if(tex != NULL) G3_TexCoord((8 << tex->nitroWidth) * FX32_ONE, (8 << tex->nitroHeight) * FX32_ONE);
				G3_Vtx(FX32_ONE, tile_corner_to_y[tile->rbCorner],  FX32_ONE);
				if(tex != NULL) G3_TexCoord((8 << tex->nitroWidth) * FX32_ONE, 0);
				G3_Vtx(FX32_ONE, tile_corner_to_y[tile->rtCorner], 0);
			}
			G3_End();
		//}
		//else
		//{
			//Just one corner up
		//	if(tile->ltCorner + tile->rtCorner + tile->lbCorner + tile->rbCorner == TILE_CORNER_UP)
		//	{

		//	}
		//}
	}
	G3_PopMtx(1);
}