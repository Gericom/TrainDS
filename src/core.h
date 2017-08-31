#ifndef __CORE_H__
#define __CORE_H__
#include <nitro.h>
#include <nnsys/fnd.h>
#include <nnsys/snd.h>
#include <nnsys/g2d.h>
#include <nnsys/g3d.h>
#include "../resources/sound/sdat/sound_data.sadl"
#include "print.h"

#define WFS_HEAP_GROUP_ID	0x5B

#define SDK_MAKEGGID_SYSTEM(num)    (0x003FFF00 | (num))
#define WH_GGID		SDK_MAKEGGID_SYSTEM(0x42)

#define PORT_WFS                  4

#define LOAD_OVERLAY_ITCM(name)	\
	do {	\
		int default_dma = FS_GetDefaultDMA();	\
		FS_SetDefaultDMA(FS_DMA_NOT_USE);	\
		FS_EXTERN_OVERLAY(name);	\
		FS_LoadOverlay(MI_PROCESSOR_ARM9, FS_OVERLAY_ID(name));	\
		FS_SetDefaultDMA(default_dma);	\
	} while (false);

//fix error in include (; removed)
#undef GX_FX32_FX10_MAX
#undef GX_FX32_FX10_MIN

#define GX_FX32_FX10_MAX          (fx32)(0x00000fff)
#define GX_FX32_FX10_MIN          (fx32)(0xfffff000)

typedef u8 uint8_t;
typedef u16 uint16_t;
typedef u32 uint32_t;
typedef u64 uint64_t;

typedef s8 int8_t;
typedef s16 int16_t;
typedef s32 int32_t;
typedef s64 int64_t;

#define MKTAG(a0,a1,a2,a3) ((u32)((a0) | ((a1) << 8) | ((a2) << 16) | ((a3) << 24))) 

#define NOCASH_Break() __asm { mov r11, r11 }

extern GXOamAttr gOamTmpBuffer[128];

extern NNSFndHeapHandle gHeapHandle;

void Core_PreInit();

#ifdef __cplusplus

void* operator new(std::size_t blocksize);
void* operator new[](std::size_t blocksize);
void operator delete(void *block) throw();
void operator delete[](void *block) throw();

//#define tmpnew new (NNS_FndAllocFromExpHeapEx(gHeapHandle, blocksize, -16));

#endif

#ifdef __cplusplus
extern "C" {
#endif

	void* mb_alloc(int size);

#ifdef __cplusplus
}
#endif

extern NNSSndArc gSndArc;
extern NNSSndHeapHandle gSndHeapHandle;

void Core_Init();

void Core_StartTPSampling();
void Core_StopTPSampling();

void Core_SetupSndArc();

extern uint16_t gOldKeys;
extern uint16_t gKeys;
void Core_ReadInput();
void Core_GetTouchInput(TPData* dst);

#endif