#include <nitro.h>
#include <nnsys/gfd.h>
#include "core.h"
#include "util.h"

void* Util_LoadFileToBuffer(char* path, uint32_t* size)
{
	FSFile file;
	FS_InitFile(&file);
	FS_OpenFile(&file, path);
	uint32_t mSize = FS_GetLength(&file);
	void* buffer = NNS_FndAllocFromExpHeapEx(mHeapHandle, mSize, 16);
	if(buffer != NULL)
		FS_ReadFile(&file, buffer, (int)mSize);
	FS_CloseFile(&file);
	if(size != NULL) *size = mSize;
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

void Util_LoadPaletteWithKey(NNSGfdPlttKey key, void* data)
{
	GX_BeginLoadTexPltt();
	{
		GX_LoadTexPltt(data, NNS_GfdGetPlttKeyAddr(key), NNS_GfdGetPlttKeySize(key));
	}
	GX_EndLoadTexPltt();
}