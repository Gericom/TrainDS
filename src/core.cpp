#include <nitro.h>
#include <nnsys/fnd.h>
#include <nnsys/g3d.h>
#include <nnsys/gfd.h>
#include "core.h"
#include "util.h"

GXOamAttr gOamTmpBuffer[128];

NNSFndHeapHandle gHeapHandle;

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
	gHeapHandle = NNS_FndCreateExpHeap(appHeapMemory, appHeapSize);
}

void* operator new(std::size_t blocksize)
{
    void* block = NNS_FndAllocFromExpHeapEx(gHeapHandle, blocksize, 16);
	MI_CpuClear32(block, blocksize);
	return block;
}

void* operator new[](std::size_t blocksize)
{
    void* block = NNS_FndAllocFromExpHeapEx(gHeapHandle, blocksize, 16);
	MI_CpuClear32(block, blocksize);
	return block;
}

void operator delete(void* block) throw()
{
    NNS_FndFreeToExpHeap(gHeapHandle, block);
}

void operator delete[](void* block) throw()
{
    NNS_FndFreeToExpHeap(gHeapHandle, block);
}

#define SOUND_HEAP_SIZE 0x80000

NNSSndArc gSndArc;

static u8 mSndHeap[SOUND_HEAP_SIZE];
NNSSndHeapHandle gSndHeapHandle;

#define STREAM_THREAD_PRIO 10

static uint32_t mSzWork;
static void* mPMgrWork;

void Core_Init()
{
	NNS_SndInit();
	gSndHeapHandle = NNS_SndHeapCreate(&mSndHeap[0], sizeof(mSndHeap));
	NNS_SndArcInit(&gSndArc, "/data/sound_data.sdat", gSndHeapHandle, FALSE);
	NNS_SndArcPlayerSetup(gSndHeapHandle);
	NNS_SndArcStrmInit(STREAM_THREAD_PRIO, gSndHeapHandle);

	//If we do it like this, we can only use 2 vram blocks for textures in the whole game
	//Should I do this different?
	mSzWork = NNS_GfdGetLnkTexVramManagerWorkSize(4096);
	mPMgrWork = NNS_FndAllocFromExpHeapEx(gHeapHandle, mSzWork, 16);
	//NNS_GfdInitLnkTexVramManager(256 * 1024, 128 * 1024, mPMgrWork, mSzWork, TRUE);
	NNS_GfdInitLnkTexVramManager(/*256*/128 * 1024, 0, mPMgrWork, mSzWork, TRUE);

	uint32_t szWork = NNS_GfdGetLnkPlttVramManagerWorkSize(4096);
	void* pMgrWork = NNS_FndAllocFromExpHeapEx(gHeapHandle, szWork, 16);
	NNS_GfdInitLnkPlttVramManager(64 * 1024, pMgrWork, szWork, TRUE);
}

uint16_t gKeys;

void Core_ReadInput()
{
	gKeys = PAD_Read();
}