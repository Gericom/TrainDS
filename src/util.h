#ifndef __UTIL_H__
#define __UTIL_H__
#include <nnsys/gfd.h>

void* Util_LoadFileToBuffer(char* path, uint32_t* size, BOOL tempoarly);
void Util_LoadTextureWithKey(NNSGfdTexKey key, void* data);
void Util_LoadPaletteWithKey(NNSGfdPlttKey key, void* data);
void Util_FreeAllToExpHeapByGroupId(NNSFndHeapHandle heap, int groupId);

#endif