#include <nitro.h>
#include <nnsys/gfd.h>
#include "core.h"
#include "terrain.h"
#include "TerrainManager.h"

static fx32 tile_corner_to_y[4] = {0, TILE_HEIGHT, -TILE_HEIGHT, 0};

//fix error in include (; removed)
#undef GX_FX32_FX10_MAX
#undef GX_FX32_FX10_MIN

#define GX_FX32_FX10_MAX          (fx32)(0x00000fff)
#define GX_FX32_FX10_MIN          (fx32)(0xfffff000)

static VecFx16 normalsForCorners[256];

void setup_normals()
{
	for (int i = 0; i < 256; i++)
	{
		tile_t tile;
		tile.corners = i;
		if (tile.ltCorner == 3 || tile.rtCorner == 3 || tile.lbCorner == 3 || tile.rbCorner == 3)
			continue;
		VecFx32 vtx[4] =
		{
			{ 0, tile_corner_to_y[tile.ltCorner], 0 },
			{ 0, tile_corner_to_y[tile.lbCorner],  FX32_ONE },
			{ FX32_ONE, tile_corner_to_y[tile.rbCorner],  FX32_ONE },
			{ FX32_ONE, tile_corner_to_y[tile.rtCorner], 0 }
		};
		if (tile.ltCorner + tile.lbCorner + tile.rbCorner + tile.rtCorner == 0)
		{
			normalsForCorners[i].x = 0;
			normalsForCorners[i].y = GX_FX16_FX10_MAX;
			normalsForCorners[i].z = 0;
		}
		else if ((tile.ltCorner == TILE_CORNER_UP && tile.rbCorner == TILE_CORNER_DOWN && tile.rtCorner + tile.lbCorner == 0) ||
			(tile.ltCorner == TILE_CORNER_DOWN && tile.rbCorner == TILE_CORNER_UP && tile.rtCorner + tile.lbCorner == 0) ||
			(tile.rtCorner == TILE_CORNER_DOWN && tile.lbCorner == TILE_CORNER_UP && tile.ltCorner + tile.rbCorner == 0) ||
			(tile.rtCorner == TILE_CORNER_UP && tile.lbCorner == TILE_CORNER_DOWN && tile.ltCorner + tile.rbCorner == 0))
		{
			VecFx32 tmpA, tmpB, tmpC, normal;
			VEC_Subtract(&vtx[3], &vtx[0], &tmpA);
			VEC_Subtract(&vtx[1], &vtx[0], &tmpB);
			VEC_CrossProduct(&tmpA, &tmpB, &tmpC);
			VEC_Normalize(&tmpC, &normal);
			if (normal.x > GX_FX32_FX10_MAX) normal.x = GX_FX32_FX10_MAX;
			else if (normal.x < GX_FX32_FX10_MIN) normal.x = GX_FX32_FX10_MIN;
			if (normal.y > GX_FX32_FX10_MAX) normal.y = GX_FX32_FX10_MAX;
			else if (normal.y < GX_FX32_FX10_MIN) normal.y = GX_FX32_FX10_MIN;
			if (normal.z > GX_FX32_FX10_MAX) normal.z = GX_FX32_FX10_MAX;
			else if (normal.z < GX_FX32_FX10_MIN) normal.z = GX_FX32_FX10_MIN;
			normalsForCorners[i].x = -normal.x;
			normalsForCorners[i].y = -normal.y;
			normalsForCorners[i].z = -normal.z;
		}
		else if ((tile.ltCorner == TILE_CORNER_UP && tile.rtCorner == TILE_CORNER_UP && tile.lbCorner + tile.rbCorner == 0) ||
			(tile.ltCorner == TILE_CORNER_DOWN && tile.rtCorner == TILE_CORNER_DOWN && tile.lbCorner + tile.rbCorner == 0) ||
			(tile.rtCorner == TILE_CORNER_UP && tile.rbCorner == TILE_CORNER_UP && tile.ltCorner + tile.lbCorner == 0) ||
			(tile.rtCorner == TILE_CORNER_DOWN && tile.rbCorner == TILE_CORNER_DOWN && tile.ltCorner + tile.lbCorner == 0) ||
			(tile.rbCorner == TILE_CORNER_UP && tile.lbCorner == TILE_CORNER_UP && tile.ltCorner + tile.rtCorner == 0) ||
			(tile.rbCorner == TILE_CORNER_DOWN && tile.lbCorner == TILE_CORNER_DOWN && tile.ltCorner + tile.rtCorner == 0) ||
			(tile.ltCorner == TILE_CORNER_UP && tile.lbCorner == TILE_CORNER_UP && tile.rtCorner + tile.rbCorner == 0) ||
			(tile.ltCorner == TILE_CORNER_DOWN && tile.lbCorner == TILE_CORNER_DOWN && tile.rtCorner + tile.rbCorner == 0))
		{
			VecFx32 tmpA, tmpB, tmpC, normal;
			VEC_Subtract(&vtx[3], &vtx[0], &tmpA);
			VEC_Subtract(&vtx[1], &vtx[0], &tmpB);
			VEC_CrossProduct(&tmpA, &tmpB, &tmpC);
			VEC_Normalize(&tmpC, &normal);
			if (normal.x > GX_FX32_FX10_MAX) normal.x = GX_FX32_FX10_MAX;
			else if (normal.x < GX_FX32_FX10_MIN) normal.x = GX_FX32_FX10_MIN;
			if (normal.y > GX_FX32_FX10_MAX) normal.y = GX_FX32_FX10_MAX;
			else if (normal.y < GX_FX32_FX10_MIN) normal.y = GX_FX32_FX10_MIN;
			if (normal.z > GX_FX32_FX10_MAX) normal.z = GX_FX32_FX10_MAX;
			else if (normal.z < GX_FX32_FX10_MIN) normal.z = GX_FX32_FX10_MIN;
			normalsForCorners[i].x = -normal.x;
			normalsForCorners[i].y = -normal.y;
			normalsForCorners[i].z = -normal.z;
		}
		else
		{
			int idx = 0;
			if (vtx[0].y == 0 && vtx[1].y == 0 && vtx[2].y == 0 && vtx[3].y != 0) idx = 0;
			else if (vtx[0].y == 0 && vtx[1].y == 0 && vtx[2].y != 0 && vtx[3].y == 0) idx = 3;
			else if (vtx[0].y == 0 && vtx[1].y != 0 && vtx[2].y == 0 && vtx[3].y == 0) idx = 2;
			else if (vtx[0].y != 0 && vtx[1].y == 0 && vtx[2].y == 0 && vtx[3].y == 0) idx = 1;
			//First triangle
			idx = (idx + 1) & 3;
			idx = (idx + 1) & 3;
			//Second triangle
			VecFx32 tmpA, tmpB, tmpC, normal;
			VEC_Subtract(&vtx[idx], &vtx[(idx + 1) & 3], &tmpA);
			VEC_Subtract(&vtx[(idx + 2) & 3], &vtx[(idx + 1) & 3], &tmpB);
			VEC_CrossProduct(&tmpA, &tmpB, &tmpC);
			VEC_Normalize(&tmpC, &normal);
			if (normal.x > GX_FX32_FX10_MAX) normal.x = GX_FX32_FX10_MAX;
			else if (normal.x < GX_FX32_FX10_MIN) normal.x = GX_FX32_FX10_MIN;
			if (normal.y > GX_FX32_FX10_MAX) normal.y = GX_FX32_FX10_MAX;
			else if (normal.y < GX_FX32_FX10_MIN) normal.y = GX_FX32_FX10_MIN;
			if (normal.z > GX_FX32_FX10_MAX) normal.z = GX_FX32_FX10_MAX;
			else if (normal.z < GX_FX32_FX10_MIN) normal.z = GX_FX32_FX10_MIN;
			normalsForCorners[i].x = -normal.x;
			normalsForCorners[i].y = -normal.y;
			normalsForCorners[i].z = -normal.z;
		}
	}
}

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

		VecFx32 vtx[4] =
		{
			{0, tile_corner_to_y[tile->ltCorner], 0},
			{0, tile_corner_to_y[tile->lbCorner],  FX32_ONE},
			{FX32_ONE, tile_corner_to_y[tile->rbCorner],  FX32_ONE},
			{FX32_ONE, tile_corner_to_y[tile->rtCorner], 0}
		};

		if(tile->ltCorner + tile->lbCorner + tile->rbCorner + tile->rtCorner == 0)
		{
			G3_Begin(GX_BEGIN_QUADS);
			{
				G3_Normal(0, GX_FX16_FX10_MAX, 0);
				G3_TexCoord(0, 0);
				G3_Vtx(vtx[0].x, vtx[0].y, vtx[0].z);
				G3_TexCoord(0, (8 << tex->nitroHeight) * FX32_ONE);
				G3_Vtx(vtx[1].x, vtx[1].y, vtx[1].z);
				G3_TexCoord((8 << tex->nitroWidth) * FX32_ONE, (8 << tex->nitroHeight) * FX32_ONE);
				G3_Vtx(vtx[2].x, vtx[2].y, vtx[2].z);
				G3_TexCoord((8 << tex->nitroWidth) * FX32_ONE, 0);
				G3_Vtx(vtx[3].x, vtx[3].y, vtx[3].z);
			}
			G3_End();
		}
		else if((tile->ltCorner == TILE_CORNER_UP && tile->rbCorner == TILE_CORNER_DOWN && tile->rtCorner + tile->lbCorner == 0) || 
				(tile->ltCorner == TILE_CORNER_DOWN && tile->rbCorner == TILE_CORNER_UP && tile->rtCorner + tile->lbCorner == 0) || 
				(tile->rtCorner == TILE_CORNER_DOWN && tile->lbCorner == TILE_CORNER_UP && tile->ltCorner + tile->rbCorner == 0) || 
				(tile->rtCorner == TILE_CORNER_UP && tile->lbCorner == TILE_CORNER_DOWN && tile->ltCorner + tile->rbCorner == 0))
		{
			G3_Begin(GX_BEGIN_QUADS);
			{
				G3_Normal(normalsForCorners[tile->corners].x, normalsForCorners[tile->corners].y, normalsForCorners[tile->corners].z);
				G3_TexCoord(0, 0);
				G3_Vtx(vtx[0].x, vtx[0].y, vtx[0].z);
				G3_TexCoord(0, (8 << tex->nitroHeight) * FX32_ONE);
				G3_Vtx(vtx[1].x, vtx[1].y, vtx[1].z);
				G3_TexCoord((8 << tex->nitroWidth) * FX32_ONE, (8 << tex->nitroHeight) * FX32_ONE);
				G3_Vtx(vtx[2].x, vtx[2].y, vtx[2].z);
				G3_TexCoord((8 << tex->nitroWidth) * FX32_ONE, 0);
				G3_Vtx(vtx[3].x, vtx[3].y, vtx[3].z);
			}
			G3_End();
		}
		else if((tile->ltCorner == TILE_CORNER_UP && tile->rtCorner == TILE_CORNER_UP && tile->lbCorner + tile->rbCorner == 0) || 
				(tile->ltCorner == TILE_CORNER_DOWN && tile->rtCorner == TILE_CORNER_DOWN && tile->lbCorner + tile->rbCorner == 0) || 
				(tile->rtCorner == TILE_CORNER_UP && tile->rbCorner == TILE_CORNER_UP && tile->ltCorner + tile->lbCorner == 0) || 
				(tile->rtCorner == TILE_CORNER_DOWN && tile->rbCorner == TILE_CORNER_DOWN && tile->ltCorner + tile->lbCorner == 0) || 
				(tile->rbCorner == TILE_CORNER_UP && tile->lbCorner == TILE_CORNER_UP && tile->ltCorner + tile->rtCorner == 0) || 
				(tile->rbCorner == TILE_CORNER_DOWN && tile->lbCorner == TILE_CORNER_DOWN && tile->ltCorner + tile->rtCorner == 0) || 
				(tile->ltCorner == TILE_CORNER_UP && tile->lbCorner == TILE_CORNER_UP && tile->rtCorner + tile->rbCorner == 0) || 
				(tile->ltCorner == TILE_CORNER_DOWN && tile->lbCorner == TILE_CORNER_DOWN && tile->rtCorner + tile->rbCorner == 0))
		{
			G3_Begin(GX_BEGIN_QUADS);
			{
				G3_Normal(normalsForCorners[tile->corners].x, normalsForCorners[tile->corners].y, normalsForCorners[tile->corners].z);
				G3_TexCoord(0, 0);
				G3_Vtx(vtx[0].x, vtx[0].y, vtx[0].z);
				G3_TexCoord(0, (8 << tex->nitroHeight) * FX32_ONE);
				G3_Vtx(vtx[1].x, vtx[1].y, vtx[1].z);
				G3_TexCoord((8 << tex->nitroWidth) * FX32_ONE, (8 << tex->nitroHeight) * FX32_ONE);
				G3_Vtx(vtx[2].x, vtx[2].y, vtx[2].z);
				G3_TexCoord((8 << tex->nitroWidth) * FX32_ONE, 0);
				G3_Vtx(vtx[3].x, vtx[3].y, vtx[3].z);
			}
			G3_End();
		}
		else
		{
			int idx = 0;
			if(vtx[0].y == 0 && vtx[1].y == 0 && vtx[2].y == 0 && vtx[3].y != 0) idx = 0;
			else if(vtx[0].y == 0 && vtx[1].y == 0 && vtx[2].y != 0 && vtx[3].y == 0) idx = 3;
			else if(vtx[0].y == 0 && vtx[1].y != 0 && vtx[2].y == 0 && vtx[3].y == 0) idx = 2;
			else if(vtx[0].y != 0 && vtx[1].y == 0 && vtx[2].y == 0 && vtx[3].y == 0) idx = 1;
			G3_Begin(GX_BEGIN_TRIANGLES);
			{
				//First triangle
				G3_Normal(0, GX_FX16_FX10_MAX, 0);
				G3_TexCoord(0, 0);
				G3_Vtx(vtx[idx].x, vtx[idx].y, vtx[idx].z);
				idx = (idx + 1) & 3;
				G3_TexCoord(0, (8 << tex->nitroHeight) * FX32_ONE);
				G3_Vtx(vtx[idx].x, vtx[idx].y, vtx[idx].z);
				idx = (idx + 1) & 3;
				G3_TexCoord((8 << tex->nitroWidth) * FX32_ONE, (8 << tex->nitroHeight) * FX32_ONE);
				G3_Vtx(vtx[idx].x, vtx[idx].y, vtx[idx].z);
				//Second triangle
				G3_Normal(normalsForCorners[tile->corners].x, normalsForCorners[tile->corners].y, normalsForCorners[tile->corners].z);
				G3_TexCoord(0, 0);
				G3_Vtx(vtx[idx].x, vtx[idx].y, vtx[idx].z);
				idx = (idx + 1) & 3;
				G3_TexCoord(0, (8 << tex->nitroHeight) * FX32_ONE);
				G3_Vtx(vtx[idx].x, vtx[idx].y, vtx[idx].z);
				idx = (idx + 1) & 3;
				G3_TexCoord((8 << tex->nitroWidth) * FX32_ONE, (8 << tex->nitroHeight) * FX32_ONE);
				G3_Vtx(vtx[idx].x, vtx[idx].y, vtx[idx].z);
				idx = (idx + 1) & 3;
			}
			G3_End();
		}
	}
	G3_PopMtx(1);
}