#ifndef __UTIL_H__
#define __UTIL_H__
#include <nnsys/gfd.h>

void* Util_LoadFileToBuffer(char* path, uint32_t* size, BOOL tempoarly);
void* Util_LoadLZ77FileToBuffer(char* path, uint32_t* size, BOOL tempoarly);
void Util_LoadTextureWithKey(NNSGfdTexKey key, void* data);
void Util_LoadTexture4x4WithKey(NNSGfdTexKey key, void* data, void* indexData);
void Util_LoadTextureFromCard(char* texPath, char* plttPath, NNSGfdTexKey &texKey, NNSGfdPlttKey &plttKey);
void Util_LoadTexture4x4FromCard(char* texPath, char* texIndexPath, char* plttPath, NNSGfdTexKey &texKey, NNSGfdPlttKey &plttKey);
void Util_LoadPaletteWithKey(NNSGfdPlttKey key, void* data);

void Util_FreeAllToExpHeapByGroupId(NNSFndHeapHandle heap, int groupId);
void FX_Lerp(VecFx32* a, VecFx32* b, fx32 t, VecFx32* result);

void Util_OBJ1DToTex(uint8_t* pOBJData, int objWidth, int objHeight, uint8_t* pTexData, int texWidth, int texHeight);
void Util_SetupSubOAMForDouble3D();

#endif