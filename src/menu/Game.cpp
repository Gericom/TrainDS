#include <nitro.h>
#include "core.h"
#include "Menu.h"
#include "../terrain/terrain.h"
#include "../terrain/TerrainManager.h"
#include "Game.h"

static tile_t sDummyMap[16][16];

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

	mTerrainManager = new TerrainManager();

	MI_CpuClearFast(&sDummyMap[0][0], sizeof(sDummyMap));

	for(int y = 0; y < 16; y++)
	{
		for(int x = 0; x < 16; x++)
		{
			if(y == 5 && x < 8)
			{
				sDummyMap[y][x].groundType = 1;
				sDummyMap[y][x].y = 1;
			}
			else if(y == 5 && x > 8) sDummyMap[y][x].groundType = 1;
			else if(y == 6 && x < 8)
			{
				sDummyMap[y][x].ltCorner = TILE_CORNER_UP;
				sDummyMap[y][x].rtCorner = TILE_CORNER_UP;
				sDummyMap[y][x].lbCorner = TILE_CORNER_FLAT;
				sDummyMap[y][x].rbCorner = TILE_CORNER_FLAT;
			}
			else if(y == 4 && x < 8)
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
	sDummyMap[5][8].groundType = 1;
	sDummyMap[4][8].lbCorner = TILE_CORNER_UP;
	sDummyMap[6][8].ltCorner = TILE_CORNER_UP;
}

void Game::Render()
{
	G3X_Reset();
	G3_MtxMode(GX_MTXMODE_PROJECTION);
	{
		G3_Identity();
		G3_Perspective(FX32_SIN30, FX32_COS30, (256 * 4096 / 192), 1 * 4096, 512 * 4096, NULL);
		G3_StoreMtx(0);
	}
	G3_MtxMode(GX_MTXMODE_TEXTURE);
	{
		G3_Identity();
	}
	G3_MtxMode(GX_MTXMODE_POSITION_VECTOR);
	{
		G3_Identity();
		G3_MtxMode(GX_MTXMODE_POSITION);

		G3_PolygonAttr(GX_LIGHTMASK_0, GX_POLYGONMODE_MODULATE, GX_CULL_BACK, 0, 31, GX_POLYGON_ATTR_MISC_NONE);
		//Do this with identity matrix, because we don't want to rotate it
		G3_LightVector(GX_LIGHTID_0, -FX16_SQRT1_3, -FX16_SQRT1_3, FX16_SQRT1_3);
		G3_LightColor(GX_LIGHTID_0, GX_RGB(31,31,31));
		G3_MaterialColorDiffAmb(GX_RGB(31,31,31), GX_RGB(25,25,25), FALSE);
		G3_MaterialColorSpecEmi(GX_RGB(8,8,8), GX_RGB(0,0,0), FALSE);

		VecFx32 pos;
		pos.x = 2 * FX32_ONE;
		pos.y = 1.5 * FX32_ONE;
		pos.z = -0.5 * FX32_ONE;
		VecFx32 up;
		up.x = 0;
		up.y = FX32_ONE;
		up.z = 0;
		VecFx32 dst;
		dst.x = 0 * FX32_ONE;
		dst.y = 0;
		dst.z = -2 * FX32_ONE;
			
		G3_LookAt(&pos, &up, &dst, NULL);
		G3_Translate(-8 * FX32_ONE, 0, -8 * FX32_ONE);

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
	G3_SwapBuffers(GX_SORTMODE_AUTO, GX_BUFFERMODE_W);
}

void Game::VBlank()
{

}

void Game::Finalize()
{

}