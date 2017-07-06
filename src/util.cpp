#include <nitro.h>
#include <nnsys/gfd.h>
#include <nnsys/g2d.h>
#include "core.h"
#include "lh.h"
//#include "lrc.h"
#include "util.h"

void* Util_LoadFileToBuffer(const char* path, uint32_t* size, bool tempoarly)
{
	FSFile file;
	FS_InitFile(&file);
	FS_OpenFile(&file, path);
	uint32_t mSize = FS_GetLength(&file);
	void* buffer = NNS_FndAllocFromExpHeapEx(gHeapHandle, mSize, (tempoarly ? -32 : 32));
	if (buffer != NULL)
		FS_ReadFile(&file, buffer, (int)mSize);
	FS_CloseFile(&file);
	DC_FlushRange(buffer, mSize);
	if (size != NULL) *size = mSize;
	return buffer;
}

void* Util_LoadLZ77FileToBuffer(const char* path, uint32_t* size, bool tempoarly)
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

void* Util_LoadLHFileToBuffer(const char* path, uint32_t* size, bool tempoarly)
{
	FSFile file;
	FS_InitFile(&file);
	FS_OpenFile(&file, path);

	CXCompressionHeader header;
	FS_ReadFile(&file, &header, sizeof(CXCompressionHeader));
	uint32_t mSize = CXGetUncompressedSize(&header);
	FS_SeekFile(&file, 0, FS_SEEK_SET);
	void* buffer = NNS_FndAllocFromExpHeapEx(gHeapHandle, mSize, (tempoarly ? -32 : 32));
	if (buffer != NULL)
	{
		CXUncompContextLH* context = (CXUncompContextLH*)NNS_FndAllocFromExpHeapEx(gHeapHandle, sizeof(CXUncompContextLH), -32);
		void* tmpBuffer = NNS_FndAllocFromExpHeapEx(gHeapHandle, 512, -32);
		CXInitUncompContextLH(context, (uint8_t*)buffer);
		int read_len;
		while (1)
		{
			read_len = FS_ReadFile(&file, tmpBuffer, 512);
			if (CXReadUncompLH(context, (uint8_t*)tmpBuffer, (u32)read_len) == 0)
				break;
		}
		NNS_FndFreeToExpHeap(gHeapHandle, tmpBuffer);
		NNS_FndFreeToExpHeap(gHeapHandle, context);
	}
	FS_CloseFile(&file);
	DC_FlushRange(buffer, mSize);
	if (size != NULL)
		*size = mSize;
	return buffer;
}

/*void* Util_LoadLRCFileToBuffer(char* path, uint32_t* size, bool tempoarly)
{
	FSFile file;
	FS_InitFile(&file);
	FS_OpenFile(&file, path);

	CXCompressionHeader header;
	FS_ReadFile(&file, &header, sizeof(CXCompressionHeader));
	uint32_t mSize = CXGetUncompressedSize(&header);
	FS_SeekFile(&file, 0, FS_SEEK_SET);
	void* buffer = NNS_FndAllocFromExpHeapEx(gHeapHandle, mSize, (tempoarly ? -32 : 32));
	if (buffer != NULL)
	{
		CXUncompContextLRC* context = (CXUncompContextLRC*)NNS_FndAllocFromExpHeapEx(gHeapHandle, sizeof(CXUncompContextLRC), -32);
		void* tmpBuffer = NNS_FndAllocFromExpHeapEx(gHeapHandle, 2048, -32);
		CXInitUncompContextLRC(context, (uint8_t*)buffer);
		int read_len;
		while (1)
		{
			read_len = FS_ReadFile(&file, tmpBuffer, 2048);
			if (CXReadUncompLRC(context, (uint8_t*)tmpBuffer, (u32)read_len) == 0)
				break;
		}
		NNS_FndFreeToExpHeap(gHeapHandle, tmpBuffer);
		NNS_FndFreeToExpHeap(gHeapHandle, context);
	}
	FS_CloseFile(&file);
	DC_FlushRange(buffer, mSize);
	if (size != NULL)
		*size = mSize;
	return buffer;
}*/

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

void Util_LoadTextureFromCard(const char* texPath, const char* plttPath, NNSGfdTexKey &texKey, NNSGfdPlttKey &plttKey)
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

void Util_LoadLZ77TextureFromCard(const char* texPath, const char* plttPath, NNSGfdTexKey &texKey, NNSGfdPlttKey &plttKey)
{
	uint32_t size;
	void* buffer = Util_LoadLZ77FileToBuffer(texPath, &size, TRUE);
	DC_FlushRange(buffer, size);

	texKey = NNS_GfdAllocTexVram(size, FALSE, 0);
	Util_LoadTextureWithKey(texKey, buffer);
	NNS_FndFreeToExpHeap(gHeapHandle, buffer);

	buffer = Util_LoadLZ77FileToBuffer(plttPath, &size, TRUE);
	DC_FlushRange(buffer, size);

	plttKey = NNS_GfdAllocPlttVram(size, FALSE, 0);
	Util_LoadPaletteWithKey(plttKey, buffer);
	NNS_FndFreeToExpHeap(gHeapHandle, buffer);
}

void Util_LoadLHTextureFromCard(const char* texPath, const char* plttPath, NNSGfdTexKey &texKey, NNSGfdPlttKey &plttKey)
{
	uint32_t size;
	void* buffer = Util_LoadLHFileToBuffer(texPath, &size, TRUE);
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

void Util_LoadTexture4x4FromCard(const char* texPath, const char* texIndexPath, const char* plttPath, NNSGfdTexKey &texKey, NNSGfdPlttKey &plttKey)
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

void Util_LoadLZ77Texture4x4FromCard(const char* texPath, const char* texIndexPath, const char* plttPath, NNSGfdTexKey &texKey, NNSGfdPlttKey &plttKey)
{
	uint32_t size;
	void* buffer = Util_LoadLZ77FileToBuffer(texPath, &size, TRUE);
	DC_FlushRange(buffer, size);
	void* buffer2 = Util_LoadLZ77FileToBuffer(texIndexPath, NULL, TRUE);
	DC_FlushRange(buffer2, size >> 1);

	texKey = NNS_GfdAllocTexVram(size, TRUE, 0);
	Util_LoadTexture4x4WithKey(texKey, buffer, buffer2);
	NNS_FndFreeToExpHeap(gHeapHandle, buffer2);
	NNS_FndFreeToExpHeap(gHeapHandle, buffer);

	buffer = Util_LoadLZ77FileToBuffer(plttPath, &size, TRUE);
	DC_FlushRange(buffer, size);

	plttKey = NNS_GfdAllocPlttVram(size, FALSE, 0);
	Util_LoadPaletteWithKey(plttKey, buffer);
	NNS_FndFreeToExpHeap(gHeapHandle, buffer);
}

void Util_LoadLHTexture4x4FromCard(const char* texPath, const char* texIndexPath, const char* plttPath, NNSGfdTexKey &texKey, NNSGfdPlttKey &plttKey)
{
	uint32_t size;
	void* buffer = Util_LoadLHFileToBuffer(texPath, &size, TRUE);
	DC_FlushRange(buffer, size);
	void* buffer2 = Util_LoadLHFileToBuffer(texIndexPath, NULL, TRUE);
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

void Util_DrawSprite(fx32 x, fx32 y, fx32 z, fx32 width, fx32 height)
{
	G3_PushMtx();
	{
		G3_Translate(x + (width >> 1), y + (height >> 1), z);
		G3_Scale(width, height, FX32_ONE);

		G3_Begin(GX_BEGIN_QUADS);
		{
			G3_TexCoord(0, 0);
			G3_Vtx(-FX32_HALF, -FX32_HALF, 0);
			G3_TexCoord(0, height);
			G3_Vtx(-FX32_HALF, FX32_HALF, 0);
			G3_TexCoord(width, height);
			G3_Vtx(FX32_HALF, FX32_HALF, 0);
			G3_TexCoord(width, 0);
			G3_Vtx(FX32_HALF, -FX32_HALF, 0);
		}
		G3_End();
	}
	G3_PopMtx(1);
}

void Util_DrawSpriteScaled(fx32 x, fx32 y, fx32 z, fx32 width, fx32 height, fx32 scale)
{
	G3_PushMtx();
	{
		G3_Translate(x + (width >> 1), y + (height >> 1), z);
		G3_Scale(width, height, FX32_ONE);
		G3_Scale(scale, scale, FX32_ONE);

		G3_Begin(GX_BEGIN_QUADS);
		{
			G3_TexCoord(0, 0);
			G3_Vtx(-FX32_HALF, -FX32_HALF, 0);
			G3_TexCoord(0, height);
			G3_Vtx(-FX32_HALF, FX32_HALF, 0);
			G3_TexCoord(width, height);
			G3_Vtx(FX32_HALF, FX32_HALF, 0);
			G3_TexCoord(width, 0);
			G3_Vtx(FX32_HALF, -FX32_HALF, 0);
		}
		G3_End();
	}
	G3_PopMtx(1);
}

void VEC_Lerp(VecFx32* a, VecFx32* b, fx32 t, VecFx32* result)
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

void Util_SetupBillboardMatrix()
{
	static u32 bbcmd1[] =
	{
		((G3OP_MTX_POP << 0) |
		(G3OP_MTX_MODE << 8) |
			(G3OP_MTX_LOAD_4x3 << 16) |
			(G3OP_MTX_SCALE << 24)),
		1,
		GX_MTXMODE_POSITION_VECTOR,
		FX32_ONE, 0, 0,
		0, FX32_ONE, 0,
		0, 0, FX32_ONE,
		0, 0, 0,   // This is subject to change  (Trans)
		0, 0, 0    // This is subject to change  (Scale)
	};

	VecFx32* trans = (VecFx32*)&bbcmd1[12];
	VecFx32* scale = (VecFx32*)&bbcmd1[15];
	MtxFx44 m;

	// command  transmission :
	// change to PROJ mode
	// save the projection matrix
	// set the unit matrix
	reg_G3X_GXFIFO = ((G3OP_MTX_MODE << 0) |
		(G3OP_MTX_PUSH << 8) |
		(G3OP_MTX_IDENTITY << 16));
	reg_G3X_GXFIFO = (u32)GX_MTXMODE_PROJECTION;
	reg_G3X_GXFIFO = 0; // 2004/08/26 geometry fifo glitch

						// Wait further for the geometry engine to stop
						// get the current matrix
	while (G3X_GetClipMtx(&m))
		;

	// billboard matrix calculation
	trans->x = m._30;
	trans->y = m._31;
	trans->z = m._32;

	scale->x = VEC_Mag((VecFx32*)&m._00);
	scale->y = VEC_Mag((VecFx32*)&m._10);
	scale->z = VEC_Mag((VecFx32*)&m._20);

	// projection matrix POP
	// return to POS_VEC
	// store in the current matrix
	// multiply the calculated scale
	MI_CpuSend32(&bbcmd1[0],
		&reg_G3X_GXFIFO,
		18 * sizeof(u32));
}

void Util_SetupBillboardYMatrix()
{
	MtxFx44 m;
	static u32 bbcmd1[] =
	{
		((G3OP_MTX_POP << 0) |
		(G3OP_MTX_MODE << 8) |
			(G3OP_MTX_LOAD_4x3 << 16) |
			(G3OP_MTX_SCALE << 24)),
		1,
		GX_MTXMODE_POSITION_VECTOR,
		FX32_ONE, 0, 0, // This is subject to change  (4x3Mtx)
		0, FX32_ONE, 0,
		0, 0, FX32_ONE,
		0, 0, 0,
		0, 0, 0    // This is subject to change  (Scale)
	};
	VecFx32* trans = (VecFx32*)&bbcmd1[12];
	VecFx32* scale = (VecFx32*)&bbcmd1[15];
	MtxFx43* mtx = (MtxFx43*)&bbcmd1[3];
	reg_G3X_GXFIFO = ((G3OP_MTX_MODE << 0) |
		(G3OP_MTX_PUSH << 8) |
		(G3OP_MTX_IDENTITY << 16));
	reg_G3X_GXFIFO = (u32)GX_MTXMODE_PROJECTION;
	reg_G3X_GXFIFO = 0; // 2004/08/26 geometry fifo glitch

						// Wait further for the geometry engine to stop
						// get the current matrix (clip matrix)
	while (G3X_GetClipMtx(&m));
	trans->x = m._30;
	trans->y = m._31;
	trans->z = m._32;

	// 2: approximate the scale with the size of the vector on each line
	scale->x = VEC_Mag((VecFx32*)&m._00);
	scale->y = VEC_Mag((VecFx32*)&m._10);
	scale->z = VEC_Mag((VecFx32*)&m._20);

	// 3: the rotation  matrix will be the rotation matrix for the X-axis as a the result, so
	//    set an appropriate value
	if (m._11 != 0 || m._12 != 0)
	{
		VEC_Normalize((VecFx32*)&m._10, (VecFx32*)&mtx->_10);

		mtx->_21 = -mtx->_12;
		mtx->_22 = mtx->_11;
	}
	else
	{
		VEC_Normalize((VecFx32*)&m._20, (VecFx32*)&mtx->_20);

		mtx->_12 = -mtx->_21;
		mtx->_11 = mtx->_22;
	}
	MI_CpuSend32(&bbcmd1[0], &reg_G3X_GXFIFO, 18 * sizeof(u32));
}