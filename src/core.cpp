#include <nitro.h>
#include <nnsys/fnd.h>
#include <nnsys/gfd.h>
#include <nnsys/snd.h>
#include "core.h"

GXOamAttr gOamTmpBuffer[128];

NNSFndHeapHandle mHeapHandle;

#define SYSTEM_HEAP_SIZE        64*1024

#define ROUND_UP(value, alignment) \
    (((u32)(value) + (alignment-1)) & ~(alignment-1))

#define ROUND_DOWN(value, alignment) \
    ((u32)(value) & ~(alignment-1))

void Core_PreInit()
{
	void* sysHeapMemory = OS_AllocFromMainArenaLo(SYSTEM_HEAP_SIZE, 16);
    u32 arenaLow = ROUND_UP(OS_GetMainArenaLo(), 16);
    u32 arenaHigh = ROUND_DOWN(OS_GetMainArenaHi(), 16);
    u32 appHeapSize = arenaHigh - arenaLow;
    void* appHeapMemory = OS_AllocFromMainArenaLo(appHeapSize, 16);
    mHeapHandle = NNS_FndCreateExpHeap(appHeapMemory, appHeapSize);
}

void* operator new(std::size_t blocksize)
{
    void* block = NNS_FndAllocFromExpHeapEx(mHeapHandle, blocksize, 16);
	MI_CpuClear32(block, blocksize);
	return block;
}

void* operator new[](std::size_t blocksize)
{
    void* block = NNS_FndAllocFromExpHeapEx(mHeapHandle, blocksize, 16);
	MI_CpuClear32(block, blocksize);
	return block;
}

void operator delete(void* block) throw()
{
    NNS_FndFreeToExpHeap(mHeapHandle, block);
}

void operator delete[](void* block) throw()
{
    NNS_FndFreeToExpHeap(mHeapHandle, block);
}

#define SOUND_HEAP_SIZE 0x80000

NNSSndArc mSndArc;

static u8 sndHeap[SOUND_HEAP_SIZE];
NNSSndHeapHandle mSndHeapHandle;

#define STREAM_THREAD_PRIO 10

void Core_Init()
{
	NNS_SndInit();
    mSndHeapHandle = NNS_SndHeapCreate(&sndHeap[0], sizeof(sndHeap));
    NNS_SndArcInit(&mSndArc, "/data/sound_data.sdat", mSndHeapHandle, FALSE);
    NNS_SndArcPlayerSetup(mSndHeapHandle);
	NNS_SndArcStrmInit(STREAM_THREAD_PRIO, mSndHeapHandle);

	//If we do it like this, we can only use 2 vram blocks for textures in the whole game
	//Should I do this different?
	uint32_t szWork = NNS_GfdGetLnkTexVramManagerWorkSize( 4096 );
    void* pMgrWork = NNS_FndAllocFromExpHeapEx(mHeapHandle, szWork, 16);
    NNS_GfdInitLnkTexVramManager(256 * 1024, 128 * 1024, pMgrWork, szWork, TRUE);
	
	szWork = NNS_GfdGetLnkPlttVramManagerWorkSize( 4096 );
    pMgrWork = NNS_FndAllocFromExpHeapEx(mHeapHandle, szWork, 16);
    NNS_GfdInitLnkPlttVramManager(64 * 1024, pMgrWork, szWork, TRUE);
}