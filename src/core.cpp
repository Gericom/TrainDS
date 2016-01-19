#include <nitro.h>
#include <nnsys/fnd.h>
#include "core.h"

NNSFndHeapHandle mHeapHandle;

#define SYSTEM_HEAP_SIZE        64*1024

#define ROUND_UP(value, alignment) \
    (((u32)(value) + (alignment-1)) & ~(alignment-1))

#define ROUND_DOWN(value, alignment) \
    ((u32)(value) & ~(alignment-1))

void Core_Init()
{
	void* sysHeapMemory = OS_AllocFromMainArenaLo(SYSTEM_HEAP_SIZE, 16);
    u32   arenaLow      = ROUND_UP  (OS_GetMainArenaLo(), 16);
    u32   arenaHigh     = ROUND_DOWN(OS_GetMainArenaHi(), 16);
    u32   appHeapSize   = arenaHigh - arenaLow;
    void* appHeapMemory = OS_AllocFromMainArenaLo(appHeapSize, 16);
    mHeapHandle      = NNS_FndCreateExpHeap(appHeapMemory, appHeapSize);
}

void* operator new ( std::size_t blocksize )
{
    void* block = NNS_FndAllocFromExpHeapEx(mHeapHandle, blocksize, 16);//OS_AllocFromHeap( ARENA_ID, HEAP_ID, blocksize );
	MI_CpuClear32(block, blocksize);
	return block;
}

void* operator new[] ( std::size_t blocksize )
{
    void* block = NNS_FndAllocFromExpHeapEx(mHeapHandle, blocksize, 16);//return OS_AllocFromHeap( ARENA_ID, HEAP_ID, blocksize );
	MI_CpuClear32(block, blocksize);
	return block;
}

void operator delete ( void* block ) throw()
{
    NNS_FndFreeToExpHeap(mHeapHandle, block);//OS_FreeToHeap( ARENA_ID, HEAP_ID, block );
}

void operator delete[] ( void* block ) throw()
{
    NNS_FndFreeToExpHeap(mHeapHandle, block);//OS_FreeToHeap( ARENA_ID, HEAP_ID, block );
}