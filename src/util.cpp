#include <nitro.h>
#include "core.h"
#include "util.h"

void* Util_LoadFileToBuffer(char* path, uint32_t* size)
{
	FSFile file;
	FS_InitFile(&file);
	FS_OpenFile(&file, path);
	*size = FS_GetLength(&file);
	void* buffer = NNS_FndAllocFromExpHeapEx(mHeapHandle, *size, 16);
	FS_ReadFile(&file, buffer, (int)*size);
	FS_CloseFile(&file);
	return buffer;
}