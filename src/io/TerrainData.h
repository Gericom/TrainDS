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
	//u8* mTerrainData;
	FSFile mDataFile;

	struct terrain_data_header_t
	{
		uint32_t width;
		uint32_t h_block_count;
		uint32_t height;
		uint32_t v_block_count;
	};
	struct terrain_data_t
	{
		terrain_data_header_t header;
	};
	terrain_data_header_t mDataHeader;//mTerrainData;

public:
	TerrainData(const char* filePath);

	~TerrainData()
	{
		//NNS_FndFreeToExpHeap(gHeapHandle, mDataHeader);
	}

	int GetHBlockCount() const { return mDataHeader.h_block_count; }
	int GetWidth() const { return mDataHeader.width; }
	int GetVBlockCount() const { return mDataHeader.v_block_count; }
	int GetHeight() const { return mDataHeader.height; }

	void GetBlock(int x, int y, hvtx_t* dst);
};