#include <nitro.h>
#include <nnsys/fnd.h>
#include <nnsys/g3d.h>
#include <nnsys/gfd.h>
#include <dwc.h>
#include "core.h"
#include "util.h"

#include <nitro/dtcm_begin.h>
static NNSG3dGeBuffer sGeBuffer;
#include <nitro/dtcm_end.h>

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

extern "C" void* mb_alloc(int size)
{
	void* result;
	OSIntrMode cur = OS_DisableInterrupts();
	{
		int curHeapGroup = NNS_FndGetGroupIDForExpHeap(gHeapHandle);
		NNS_FndSetGroupIDForExpHeap(gHeapHandle, WFS_HEAP_GROUP_ID);
		{			
			result = NNS_FndAllocFromExpHeapEx(gHeapHandle, size, 16);
		}
		NNS_FndSetGroupIDForExpHeap(gHeapHandle, curHeapGroup);
	}
	OS_RestoreInterrupts(cur);
	return result;
}

#define SOUND_HEAP_SIZE 0x80000

NNSSndArc gSndArc;

static u8 mSndHeap[SOUND_HEAP_SIZE];
NNSSndHeapHandle gSndHeapHandle;

#define STREAM_THREAD_PRIO 10

static uint32_t mSzWork;
static void* mPMgrWork;

static TPData mTPData[4];

//compatibility patch for dwc to work with old nitrosystem

typedef struct NNSiG2dCharCanvasVTable
{
	NNSiG2dDrawGlyphFunc    pDrawGlyph;
	NNSiG2dClearFunc        pClear;
	NNSiG2dClearAreaFunc    pClearArea;
}
NNSiG2dCharCanvasVTable;

static NNSiG2dCharCanvasVTable VTABLE_BG = { NULL };
//{ DrawGlyphLine, ClearContinuous, ClearAreaLine };
static NNSiG2dCharCanvasVTable VTABLE_OBJ1D = { NULL };
//{ DrawGlyph1D, ClearContinuous, ClearArea1D };

extern "C" void FIX_G2dCharCanvasInitForBG(NNSG2dCharCanvas* pCC, void* charBase, int areaWidth, int areaHeight, NNSG2dCharaColorMode colorMode)
{
	u32 save[2] = { (u32)pCC->pClear, (u32)pCC->pClearArea };
	NNS_G2dCharCanvasInitForBG(pCC, charBase, areaWidth, areaHeight, colorMode);
	VTABLE_BG.pDrawGlyph = pCC->pDrawGlyph;
	VTABLE_BG.pClear = pCC->pClear;
	VTABLE_BG.pClearArea = pCC->pClearArea;
	pCC->pDrawGlyph = (NNSiG2dDrawGlyphFunc)&VTABLE_BG;
	pCC->pClear = (NNSiG2dClearFunc)save[0];
	pCC->pClearArea = (NNSiG2dClearAreaFunc)save[1];
}

extern "C" void FIX_G2dCharCanvasInitForOBJ1D(NNSG2dCharCanvas* pCC, void* charBase, int areaWidth, int areaHeight, NNSG2dCharaColorMode colorMode)
{
	u32 save[2] = { (u32)pCC->pClear, (u32)pCC->pClearArea };
	NNS_G2dCharCanvasInitForOBJ1D(pCC, charBase, areaWidth, areaHeight, colorMode);
	VTABLE_OBJ1D.pDrawGlyph = pCC->pDrawGlyph;
	VTABLE_OBJ1D.pClear = pCC->pClear;
	VTABLE_OBJ1D.pClearArea = pCC->pClearArea;
	pCC->pDrawGlyph = (NNSiG2dDrawGlyphFunc)&VTABLE_OBJ1D;
	pCC->pClear = (NNSiG2dClearFunc)save[0];
	pCC->pClearArea = (NNSiG2dClearAreaFunc)save[1];
}

extern "C" void FIX_G2dCharCanvasDrawChar(const NNSG2dCharCanvas* pCC, const NNSG2dFont* pFont, int x, int y, int cl, u16 ccode)
{
	NNSG2dCharCanvas clone;
	MI_CpuCopy32(pCC, &clone, sizeof(NNSG2dCharCanvas));
	NNSiG2dCharCanvasVTable* vt = (NNSiG2dCharCanvasVTable*)pCC->pDrawGlyph;
	clone.pDrawGlyph = vt->pDrawGlyph;
	clone.pClear = vt->pClear;
	clone.pClearArea = vt->pClearArea;
	NNS_G2dCharCanvasDrawChar(&clone, pFont, x, y, cl, ccode);
}

extern "C" void FIXi_G2dTextCanvasDrawTextRect(const NNSG2dTextCanvas* pTxn, int x, int y, int w, int h, int cl, u32 flags, const void* txt)
{
	NNSG2dCharCanvas clone;
	MI_CpuCopy32(pTxn->pCanvas, &clone, sizeof(NNSG2dCharCanvas));
	NNSiG2dCharCanvasVTable* vt = (NNSiG2dCharCanvasVTable*)pTxn->pCanvas->pDrawGlyph;
	clone.pDrawGlyph = vt->pDrawGlyph;
	clone.pClear = vt->pClear;
	clone.pClearArea = vt->pClearArea;
	NNSG2dTextCanvas clone2;
	MI_CpuCopy32(pTxn, &clone2, sizeof(NNSG2dTextCanvas));
	clone2.pCanvas = &clone;
	NNSi_G2dTextCanvasDrawTextRect(&clone2, x, y, w, h, cl, flags, txt);
}

extern "C" void FIXi_G2dTextCanvasDrawText(const NNSG2dTextCanvas* pTxn, int x, int y, int cl, u32 flags, const void* txt)
{
	NNSG2dCharCanvas clone;
	MI_CpuCopy32(pTxn->pCanvas, &clone, sizeof(NNSG2dCharCanvas));
	NNSiG2dCharCanvasVTable* vt = (NNSiG2dCharCanvasVTable*)pTxn->pCanvas->pDrawGlyph;
	clone.pDrawGlyph = vt->pDrawGlyph;
	clone.pClear = vt->pClear;
	clone.pClearArea = vt->pClearArea;
	NNSG2dTextCanvas clone2;
	MI_CpuCopy32(pTxn, &clone2, sizeof(NNSG2dTextCanvas));
	clone2.pCanvas = &clone;
	NNSi_G2dTextCanvasDrawText(&clone2, x, y, cl, flags, txt);
}

static int IrqAppli;
static OSIrqFunction FuncAppliVBlank;

extern "C" void DWCi_ITRlIntr();
extern "C" void DWCi_TSKlAct(int a);

void intrVBlank()
{
	DWCi_TSKlAct(1);
	OS_SetIrqCheckFlag(OS_IE_V_BLANK);
}

void DWCi_ITRlIntrPatch()
{
	IrqAppli = reg_OS_IE;
	OS_SetIrqMask(0x40018);
	OS_EnableIrqMask(1);
	FuncAppliVBlank = OS_GetIrqFunction(1);
	OS_SetIrqFunction(1, intrVBlank);
	OS_ResetRequestIrqMask(1);
	reg_OS_IME = 1;
	OS_EnableInterrupts();
}

asm void DWCi_ITRlIntrPatch2()
{
	ldr r12, = DWCi_ITRlIntrPatch
	bx r12
}

extern "C" void DWCi_ITRlEnd();

void DWCi_ITRlEndPatch()
{
	OSIntrMode old = OS_DisableInterrupts();
	OS_SetIrqMask(IrqAppli);
	OS_SetIrqFunction(1, FuncAppliVBlank);
	OS_RestoreInterrupts(old);
}

asm void DWCi_ITRlEndPatch2()
{
	ldr r12,= DWCi_ITRlEndPatch
	bx r12
}

static void* dwcAllocFunc(DWCAllocType name, u32 size, int align)
{
	void * ptr;
	OSIntrMode old;
	(void)name;
	(void)align;

	old = OS_DisableInterrupts();

	ptr = NNS_FndAllocFromExpHeapEx(gHeapHandle, size, align);

	OS_RestoreInterrupts(old);

	return ptr;
}

/*---------------------------------------------------------------------------*
Memory release function
*---------------------------------------------------------------------------*/
static void dwcFreeFunc(DWCAllocType name, void* ptr, u32 size)
{
	OSIntrMode old;
	(void)name;
	(void)size;

	if (!ptr) return;

	old = OS_DisableInterrupts();

	NNS_FndFreeToExpHeap(gHeapHandle, ptr);

	OS_RestoreInterrupts(old);
}

void Core_Init()
{
	//patch dwc utility library
	MI_CpuCopy32(&DWCi_ITRlIntrPatch2, &DWCi_ITRlIntr, 3 * 4);
	MI_CpuCopy32(&DWCi_ITRlEndPatch2, &DWCi_ITRlEnd, 3 * 4);

	NNS_SndInit();
	Core_SetupSndArc();

	//If we do it like this, we can only use 2 vram blocks for textures in the whole game
	//Should I do this different?
	mSzWork = NNS_GfdGetLnkTexVramManagerWorkSize(4096);
	mPMgrWork = NNS_FndAllocFromExpHeapEx(gHeapHandle, mSzWork, 16);
	//NNS_GfdInitLnkTexVramManager(256 * 1024, 128 * 1024, mPMgrWork, mSzWork, TRUE);
	NNS_GfdInitLnkTexVramManager(/*256*/128 * 1024, 0, mPMgrWork, mSzWork, TRUE);

	uint32_t szWork = NNS_GfdGetLnkPlttVramManagerWorkSize(4096);
	void* pMgrWork = NNS_FndAllocFromExpHeapEx(gHeapHandle, szWork, 16);
	NNS_GfdInitLnkPlttVramManager(64 * 1024, pMgrWork, szWork, TRUE);

	NNS_G3dGeSetBuffer(&sGeBuffer);
	NNS_G3dGeUseFastDma(true);

	void* dwcWork = NNS_FndAllocFromExpHeapEx(gHeapHandle, DWC_INIT_WORK_SIZE, 32);
	DWC_Init(dwcWork);
	NNS_FndFreeToExpHeap(gHeapHandle, dwcWork);

	DWC_SetMemFunc(dwcAllocFunc, dwcFreeFunc);

	Core_StartTPSampling();
}

void Core_StartTPSampling()
{
	TP_RequestAutoSamplingStart(192, 2, &mTPData[0], 4);
}

void Core_StopTPSampling()
{
	TP_RequestAutoSamplingStop();
}

void Core_SetupSndArc()
{
	gSndHeapHandle = NNS_SndHeapCreate(&mSndHeap[0], sizeof(mSndHeap));
	NNS_SndArcInit(&gSndArc, "/data/sound_data.sdat", gSndHeapHandle, FALSE);
	NNS_SndArcPlayerSetup(gSndHeapHandle);
	NNS_SndArcStrmInit(STREAM_THREAD_PRIO, gSndHeapHandle);
}

uint16_t gKeys;

void Core_ReadInput()
{
	gKeys = PAD_Read();
}

void Core_GetTouchInput(TPData* dst)
{
	*dst = mTPData[TP_GetLatestIndexInAuto()];
}