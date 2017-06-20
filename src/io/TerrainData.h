#pragma once

struct hvtx_t
{
	VecFx10 normal;
	uint8_t y;
	uint8_t tex;
	uint16_t texAddress;
};

class TerrainData
{
private:
	FSFile mDataFile;

	struct terrain_data_header_t
	{
		uint32_t width;
		uint32_t h_block_count;
		uint32_t height;
		uint32_t v_block_count;
	};
	terrain_data_header_t mDataHeader;

public:
	TerrainData(const char* filePath)
	{
		FS_InitFile(&mDataFile);
		FS_OpenFile(&mDataFile, filePath);
		FS_ReadFile(&mDataFile, &mDataHeader, sizeof(terrain_data_header_t));
	}

	int GetHBlockCount() { return mDataHeader.h_block_count; }
	int GetWidth() { return mDataHeader.width; }
	int GetVBlockCount() { return mDataHeader.v_block_count; }
	int GetHeight() { return mDataHeader.height; }

	void GetBlock(int x, int y, hvtx_t* dst);
};