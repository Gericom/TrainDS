#include <nitro.h>
#include "core.h"
#include "util.h"

void* Util_LoadFileToBuffer(char* path, uint32_t* size)
{
	FSFile file;
	FS_InitFile(&file);
	FS_OpenFile(&file, path);
	uint32_t mSize = FS_GetLength(&file);
	void* buffer = NNS_FndAllocFromExpHeapEx(mHeapHandle, mSize, 16);
	FS_ReadFile(&file, buffer, (int)mSize);
	FS_CloseFile(&file);
	if(size != NULL) *size = mSize;
	return buffer;
}