#include "common.h"
#include "util.h"
#include "TerrainData.h"

TerrainData::TerrainData(const char* filePath)
{
	mTerrainData = (terrain_data_t*)Util_LoadLZ77FileToBuffer(filePath, NULL, false);
	//FS_InitFile(&mDataFile);
	//FS_OpenFile(&mDataFile, filePath);
	//FS_ReadFile(&mDataFile, &mDataHeader, sizeof(terrain_data_header_t));
}

void TerrainData::GetBlock(int x, int y, hvtx_t* dst)
{
	if (x < 0 || y < 0 || x > mTerrainData->header.h_block_count || y > mTerrainData->header.v_block_count)
		return;
	int block = y * mTerrainData->header.h_block_count + x;
	//FS_SeekFile(&mDataFile, sizeof(terrain_data_header_t) + block * 2 * 132 * 132, FS_SEEK_SET);
	u8* datas = ((u8*)mTerrainData) + sizeof(terrain_data_header_t) + block * 2 * 132 * 132;//(u8*)NNS_FndAllocFromExpHeapEx(gHeapHandle, 2 * 132 * 132, -32);
	//FS_ReadFile(&mDataFile, datas, 2 * 132 * 132);
	u8* pHeight = datas;
	u8* pTex = datas + 132 * 132;
	for (int y2 = 0; y2 < 132 * 132; y2++)
	{
		//dst->normal = GX_VECFX10(0,GX_FX16_FX10_MAX,0);
		dst->y = *pHeight++;
		dst->tex = *pTex++;
		//dst->texAddress = 0;
		dst++;
	}
	//NNS_FndFreeToExpHeap(gHeapHandle, datas);
}