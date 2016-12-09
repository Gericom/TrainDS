#include <nitro.h>
#include <nnsys/gfd.h>
#include <nnsys/g2d.h>
#include "core.h"
#include "util.h"

void* Util_LoadFileToBuffer(char* path, uint32_t* size, BOOL tempoarly)
{
	FSFile file;
	FS_InitFile(&file);
	FS_OpenFile(&file, path);
	uint32_t mSize = FS_GetLength(&file);
	void* buffer = NNS_FndAllocFromExpHeapEx(gHeapHandle, mSize, (tempoarly ? -32 : 32));
	if (buffer != NULL)
		FS_ReadFile(&file, buffer, (int)mSize);
	FS_CloseFile(&file);
	if (size != NULL) *size = mSize;
	return buffer;
}

void* Util_LoadLZ77FileToBuffer(char* path, uint32_t* size, BOOL tempoarly)
{
	FSFile file;
	FS_InitFile(&file);
	FS_OpenFile(&file, path);
	MICompressionHeader header;
	FS_ReadFile(&file, &header, sizeof(MICompressionHeader));
	uint32_t mSize = MI_GetUncompressedSize(&header);
	void* buffer = NNS_FndAllocFromExpHeapEx(gHeapHandle, mSize, (tempoarly ? -32 : 32));
	if (buffer != NULL)
	{
		void* tmpBuffer = NNS_FndAllocFromExpHeapEx(gHeapHandle, 512, -32);
		MIUncompContextLZ context;
		MI_InitUncompContextLZ(&context, (uint8_t*)buffer, &header);
		int read_len;
		while (1)
		{
			read_len = FS_ReadFile(&file, tmpBuffer, 512);
			if (MI_ReadUncompLZ8(&context, (uint8_t*)tmpBuffer, (u32)read_len) == 0)
				break;
		}
		NNS_FndFreeToExpHeap(gHeapHandle, tmpBuffer);
	}
	FS_CloseFile(&file);
	DC_FlushRange(buffer, mSize);
	if (size != NULL) *size = mSize;
	return buffer;
}

void Util_LoadTextureWithKey(NNSGfdTexKey key, void* data)
{
	GX_BeginLoadTex();
	{
		GX_LoadTex(data, NNS_GfdGetTexKeyAddr(key), NNS_GfdGetTexKeySize(key));
	}
	GX_EndLoadTex();
}

void Util_LoadTexture4x4WithKey(NNSGfdTexKey key, void* data, void* indexData)
{
	GX_BeginLoadTex();
	{
		GX_LoadTex(data, NNS_GfdGetTexKeyAddr(key), NNS_GfdGetTexKeySize(key));
		GX_LoadTex(indexData, GX_COMP4x4_PLTT_IDX(NNS_GfdGetTexKeyAddr(key)), NNS_GfdGetTexKeySize(key) >> 1);
	}
	GX_EndLoadTex();
}

void Util_LoadTextureFromCard(char* texPath, char* plttPath, NNSGfdTexKey &texKey, NNSGfdPlttKey &plttKey)
{
	uint32_t size;
	void* buffer = Util_LoadFileToBuffer(texPath, &size, TRUE);
	DC_FlushRange(buffer, size);

	texKey = NNS_GfdAllocTexVram(size, FALSE, 0);
	Util_LoadTextureWithKey(texKey, buffer);
	NNS_FndFreeToExpHeap(gHeapHandle, buffer);

	buffer = Util_LoadFileToBuffer(plttPath, &size, TRUE);
	DC_FlushRange(buffer, size);

	plttKey = NNS_GfdAllocPlttVram(size, FALSE, 0);
	Util_LoadPaletteWithKey(plttKey, buffer);
	NNS_FndFreeToExpHeap(gHeapHandle, buffer);
}

void Util_LoadTexture4x4FromCard(char* texPath, char* texIndexPath, char* plttPath, NNSGfdTexKey &texKey, NNSGfdPlttKey &plttKey)
{
	uint32_t size;
	void* buffer = Util_LoadFileToBuffer(texPath, &size, TRUE);
	DC_FlushRange(buffer, size);
	void* buffer2 = Util_LoadFileToBuffer(texIndexPath, NULL, TRUE);
	DC_FlushRange(buffer2, size >> 1);

	texKey = NNS_GfdAllocTexVram(size, TRUE, 0);
	Util_LoadTexture4x4WithKey(texKey, buffer, buffer2);
	NNS_FndFreeToExpHeap(gHeapHandle, buffer2);
	NNS_FndFreeToExpHeap(gHeapHandle, buffer);

	buffer = Util_LoadFileToBuffer(plttPath, &size, TRUE);
	DC_FlushRange(buffer, size);

	plttKey = NNS_GfdAllocPlttVram(size, FALSE, 0);
	Util_LoadPaletteWithKey(plttKey, buffer);
	NNS_FndFreeToExpHeap(gHeapHandle, buffer);
}

void Util_LoadPaletteWithKey(NNSGfdPlttKey key, void* data)
{
	GX_BeginLoadTexPltt();
	{
		GX_LoadTexPltt(data, NNS_GfdGetPlttKeyAddr(key), NNS_GfdGetPlttKeySize(key));
	}
	GX_EndLoadTexPltt();
}

static void Util_FreeAllToExpHeapByGroupIdForMBlock(void* memBlock, NNSFndHeapHandle heap, u32 userParam)
{
	if(NNS_FndGetGroupIDForMBlockExpHeap(memBlock) == userParam)
		NNS_FndFreeToExpHeap(heap, memBlock);
}

void Util_FreeAllToExpHeapByGroupId(NNSFndHeapHandle heap, int groupId)
{
	NNS_FndVisitAllocatedForExpHeap(heap, Util_FreeAllToExpHeapByGroupIdForMBlock, groupId);
}

void FX_Lerp(VecFx32* a, VecFx32* b, fx32 t, VecFx32* result)
{
	result->x = a->x + FX_Mul(b->x - a->x, t);
	result->y = a->y + FX_Mul(b->y - a->y, t);
	result->z = a->z + FX_Mul(b->z - a->z, t);
}

#include <nitro/code16.h>
void Util_SetupSubOAMForDouble3D()
{
	GXOamAttr* sOamBak = (GXOamAttr*)NNS_FndAllocFromExpHeapEx(gHeapHandle, sizeof(GXOamAttr) * 128, -32);
	int idx = 0;

	GXS_SetOBJVRamModeBmp(GX_OBJVRAMMODE_BMP_2D_W256);

	for (int i = 0; i < 128; ++i)
	{
		sOamBak[i].attr01 = 0;
		sOamBak[i].attr23 = 0;
	}

	for (int y = 0; y < 192; y += 64)
	{
		for (int x = 0; x < 256; x += 64, idx++)
		{
			G2_SetOBJAttr(&sOamBak[idx],
				x,
				y,
				0,
				GX_OAM_MODE_BITMAPOBJ,
				FALSE,
				GX_OAM_EFFECT_NONE,
				GX_OAM_SHAPE_64x64, GX_OAM_COLOR_16, (y / 8) * 32 + (x / 8), 15, 0);
		}
	}

	DC_FlushRange(&sOamBak[0], sizeof(GXOamAttr) * 128);
	GXS_LoadOAM(&sOamBak[0], 0, sizeof(GXOamAttr) * 128);
	NNS_FndFreeToExpHeap(gHeapHandle, sOamBak);
}
#include <nitro/codereset.h>