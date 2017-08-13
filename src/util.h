#ifndef __UTIL_H__
#define __UTIL_H__
#include <nnsys/gfd.h>

void* Util_LoadFileToBuffer(const char* path, uint32_t* size, bool tempoarly);
void* Util_LoadLZ77FileToBuffer(const char* path, uint32_t* size, bool tempoarly);
void Util_LoadTextureWithKey(NNSGfdTexKey key, void* data);
void Util_LoadTexture4x4WithKey(NNSGfdTexKey key, void* data, void* indexData);
void Util_LoadTextureFromCard(const char* texPath, const char* plttPath, NNSGfdTexKey &texKey, NNSGfdPlttKey &plttKey);
void Util_LoadTexture4x4FromCard(const char* texPath, const char* texIndexPath, const char* plttPath, NNSGfdTexKey &texKey, NNSGfdPlttKey &plttKey);
void Util_LoadLZ77TextureFromCard(const char* texPath, const char* plttPath, NNSGfdTexKey &texKey, NNSGfdPlttKey &plttKey);
void Util_LoadLZ77Texture4x4FromCard(const char* texPath, const char* texIndexPath, const char* plttPath, NNSGfdTexKey &texKey, NNSGfdPlttKey &plttKey);
void Util_LoadPaletteWithKey(NNSGfdPlttKey key, void* data);

void Util_FreeAllToExpHeapByGroupId(NNSFndHeapHandle heap, int groupId);

void Util_DrawSprite(fx32 x, fx32 y, fx32 z, fx32 width, fx32 height);
void Util_DrawSpriteScaled(fx32 x, fx32 y, fx32 z, fx32 width, fx32 height, fx32 scale);

void VEC_Lerp(VecFx32* a, VecFx32* b, fx32 t, VecFx32* result);

void Util_SetupSubOAMForDouble3D();

void Util_SetupBillboardMatrix();
void Util_SetupBillboardYMatrix();

#endif