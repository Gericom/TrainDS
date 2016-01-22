#ifndef __CORE_H__
#define __CORE_H__
#include <nnsys/fnd.h>

typedef u8 uint8_t;
typedef u16 uint16_t;
typedef u32 uint32_t;

typedef s8 int8_t;
typedef s16 int16_t;
typedef s32 int32_t;

#define MKTAG(a0,a1,a2,a3) ((uint32)((a0) | ((a1) << 8) | ((a2) << 16) | ((a3) << 24))) 

#define NOCASH_Break() asm("mov r11, r11")

extern NNSFndHeapHandle mHeapHandle;

void Core_Init();

void* operator new(std::size_t blocksize);
void* operator new[](std::size_t blocksize);
void operator delete(void *block) throw();
void operator delete[](void *block) throw();

#endif