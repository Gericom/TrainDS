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
    void* block = NNS_FndAllocFromExpHeapEx(gHeapHandle, blocksize, 32);
	MI_CpuClear32(block, blocksize);
	return block;
}

void* operator new[](std::size_t blocksize)
{
    void* block = NNS_FndAllocFromExpHeapEx(gHeapHandle, blocksize, 32);
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

typedef struct
{
	// Status information (4Byte)
	u16     state;                     // WM status information
	u16     BusyApiid;                 // Busy APIID

									   // Flag (32Byte) 36
	BOOL    apiBusy;                   // API busy flag (according to FIFO, API issue prohibit flag)
	BOOL    scan_continue;             // Scan continue flag (As of 07/19/2004 not used)
	BOOL    mp_flag;                   // MP running flag
	BOOL    dcf_flag;                  // DCF running flag
	BOOL    ks_flag;                   // KeySharing running flag
	BOOL    dcf_sendFlag;              // Waiting for DCF send flag
	BOOL    VSyncFlag;                 // VBlank sync OK flag

									   // Version information (16Byte) 52
	u8      wlVersion[WM_SIZE_WL_VERSION];      // WL version
	u16     macVersion;                // MAC version
	u16     rfVersion;                 // RF version
	u16     bbpVersion[WM_SIZE_BBP_VERSION / sizeof(u16)];      // BBP version

																// MP related (106Byte) 158
	u16     mp_parentSize;             // send size in one MP communique by the parent
	u16     mp_childSize;              // send size in one MP communique by the child
	u16     mp_parentMaxSize;          // maximum send size in one MP communique by the parent
	u16     mp_childMaxSize;           // maximum send size in one MP communique by the child
	u16     mp_sendSize;               // maximum send size with current connection
	u16     mp_recvSize;               // maximum receive size with current connection
	u16     mp_maxSendSize;            // maximum send size with current connection (parentMaxSize+4 or childMaxSize+2)
	u16     mp_maxRecvSize;            // maximum receive size with current connection (childMaxSize+2 or parentMaxSize+4)
	u16     mp_parentVCount;           // MP operation start VCount on the parent side
	u16     mp_childVCount;            // MP operation start VCount on the child side
	u16     mp_parentInterval;         // MP communications interval used by the parent (us)
	u16     mp_childInterval;          // MP communications interval used by the child (us)

	OSTick  mp_parentIntervalTick;     // MP communications interval used by the parent (tick)
	OSTick  mp_childIntervalTick;      // MP communications interval used by the child (tick)

	u16     mp_minFreq;                // Number of MPs in one picture frame (lowest value)(un-mounted)
	u16     mp_freq;                   // Number of MPs in one picture frame (preparation value)
	u16     mp_maxFreq;                // Number of MPs in one picture frame (maximum value)

	u16     mp_vsyncOrderedFlag;       // Whether the next MP was instructed by the parent to synchronize to vsync
	u16     mp_vsyncFlag;              // Whether the next MP is performed by synchronizing to vsync
	s16     mp_count;                  // How many more MP for the current picture frame?
	s16     mp_limitCount;             // How many times to MP in the current picture frame, including the current resend
	u16     mp_resumeFlag;             // Resume running flag
	u16     mp_prevPollBitmap;         // PollBitmap in MP immediately before
	u16     mp_prevWmHeader;           // wmHeader in MP immediately before
	u16     mp_prevTxop;               // txop in previous MP
	u16     mp_prevDataLength;         // dataLength in previous MP
	u16     mp_recvBufSel;             // MP receive buffer selection flag
	u16     mp_recvBufSize;            // Size of MP Reception buffer
	WMMpRecvBuf *mp_recvBuf[2];        // Pointer to the MP receive buffer
	u32    *mp_sendBuf;                // Pointer to the MP send buffer
	u16     mp_sendBufSize;            // Size of the MP send buffer

	u16     mp_ackTime;                // MPACK estimated time of arrival
	u16     mp_waitAckFlag;            // Waiting for MPACK

	u16     mp_readyBitmap;            // child bitmap with MP preparations complete (for parent)

	u16     mp_newFrameFlag;           // flag that becomes TRUE when a new picture frame comes
	u16     mp_setDataFlag;            // flag with reply data set complete (for child)
	u16     mp_sentDataFlag;           // flag with reply data sent (for child)
	u16     mp_bufferEmptyFlag;        // buffer empty flag for reply data (for child)
	u16     mp_isPolledFlag;           // Was previous MP polled by itself? (for child)

	u16     mp_minPollBmpMode;         // mode that minimizes to fit the send target of the packet that sends PollBitmap
	u16     mp_singlePacketMode;       // mode that only sends 1 packet in 1 MP
	u16     mp_fixFreqMode;            // mode that prohibits increase of MP communication by retry
	u16     mp_defaultRetryCount;      // default number of retries for communication on a port that does not do Seq control
	u16     mp_ignoreFatalErrorMode;   // Whether to ignore FatalError
	u16     mp_ignoreSizePrecheckMode; // Invalidates the preliminary check of the send / receive size when starting communications

	u16     mp_pingFlag;               // flag that is enabled at the timing that ping is hit
	u16     mp_pingCounter;            // counter of number of frames remaining until ping

									   // DCF related (26Byte) 184
	u8      dcf_destAdr[WM_SIZE_MACADDR];       // DCF send destination MAC address
	u16    *dcf_sendData;              // DCF send data pointer
	u16     dcf_sendSize;              // DCF send data size
	u16     dcf_recvBufSel;            // DCF receive buffer select flag
	WMDcfRecvBuf *dcf_recvBuf[2];      // DCF receive buffer pointer
	u16     dcf_recvBufSize;           // DCF receive buffer size

	u16     curr_tgid;                 // TGID of the parent that is currently connected (used with BeaconRecv.Ind)

									   // RSSI related (4Byte) 206
	u16     linkLevel;
	u16     minRssi;
	u16     rssiCounter;

	// Additional parameters
	u16     beaconIndicateFlag;        // Notification enable flag related to beacon
	u16     wepKeyId;                  // WEP key ID (0-3)
	u16     pwrMgtMode;                // Power management mode
	u8      reserved_d[4];

	u16     VSyncBitmap;               // Bitmap of children that have acquired V-Blank sync
	u16     valarm_queuedFlag;         // V-Alarm processes have accumulated in the run queue.

									   // V-Blank sync related (14Byte) 218
	u32     v_tsf;                     // For V-Blank sync. V_TSF value
	u32     v_tsf_bak;                 // For V-Blank sync. Last acquired V_TSF value
	u32     v_remain;                  // For V-Blank sync. Remaining adjustment
	u16     valarm_counter;            // For V-Alarm interrupt generation counter

									   // Read wait flag (2Byte) 220
									   //    u16             indbuf_read_wait;       // Indication buffer read wait flag

	u8      reserved_e[2];

	// Status of the self (8Byte) 228
	u8      MacAddress[WM_SIZE_MACADDR];
	u16     mode;                      // Parent: WL_CMDLABEL_MODE_PARENT, Child: WL_CMDLABEL_MODE_CHILD

									   // Information to be used when it is parent (64 + 90 + 2 = 156Byte) 384
	WMParentParam pparam;              // Parent parameter
	u8      childMacAddress[WM_NUM_MAX_CHILD][WM_SIZE_MACADDR];
	u16     child_bitmap;              // Connection status of child (used as poll bitmap)

									   // Information to be used when it is a child (14Byte) 398
	WMBssDesc *pInfoBuf;               // Pointer to the buffer that is used for storing information regarding the parent that was scanned. (given as an argument ARM9API)
	u16     aid;                       // Current AID
	u8      parentMacAddress[WM_SIZE_MACADDR];
	u16     scan_channel;              // Channel that is currently scanned

	u8      reserved_f[4];

	// WEP related (86Byte) 488
	u16     wepMode;                   // WEP mode (how many bits is WEP?)
	BOOL    wep_flag;                  // Indicates whether other party is requesting WEP
	u16     wepKey[WM_SIZE_WEPKEY / sizeof(u16)];       // WEP key (20Byte * 4)
														/* Placed wepKeyId about 40 lines above this. (2005/02/07 terui)*/

														// Other (4Byte) 492
	u16     rate;                      // Utilization rate(WL_CMDLABEL_RATE_AUTO, 1M, 2M)
	u16     preamble;                  // Long:0, Short:1

									   // Various parameters (Planning that user will not be allowed to touch this) (4Byte) 496
	u16     tmptt;
	u16     retryLimit;

	// Enabled channels (2Byte) 498
	u16     enableChannel;

	// usable channels (2Byte) 500
	u16     allowedChannel;

	// port related (1316+256+20+4+256 Bytes) 1816+256+20+4+256
	u16     portSeqNo[WM_NUM_MAX_CHILD + 1][WM_NUM_OF_SEQ_PORT];        // 16*8*2=256Byte

	WMPortSendQueueData sendQueueData[WM_SEND_QUEUE_NUM];       // 32*32=1024Byte
	WMPortSendQueue sendQueueFreeList; // 4Byte
	WMPortSendQueue sendQueue[WM_PRIORITY_LEVEL];       // 16Byte
	WMPortSendQueue readyQueue[WM_PRIORITY_LEVEL];      // 16Byte
	OSMutex sendQueueMutex;            // 20 byte
	BOOL    sendQueueInUse;            // MP send wait flag

									   // Null Ack timeout ( 128 + 8 Byte )
	OSTick  mp_lastRecvTick[1 + WM_NUM_MAX_CHILD];
	OSTick  mp_lifeTimeTick;

}
WMStatus_old;

static WMStatus sTmpWMStatus;

extern "C" WMErrCode WZ_ReadStatus(WMStatus_old* statusBuf)
{
	WMErrCode result = WM_ReadStatus(&sTmpWMStatus);
	*statusBuf = *((WMStatus_old*)&sTmpWMStatus);
	return result;
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
	mSzWork = NNS_GfdGetLnkTexVramManagerWorkSize(512);
	mPMgrWork = NNS_FndAllocFromExpHeapEx(gHeapHandle, mSzWork, 16);
	//NNS_GfdInitLnkTexVramManager(256 * 1024, 128 * 1024, mPMgrWork, mSzWork, TRUE);
	NNS_GfdInitLnkTexVramManager(/*256*/128 * 1024, 0, mPMgrWork, mSzWork, TRUE);

	uint32_t szWork = NNS_GfdGetLnkPlttVramManagerWorkSize(1024);
	void* pMgrWork = NNS_FndAllocFromExpHeapEx(gHeapHandle, szWork, 16);
	NNS_GfdInitLnkPlttVramManager(64 * 1024, pMgrWork, szWork, TRUE);

	NNS_G3dGeSetBuffer(&sGeBuffer);
	NNS_G3dGeUseFastDma(true);

	DWC_SetReportLevel(DWC_REPORTFLAG_ALL);// (unsigned long)(DWC_REPORTFLAG_ALL & ~DWC_REPORTFLAG_QR2_REQ));
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

uint16_t gOldKeys;
uint16_t gKeys;

void Core_ReadInput()
{
	gOldKeys = gKeys;
	gKeys = PAD_Read();
}

void Core_GetTouchInput(TPData* dst)
{
	*dst = mTPData[TP_GetLatestIndexInAuto()];
}