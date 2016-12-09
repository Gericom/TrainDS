#ifndef __CORE_H__
#define __CORE_H__
#include <nnsys/fnd.h>
#include <nnsys/snd.h>
#include <nnsys/g2d.h>
#include "../resources/sound/sdat/sound_data.sadl"
#include "print.h"

typedef u8 uint8_t;
typedef u16 uint16_t;
typedef u32 uint32_t;

typedef s8 int8_t;
typedef s16 int16_t;
typedef s32 int32_t;

#define MKTAG(a0,a1,a2,a3) ((uint32)((a0) | ((a1) << 8) | ((a2) << 16) | ((a3) << 24))) 

#define NOCASH_Break() asm("mov r11, r11")

extern GXOamAttr gOamTmpBuffer[128];

extern NNSFndHeapHandle gHeapHandle;

void Core_PreInit();

#ifdef __cplusplus

void* operator new(std::size_t blocksize);
void* operator new[](std::size_t blocksize);
void operator delete(void *block) throw();
void operator delete[](void *block) throw();

#endif


extern NNSSndArc gSndArc;
extern NNSSndHeapHandle gSndHeapHandle;

void Core_Init();

extern uint16_t gKeys;
void Core_ReadInput();

#endif