#include <nitro.h>
#include "core.h"
#include "Menu.h"
#include "../terrain/terrain.h"
#include "../terrain/TerrainManager.h"
#include "Game.h"

static tile_t sDummyMap[16][16];
static trackpiece_t sDummyPieces[8];

void Game::Initialize(int arg)
{
	GX_SetBankForLCDC(GX_VRAM_LCDC_ALL);
	MI_CpuClearFast((void *)HW_LCDC_VRAM, HW_LCDC_VRAM_SIZE);
	(void)GX_DisableBankForLCDC();

	MI_CpuFillFast((void *)HW_OAM, 192, HW_OAM_SIZE);   // clear OAM
	MI_CpuClearFast((void *)HW_PLTT, HW_PLTT_SIZE);     // clear the standard palette

	MI_CpuFillFast((void *)HW_DB_OAM, 192, HW_DB_OAM_SIZE);     // clear OAM
	MI_CpuClearFast((void *)HW_DB_PLTT, HW_DB_PLTT_SIZE);       // clear the standard palette

	G3X_Init();
	G3X_InitMtxStack();
	GX_SetBankForTex(GX_VRAM_TEX_01_AB);
	GX_SetBankForTexPltt(GX_VRAM_TEXPLTT_0123_E);
   
   	GX_SetGraphicsMode(GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BG0_AS_3D);
	GX_SetVisiblePlane(GX_PLANEMASK_BG0);
   	GXS_SetGraphicsMode(GX_BGMODE_0);

	G3X_SetShading(GX_SHADING_TOON); 
	G3X_AntiAlias(TRUE);
	G3_SwapBuffers(GX_SORTMODE_AUTO, GX_BUFFERMODE_W);
	
	G3X_AlphaTest(FALSE, 0);                   // AlphaTest OFF
	G3X_AlphaBlend(TRUE);                      // AlphaTest ON

	G3X_SetClearColor(GX_RGB(119 >> 3, 199 >> 3, 244 >> 3),31, 0x7fff, 63, FALSE);
	G3_ViewPort(0, 0, 255, 191);

	GX_SetDispSelect(GX_DISP_SELECT_SUB_MAIN);

	NNS_GfdResetLnkTexVramState();
	NNS_GfdResetLnkPlttVramState();

	mTerrainManager = new TerrainManager();

	MI_CpuClearFast(&sDummyMap[0][0], sizeof(sDummyMap));

	for(int y = 0; y < 16; y++)
	{
		for(int x = 0; x < 16; x++)
		{
			if(y == 5 && x > 6 && x < 8)
			{
				sDummyMap[y][x].groundType = 0;//1;
				sDummyMap[y][x].y = 1;
			}
			else if(y == 5 && x > 6 && x > 8) sDummyMap[y][x].groundType = 0;//1;
			else if(y == 6 && x > 6 && x < 8)
			{
				sDummyMap[y][x].ltCorner = TILE_CORNER_UP;
				sDummyMap[y][x].rtCorner = TILE_CORNER_UP;
				sDummyMap[y][x].lbCorner = TILE_CORNER_FLAT;
				sDummyMap[y][x].rbCorner = TILE_CORNER_FLAT;
			}
			else if(y == 4 && x > 6  && x < 8)
			{
				sDummyMap[y][x].ltCorner = TILE_CORNER_FLAT;
				sDummyMap[y][x].rtCorner = TILE_CORNER_FLAT;
				sDummyMap[y][x].lbCorner = TILE_CORNER_UP;
				sDummyMap[y][x].rbCorner = TILE_CORNER_UP;
			}
		}
	}
	sDummyMap[5][8].ltCorner = TILE_CORNER_UP;
	sDummyMap[5][8].lbCorner = TILE_CORNER_UP;
	sDummyMap[5][8].groundType = 0;//1;
	sDummyMap[4][8].lbCorner = TILE_CORNER_UP;
	sDummyMap[6][8].ltCorner = TILE_CORNER_UP;
	sDummyMap[6][6].rtCorner = TILE_CORNER_UP;
	sDummyMap[4][6].rbCorner = TILE_CORNER_UP;
	sDummyMap[5][6].rtCorner = TILE_CORNER_UP;
	sDummyMap[5][6].rbCorner = TILE_CORNER_UP;
	sDummyPieces[0].kind = TRACKPIECE_KIND_FLAT;
	sDummyPieces[0].rot = TRACKPIECE_ROT_0;
	sDummyPieces[0].x = 7;
	sDummyPieces[0].y = 0;
	sDummyPieces[0].z = 7;
	sDummyPieces[1].kind = TRACKPIECE_KIND_FLAT_SMALL_CURVED;
	sDummyPieces[1].rot = TRACKPIECE_ROT_0;
	sDummyPieces[1].x = 8;
	sDummyPieces[1].y = 0;
	sDummyPieces[1].z = 7;
	sDummyPieces[2].kind = TRACKPIECE_KIND_FLAT;
	sDummyPieces[2].rot = TRACKPIECE_ROT_90;
	sDummyPieces[2].x = 9;
	sDummyPieces[2].y = 0;
	sDummyPieces[2].z = 5;
	sDummyPieces[3].kind = TRACKPIECE_KIND_FLAT_SMALL_CURVED;
	sDummyPieces[3].rot = TRACKPIECE_ROT_90;
	sDummyPieces[3].x = 9;
	sDummyPieces[3].y = 0;
	sDummyPieces[3].z = 4;
	sDummyPieces[4].kind = TRACKPIECE_KIND_FLAT;
	sDummyPieces[4].rot = TRACKPIECE_ROT_180;
	sDummyPieces[4].x = 7;
	sDummyPieces[4].y = 0;
	sDummyPieces[4].z = 3;
	sDummyPieces[5].kind = TRACKPIECE_KIND_FLAT_SMALL_CURVED;
	sDummyPieces[5].rot = TRACKPIECE_ROT_180;
	sDummyPieces[5].x = 6;
	sDummyPieces[5].y = 0;
	sDummyPieces[5].z = 3;
	sDummyPieces[6].kind = TRACKPIECE_KIND_FLAT;
	sDummyPieces[6].rot = TRACKPIECE_ROT_270;
	sDummyPieces[6].x = 5;
	sDummyPieces[6].y = 0;
	sDummyPieces[6].z = 5;
	sDummyPieces[7].kind = TRACKPIECE_KIND_FLAT_SMALL_CURVED;
	sDummyPieces[7].rot = TRACKPIECE_ROT_270;
	sDummyPieces[7].x = 5;
	sDummyPieces[7].y = 0;
	sDummyPieces[7].z = 6;
}

void Game::Render()
{
	G3X_Reset();
	G3X_ResetMtxStack();
	G3_MtxMode(GX_MTXMODE_PROJECTION);
	{
		G3_Identity();
		G3_PerspectiveW(FX32_SIN30, FX32_COS30, (256 * 4096 / 192), 1 * 4096, 512 * 4096, 40960, NULL);
		G3_StoreMtx(0);
	}
	G3_MtxMode(GX_MTXMODE_TEXTURE);
	{
		G3_Identity();
	}
	G3_MtxMode(GX_MTXMODE_POSITION_VECTOR);
	{
		G3_Identity();
		//G3_MtxMode(GX_MTXMODE_POSITION);

		/*G3_PolygonAttr(GX_LIGHTMASK_0, GX_POLYGONMODE_MODULATE, GX_CULL_BACK, 0, 31, GX_POLYGON_ATTR_MISC_NONE);
		//Do this with identity matrix, because we don't want to rotate it
		G3_LightVector(GX_LIGHTID_0, -FX16_SQRT1_3, -FX16_SQRT1_3, FX16_SQRT1_3);
		G3_LightColor(GX_LIGHTID_0, GX_RGB(31,31,31));
		G3_MaterialColorDiffAmb(GX_RGB(31,31,31), GX_RGB(25,25,25), FALSE);
		G3_MaterialColorSpecEmi(GX_RGB(8,8,8), GX_RGB(0,0,0), FALSE);*/

		//G3_PolygonAttr(GX_LIGHTMASK_0, GX_POLYGONMODE_MODULATE, GX_CULL_BACK, 0, 31, GX_POLYGON_ATTR_MISC_NONE);
		//Do this with identity matrix, because we don't want to rotate it
		//{-1630.32, -3750.56, -229.392}
		//G3_LightVector(GX_LIGHTID_0, -1630, -3751, -229);//-FX16_SQRT1_3, -FX16_SQRT1_3, FX16_SQRT1_3);
		//G3_LightColor(GX_LIGHTID_0, GX_RGB(31,31,31));
		//G3_MaterialColorDiffAmb(GX_RGB(31,31,31), GX_RGB(25,25,25), FALSE);
		//G3_MaterialColorSpecEmi(GX_RGB(8,8,8), GX_RGB(0,0,0), FALSE);

		VecFx32 pos;
		//pos.x = 2 * FX32_ONE;
		//pos.y = 1.5 * FX32_ONE;
		//pos.z = -0.5 * FX32_ONE;
		pos.x = 3 * FX32_ONE;
		pos.y = 2.25 * FX32_ONE;
		pos.z = -0.75 * FX32_ONE;
		//pos.x = -0.5 * FX32_ONE;
		//pos.y = 5 * FX32_ONE;
		//pos.z = -2.5 * FX32_ONE;
		VecFx32 up;
		up.x = 0;
		up.y = FX32_ONE;
		up.z = 0;
		//up.x = 0;
		//up.y = 0;
		//up.z = FX32_ONE;
		VecFx32 dst;
		dst.x = 0 * FX32_ONE;
		dst.y = 0;
		dst.z = -2 * FX32_ONE;
		//dst.x = -0.5 * FX32_ONE;
		//dst.y = 0;
		//dst.z = -2.5 * FX32_ONE;

		G3_LookAt(&pos, &up, &dst, NULL);

		/*G3_PolygonAttr(GX_LIGHTMASK_0, GX_POLYGONMODE_MODULATE, GX_CULL_BACK, 0, 31, GX_POLYGON_ATTR_MISC_NONE);
		//Do this with identity matrix, because we don't want to rotate it
		//{-1630.32, -3750.56, -229.392}
		G3_LightVector(GX_LIGHTID_0, -1630, -3751, -229);//-FX16_SQRT1_3, -FX16_SQRT1_3, FX16_SQRT1_3);
		G3_LightColor(GX_LIGHTID_0, GX_RGB(31,31,31));
		G3_MaterialColorDiffAmb(GX_RGB(31,31,31), GX_RGB(25,25,25), FALSE);
		G3_MaterialColorSpecEmi(GX_RGB(8,8,8), GX_RGB(0,0,0), FALSE);*/
		G3_PolygonAttr(GX_LIGHTMASK_0, GX_POLYGONMODE_MODULATE, GX_CULL_BACK, 0, 31, GX_POLYGON_ATTR_MISC_NONE);
		//Do this with identity matrix, because we don't want to rotate it
		//{-2888.16, -307.163, 2888.16}
		//G3_LightVector(GX_LIGHTID_0, -2888, -307, 2888);//-FX16_SQRT1_3, -FX16_SQRT1_3, FX16_SQRT1_3);
		//{568.566, -307.223, 4044.69}
		//175.700, -8.000, 180.000
		//G3_LightVector(GX_LIGHTID_0, 569, -307, 4045);
		//{2888.16, -307.163, 2888.16}
		//G3_LightVector(GX_LIGHTID_0, 2888, -307, 2888);
		//{1108.46, -3784.16, 1108.46}
		//G3_LightVector(GX_LIGHTID_0, -1108, -3784, -1108);
		//{2047.86, -2896.5, 2047.86}
		G3_LightVector(GX_LIGHTID_0, -2048, -2897, -2048);
		G3_LightColor(GX_LIGHTID_0, GX_RGB(31,31,31));
		G3_MaterialColorDiffAmb(GX_RGB(21,21,21), /*GX_RGB(25,25,25)*/GX_RGB(15,15,15), FALSE);
		G3_MaterialColorSpecEmi(GX_RGB(0,0,0), GX_RGB(0,0,0), FALSE);

		G3_Translate(-8 * FX32_ONE, 0, -8 * FX32_ONE);

		G3_PushMtx();
		{
			for(int y = 0; y < 16; y++)
			{
				G3_PushMtx();
				{
					for(int x = 0; x < 16; x++)
					{
						tile_render(&sDummyMap[y][x], mTerrainManager);
						G3_Translate(FX32_ONE, 0, 0);
					}
				}
				G3_PopMtx(1);
				G3_Translate(0, 0, FX32_ONE);
			}
		}
		G3_PopMtx(1);
		for(int i = 0; i < 8; i++)
		{
			trackpiece_render(&sDummyPieces[i], mTerrainManager);
		}
	}
	G3_SwapBuffers(GX_SORTMODE_AUTO, GX_BUFFERMODE_W);
}

void Game::VBlank()
{

}

void Game::Finalize()
{
	delete mTerrainManager;
}