/*
  Project:  NitroSDK - wireless_shared - demos - wh
  File:     wh.c

  Copyright 2003-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: wh.c,v $
  Revision 1.70  2007/10/19 05:12:41  yosizaki
  Fix about auto-reconnecting of WC.
  Fixed WcFinish().

  Revision 1.69  2007/10/10 00:22:13  yosizaki
  Fix about DataSharing-child.

  Revision 1.68  2007/10/09 08:56:40  seiki_masashi
  Revised sBssDesc to hold an instance which will be used as a cache-safe region.

  Revision 1.67  2007/10/04 11:35:24  okubata_ryoma
  Small fix

  Revision 1.66  2007/10/04 03:07:11  seiki_masashi
  Changed to use WM_StartScanEx.

  Revision 1.65  2007/10/04 01:51:29  seiki_masashi
  Changed the macAddr argument so that it will accept NULL.
  Corrected an state inconsistency related to automatic connections when WC was used.

  Revision 1.64  2007/10/03 05:38:37  yosizaki
  Added a WC-compatible layer.

  Revision 1.63  2007/10/02 09:06:05  yosizaki
  Small fix specific to const-cast.

  Revision 1.62  2007/04/16 02:14:45  seiki_masashi
  Fixed comment errors.

  Revision 1.61  2007/02/20 00:28:08  kitase_hirotake
  Indented source

  Revision 1.60  2006/10/26 06:56:01  kitase_hirotake
  Added WH_SetIndCallback

  Revision 1.59  2006/10/25 02:03:19  kitase_hirotake
  Made change so that calls to WH_Initialize are not duplicated

  Revision 1.58  2006/08/29 03:39:56  yosizaki
  Fixed WH_StateOutStartChildMP

  Revision 1.57  2006/08/17 10:46:43  okubata_ryoma
  Corrected disconnect process area of DataSharing

  Revision 1.56  2006/08/14 12:27:38  seiki_masashi
  Corrected bug when WH_ChildConnectAuto is cancelled with WH_EndScan

  Revision 1.55  2006/07/19 09:07:33  yosizaki
  Added WH_SetSsid.

  Revision 1.54  2006/07/19 04:08:33  yosizaki
  Fixed comments.

  Revision 1.53  2006/07/06 11:58:02  okubata_ryoma
  undo

  Revision 1.51  2006/06/14 01:42:20  okubata_ryoma
  Added a process for calling the WM_EndDataSharing function when resetting

  Revision 1.50  2006/04/10 13:19:21  yosizaki
  Added support for WH_MP_FREQUENCY.

  Revision 1.49  2006/03/13 07:11:28  yosizaki
  Fixed assertion code.

  Revision 1.48  2006/02/20 08:05:00  seiki_masashi
  Changed code to use WM_IsValidGameInfo function

  Revision 1.47  2006/02/20 02:37:17  seiki_masashi
  Removed WMGameInfo.version and added magicNumber and ver

  Revision 1.46  2006/01/18 02:12:39  kitase_hirotake
  do-indent

  Revision 1.45  2006/01/10 02:51:04  adachi_hiroaki
  All hexadecimal notation output was explicitly labeled with 0x. /([^xX])(%\d*[xX][^hH])/ -> \10x\2

  Revision 1.44  2005/11/28 05:00:46  seiki_masashi
  Added compatibility with the WM_SetMPParameter function addition

  Revision 1.43  2005/11/01 00:37:18  adachi_hiroaki
  Parts added to WM_STATECODE have also been added to the display table.

  Revision 1.42  2005/10/28 11:25:35  seiki_masashi
  Supported addition of WM_STATECODE_DISCONNECTED_FROM_MYSELF.
  Supported addition of WM_STATECODE_PORT_INIT.

  Revision 1.41  2005/07/29 03:44:26  seiki_masashi
  Added to WM_STATECODE_INFORMATION

  Revision 1.40  2005/06/27 11:10:11  yosizaki
  Added comment about DC_WaitWriteBufferEmpty().

  Revision 1.39  2005/06/07 05:45:48  seiki_masashi
  Revised to reduce special processing related to key sharing

  Revision 1.38  2005/04/26 02:35:02  terui
  Fixed comment

  Revision 1.37  2005/03/25 03:05:23  seiki_masashi
  Added WH_Set{Parent,Child}WEPKeyGenerator to set WEPKey setting functions

  Revision 1.36  2005/03/04 09:26:10  takano_makoto
  Addition of an MB flag check for the parent beacon

  Revision 1.35  2005/03/04 09:18:56  takano_makoto
  Addition of WH_SetUserGameInfo

  Revision 1.34  2005/02/28 05:26:35  yosizaki
  do-indent.

  Revision 1.33  2005/02/18 10:38:04  yosizaki
  Fixes related to hidden warnings.

  Revision 1.32  2005/02/08 01:22:38  takano_makoto
  Fixed a bug in determining the success or failure of WH_SetReceiver

  Revision 1.31  2005/01/11 07:48:59  takano_makoto
  Fixed copyright header.

  Revision 1.30  2005/01/07 12:04:47  terui
  Added processing for receiving unrecoverable error notification.

  Revision 1.29  2005/01/07 06:15:26  takano_makoto
  Added the WM_StartConnect callback error handling.

  Revision 1.28  2004/12/22 02:48:43  terui
  Support for Pictochat Search

  Revision 1.27  2004/12/20 08:31:27  takano_makoto
  Made changes to shorten the initialization process by using WH_Initialize.

  Revision 1.26  2004/12/20 07:17:20  takano_makoto
  Changed WHReceiver to WHReceiverFunc, excluded the internal use of OS_Alloc, separated parameters to set for each application as wh_config.h.

  Revision 1.25  2004/12/09 08:14:00  takano_makoto
  Made correction so that a random number is used when there are multiple channels with the lowest electric wave usage rate.

  Revision 1.24  2004/11/26 02:38:09  takano_makoto
  Changed so that when a connection fails the state transitions to WM_SYSSTATE_CONNECT_FAIL.

  Revision 1.23  2004/11/16 03:01:11  takano_makoto
  Added the WH_GetCurrentAid function.

  Revision 1.22  2004/11/11 03:50:58  seiki_masashi
  Corrected the aidBitmap argument of WM_StartDataSharing on the child side

  Revision 1.21  2004/11/10 06:13:38  takano_makoto
  Corrected the WM_EndKeySharing success/failure determination to WM_ERRCODE_OPERATING

  Revision 1.20  2004/11/08 02:46:17  takano_makoto
  Prevented multiple calling of WM_EndScan

  Revision 1.19  2004/11/05 04:27:40  sasakis
  Added the lobby screen and parent selection screen, and made the necessary modifications (such as scan related).

  Revision 1.18  2004/11/02 19:36:19  takano_makoto
  Changed the return values of WH_StartMeasureChannel and WH_EndScan to BOOL

  Revision 1.17  2004/11/02 10:08:05  takano_makoto
  Changed the timing for changing the state of WH_Finalize

  Revision 1.16  2004/11/02 10:03:10  seiki_masashi
  Corrected a problem when terminating during a scan

  Revision 1.15  2004/11/01 09:32:12  takano_makoto
  Changed the debug output

  Revision 1.14  2004/11/01 09:19:57  takano_makoto
  Reviewed the WH state transition

  Revision 1.13  2004/11/01 06:28:10  seiki_masashi
  Corrected comment

  Revision 1.12  2004/11/01 02:48:56  takano_makoto
  Moved the position where the state is changed in WH_StateInInitialize. Deleted the sequence for skipping WM_EndMP with WH_StateInParentMP.

  Revision 1.11  2004/10/29 07:27:56  takano_makoto
  Changed the internal state change position

  Revision 1.10  2004/10/29 02:35:30  takano_makoto
  Debug output correction
  
  Revision 1.9  2004/10/29 02:16:34  takano_makoto
  Changed the WH_StartScan argument
  
  Revision 1.8  2004/10/29 02:05:28  takano_makoto
  Changed the Scan function so it is externally accessible and a parent list can be created
  
  Revision 1.7  2004/10/28 10:38:31  terui
  Corrected the end processing portion for the parent.
  Corrected the error check processing in the callback portion for WM_StartConnect.

  Revision 1.6  2004/10/28 07:11:20  takano_makoto
  Changed the WH_Connect function name

  Revision 1.5  2004/10/27 02:31:31  takano_makoto
  Integrated the multiple MeasureChannel-related functions
  Fixed a bug that was using WM_ERRCODE_SUCCESS to determine a section that should wait on WM_ERRCODE_OPERATING
  Fixed other bugs

  Revision 1.4  2004/10/25 05:43:33  seiki_masashi
  Updated the character string conversion function for WM_APIID_* to match the latest wm.h
  Added processing for when WM_GetAllowedChannel returns 0x800
  Comment change

  Revision 1.3  2004/10/25 02:18:17  seiki_masashi
  Corrected a problem in calculating the receiving buffer size
  Made the maximum send size for the parent and child into WH_{PARENT,CHILD}_MAX_SIZE
  Modified aidBitmap for WM_StartDataSharing to reflect WH_CHILD_MAX

  Revision 1.2  2004/10/22 07:36:55  sasakis
  The error display character string was insufficient, so increased it.

  Revision 1.1  2004/10/21 00:41:50  yosizaki
  Initial upload.

  $NoKeywords$
 */

#include <nitro.h>
#include <nitro/wm.h>
#include <nitro/cht.h>

#include "wh.h"

/*
  wh.c : Wireless Manager related Wrapper

  -Internal description for users

  The three communication methods implemented in WM API, ordered starting with the least specialized, are:

  - General MP communication
  - Data sharing
  - Key sharing

  
  Each of these are not exactly separate: for example, key sharing is one of the methods that use data sharing, and data sharing is a shared data system that simply uses MP communication. However, for the sake of convenience they are described separately.
  
  
  

  "General MP communication" refers to a communication mode at the Multi Poll protocol level, and can be summarized as performing communication with the following steps as a single cycle.
  

  (1) Broadcasting of data by the parent to all the children
  (2) Replying from each child
  (3) The parent broadcasts notification that the communication cycle has ended.

  

  Data sharing is the mode in which data is extracted by the parent from all children connected to it (processing in (2) above); all that data, including the data of the parent, is then gathered together and distributed to all children as shared data (processing in (1) above).
  
  
  

  In key sharing, the data that is shared in data sharing becomes a key data array on each device.
  


  1) Overview of communication processing

  The specific operations of features accomplished using wireless LAN are actually carried out by the ARM7 processor.
  
  All send/receive requests and the like from the ARM9 thus need to pass through the ARM7; as a result, nearly all ARM9 WM APIs are asynchronous. Generally speaking, basic processing follows the flow below.
  

  1. A function for writing ARM7 requests to the FIFO is called
  2. The callback function (specified in 1.) is invoked by the system receiving from the FIFO the result that the ARM7 returned.
  

  

  In this sample implementation, the two functions required for this process are considered to express, as a set, the Wireless LAN functionality "state." By creating a chain of these "states", communication processing is carried out (hereafter, the expression "WH state" will be used to indicate the "state" of the Wireless LAN and differentiate it contextually).
  
  
  
  

  The names of functions in this source correspond to their respective WH states, as shown below.

  WH_StateInXXXXXX   - Start function for the WH state XXXXXX.
  WH_StateOutXXXXXX  - Callback function that receives completion notifications for the WH state XXXXXX.

  


  2) Transitions between WH states

  At the end of processing in the WH state callback function, which must automatically transition to the next WH state, the start function for the next WH state is called.
  
  (The state transition basically works from the top to bottom in the source.)

  All the WH state names, and their mutual sequence transition relationships, are as follows.
  

  - Initialization sequence
  (WH_Initialize function)
  
  Initialize

  - Parent communication preparation sequence
  (WH_Connect function)
  
  SetParentParam
  
  StartParent
  
  StartParentMP
  
  (**)
  
  StartParentKeyShare

  Note: If MP connection mode (or DS connection mode) is specified in WH_Connect, end at (**).
  

  - Parent end sequence
  (WH_Finalize function)
  
  (EndParentKeyShare)
  
  EndParentMP
  
  EndParent

  - Child communication preparation sequence
  (WH_Connect function)
  
  StartScan
  
  EndScan
  
  StartChild
  
  StartChildMP
  
  (**)
  
  StartChildKeyShare

  Note: If MP connection mode (or DS connection mode) is specified in WH_Connect, end at (**).
  

  - Child end sequence
  (WH_Finalize function)
  
  (EndChildKeyShare)
  
  EndChildMP
  
  EndChild

  Be careful here, as the callback function for each WH state will not necessarily be called only when that WH state ends.
  

  For example, the WH_StateOutStartParentMP function, which is the callback for the WH state StartParentMP, is called not only when StartMP processing ends, but also roughly when an MP sequence ends (for further details, refer to comments in the function body).
  
  


  3) Actual sending and receiving operations

  In this source, three communications methods are implemented: general MP communications, data sharing, and key sharing. Depending on which of these communications methods is used, the way in which data is sent and received will change entirely.
  
  

  For general MP communication, a callback needs to be set for the functions below at initialization time.
  

  WH_SetReceiver(WHReceiverFunc proc);

  Data is passed to the configured callback function as the data content is received.
  
  Data transmission is executed with WH_SendData.
  Make sure the transmission data memory is not overwritten until the callback set with WH_SendData is called.
  

  With data sharing, if the WH_StepDS function is called and data is passed internal updating will be carried out simultaneously, and data will be fetched with WH_GetSharedDataAdr.
  

  With key sharing communication, key data is fetched with the WH_GetKeySet function.
  (Processing corresponding to WH_StepDS is carried out internally.
  Key data that must be transmitted is sent automatically by the library, so nothing special needs to be done.)


  4) Errors and recovering

  This implementation is formulated as follows.

  - Error determination with the top function is done with a return value.
    Determined with TRUE (success) or FALSE (failure).
  - More detailed error codes are stored in sErrCode.
  - If a state is reached where processing cannot continue (such as when an operation is required), sSysState will change to a WH_SYSSTATE_ERROR, and that will be detected.
    

  
  Depending on the application, it would seem that a callback function or OS_SendMessage needs to be used to report an error, but this is not particularly supported in this implementation.
  
  (It is probably adequate to rewrite the WH_SetError function if using the source elsewhere.)
  

  Also, for "well-behaved" disconnection (recovery) processing, we recommended that you explicitly call WM_Disconnect or the like whenever possible after terminating the state.
  
  In this implementation, the WH_Finalize function has been prepared for performing "well-behaved termination".
  This function can be split off to various termination processing depending on the current state (at least when possible).
  


  5) Guideline compliance

  Comments are attached to code that is required for guideline compliance.
  (Be careful as processing to handle the regulations in the guidelines may also be located outside of this file.
  Look for comments annotated with the expression 'GUIDELINE' in applicable locations.)
  
 */

/* ======================================================================
   static contents
   ====================================================================== */

static WMParentParam sParentParam ATTRIBUTE_ALIGN(32);

/*  The values below are configured throughout the code.
static WMParentParam sParentParam ATTRIBUTE_ALIGN(32) =
{
    Specify user definition data here if to be used in the beacon.
    // When you want to display the parent device's nickname on the parent device selection screen of the child device (or other such occasions), set the information here, and it will be conveyed from the parent device to the child device.
    // 
    NULL,                         // userGameInfo
    0,                            // userGameInfoLength
    0,                            // padding
    // The ggid is used in identifying the game loop.
    // A value allocated by the game is used here.
    0,
    // The tgid is used to identify the temporary group.
    // A different value is needed each time activity is started as the parent device.
    // In this source, it is configured in the WH_Initialize function.
    0x0001,                       // tgid
    //  Designate whether or not to accept the new connection from the child device.
    1,                            // entryFlag
    // Designate the maximum number of connectable child devices
    WH_CHILD_MAX,                 // maxEntry
    //  Set to zero when not doing multiboot
    0,                            // multiBootFlag
    // By setting KS_Flag to 1, the correction for key sharing will automatically be placed in parentMaxSize and childMaxSize at connection time.
    // 
    // Even when using key sharing, a zero is OK if the parentMaxSize and childMaxSize have been properly calculated.
    // 
    0,                            // KS_Flag
    // In order to maintain compatibility with past versions, this is currently ignored in the prepared fields.
    0,                            // CS_Flag
    The interval of the beacon sent by the parent. [ms]
    // At actual startup, values that have been properly dispersed by WM_GetDispersionBeaconPeriod() are used, but they are approximately 200[ms].
    // 
    200,                          // beaconPeriod (millisec)
    // This is a reserved region
    {0},                           // rsv1
    {0},                           // rsv2
    // This is the wireless channel where the parent device waits for the child device.
    WH_DEFAULT_CHANNEL ,          // channel
    // Appropriate values have been set as maximum values in the members below
    // parentMaxSize - the maximum size of the data the parent device sends
    WH_PARENT_MAX_SIZE,
    // childMaxSize - the maximum data size the child device sends
    WH_CHILD_MAX_SIZE,
};
*/

// The wh state (This is different from the WH state. It can only be referenced externally.)
static int sSysState = WH_SYSSTATE_STOP;

// Connection mode (set externally, not touched in this source)
static int sConnectMode;

// The data receiving user function in MP communication mode
static WHReceiverFunc sReceiverFunc;

// User function for determining connection permission
static WHJudgeAcceptFunc sJudgeAcceptFunc;

// Own aid enters (The child may change during disconnect or reconnect)
static u16 sMyAid;

// Stores the bitmap showing the connection state
static u16 sConnectBitmap;

// For storing the error code
static int sErrCode;

// For random numbers
static u32 sRand;

/* For determining the communications usage rate (used in WH_StartMeasureChannel and WH_GetMeasureChannel) */
static u16 sChannel;
static u16 sChannelBusyRatio;
static u16 sChannelBitmap;

/* System work region buffer for WM */
static u8 sWmBuffer[WM_SYSTEM_BUF_SIZE] ATTRIBUTE_ALIGN(32);
// Send and receive buffer used internally and its size
static u8 sSendBuffer[MATH_MAX(WH_PARENT_SEND_BUFFER_SIZE, WH_CHILD_SEND_BUFFER_SIZE)]
ATTRIBUTE_ALIGN(32);
static u8 sRecvBuffer[MATH_MAX(WH_PARENT_RECV_BUFFER_SIZE, WH_CHILD_RECV_BUFFER_SIZE)]
ATTRIBUTE_ALIGN(32);
static s32 sSendBufferSize;
static s32 sRecvBufferSize;


/* For parent device search */
static WMBssDesc sBssDesc ATTRIBUTE_ALIGN(32);
static u8 sScanBuf[WM_SIZE_SCAN_EX_BUF] ATTRIBUTE_ALIGN(32);
static WMScanExParam sScanExParam ATTRIBUTE_ALIGN(32);
static WHStartScanCallbackFunc sScanCallback;
static u16 sChannelIndex;
static u16 sAutoConnectFlag;

static BOOL sPictoCatchFlag = FALSE;

/* Settings used for the parent connection */
static u8 sConnectionSsid[WM_SIZE_CHILD_SSID];

/* For WEP settings */
static WHParentWEPKeyGeneratorFunc sParentWEPKeyGenerator = NULL;
static WHChildWEPKeyGeneratorFunc sChildWEPKeyGenerator = NULL;
static u16 sWEPKey[20 / sizeof(u16)] ATTRIBUTE_ALIGN(32);

/* For data sharing */
static WMDataSharingInfo sDSInfo ATTRIBUTE_ALIGN(32);
static WMDataSet sDataSet ATTRIBUTE_ALIGN(32);
static WMKeySetBuf sWMKeySetBuf ATTRIBUTE_ALIGN(32);

/* WH initialization flag */
static u16 whInitialized = 0;

/* Callback for Indication notification of WM */
static WMCallbackFunc sIndicationCallback = NULL;

/* debug output hook function */
static void (*wh_trace) (const char *, ...) =
#if !defined(SDK_FINALROM)
        OS_TPrintf;
#else
        NULL;
#endif


// * WH members, extended for compatibility with WC.
static void (*sStateCallback)(int state, int prev) = NULL;
static BOOL sDataSharingDoubleMode = TRUE;
static WMCallbackFunc sPortExtraCallback = NULL;

#if defined(WM_DEMO_WIRELESS_SHARED_WC_WH_CONFIG_H_)
// Members to preserve behavior that is identical to WC.

typedef struct WHCompatibleInfoForWC
{
    WMParentParam               parentParam ATTRIBUTE_ALIGN(32);
    OSThreadQueue               queue[1];
    WCPictoCatchCallbackFunc    pictoCatchCallback;
    BOOL                        useDS;
    WCStatus                    wcStatus;
    WCStatus                    wcTarget;
    u16                         framePeriod;
    u16                         targetChannel;
    u16                         tmpTgid;
    u8                          padding[2];
}
WHCompatibleInfoForWC;

static WHCompatibleInfoForWC infoForWC[1] ATTRIBUTE_ALIGN(32);

static const int    parentRecvBufferSize = WH_PARENT_RECV_BUFFER_SIZE;
static const int    parentSendBufferSize = WH_PARENT_SEND_BUFFER_SIZE;
static const int    childRecvBufferSize = WH_CHILD_RECV_BUFFER_SIZE;
static const int    childSendBufferSize = WH_CHILD_SEND_BUFFER_SIZE;
#undef  WH_PARENT_RECV_BUFFER_SIZE
#define WH_PARENT_RECV_BUFFER_SIZE  parentRecvBufferSize
#undef  WH_PARENT_SEND_BUFFER_SIZE
#define WH_PARENT_SEND_BUFFER_SIZE  parentSendBufferSize
#undef  WH_CHILD_RECV_BUFFER_SIZE
#define WH_CHILD_RECV_BUFFER_SIZE   childRecvBufferSize
#undef  WH_CHILD_SEND_BUFFER_SIZE
#define WH_CHILD_SEND_BUFFER_SIZE   childSendBufferSize
#undef  WH_PLAYER_MAX
#define WH_PLAYER_MAX             (infoForWC->parentParam.maxEntry + 1)
#undef  WH_CHILD_MAX
#define WH_CHILD_MAX              infoForWC->parentParam.maxEntry
#undef  WH_DS_DATA_SIZE
#define WH_DS_DATA_SIZE           infoForWC->parentParam.childMaxSize
#undef  WH_PARENT_MAX_SIZE
#define WH_PARENT_MAX_SIZE        infoForWC->parentParam.parentMaxSize
#undef  WH_CHILD_MAX_SIZE
#define WH_CHILD_MAX_SIZE         infoForWC->parentParam.childMaxSize
#undef  WH_MP_FREQUENCY
#define WH_MP_FREQUENCY           (u16)(infoForWC->parentParam.CS_Flag ? 0 : 1)

#endif // WM_DEMO_WIRELESS_SHARED_WC_WH_CONFIG_H_ 


/* ======================================================================
   state functions
   ====================================================================== */

// Random number macro
#define RAND()  ( sRand = sRand * 69069UL + 12345 )
#define RAND_INIT(x) ( sRand = (u32)(x) )

#define WH_MAX(a, b) ( ((a) > (b)) ? (a) : (b) )

/* Function that receives irregular notifications */
static void WH_IndicateHandler(void *arg);

/* (stateless) -> READY -> STOP -> IDLE */
static BOOL WH_StateInInitialize(void);

#ifndef WH_USE_DETAILED_INITIALIZE
static void WH_StateOutInitialize(void *arg);
#else
static BOOL WH_StateInEnable(void);
static void WH_StateOutEnable(void *arg);
static BOOL WH_StateInPowerOn(void);
static void WH_StateOutPowerOn(void *arg);
#endif

/* IDLE -> MEASURECHANNEL -> IDLE */
static u16 WH_StateInMeasureChannel(u16 channel);
static void WH_StateOutMeasureChannel(void *arg);
static WMErrCode WHi_MeasureChannel(WMCallbackFunc func, u16 channel);
static s16 SelectChannel(u16 bitmap);


/* IDLE -> PARENT -> MP-PARENT */
static BOOL WH_StateInSetParentParam(void);
static void WH_StateOutSetParentParam(void *arg);
static BOOL WH_StateInSetParentWEPKey(void);
static void WH_StateOutSetParentWEPKey(void *arg);
static BOOL WH_StateInStartParent(void);
static void WH_StateOutStartParent(void *arg);
static BOOL WH_StateInStartParentMP(void);
static void WH_StateOutStartParentMP(void *arg);

/* IDLE -> SCAN */
static BOOL WH_StateInStartScan(void);
static void WH_StateOutStartScan(void *arg);

/* SCAN -> IDLE */
static BOOL WH_StateInEndScan(void);
static void WH_StateOutEndScan(void *arg);

/* IDLE -> CHILD -> MP-CHILD */
static BOOL WH_StateInSetChildWEPKey(void);
static void WH_StateOutSetChildWEPKey(void *arg);
static BOOL WH_StateInStartChild(void);
static void WH_StateOutStartChild(void *arg);
static BOOL WH_StateInStartChildMP(void);
static void WH_StateOutStartChildMP(void *arg);

/* MP + key sharing */
static BOOL WH_StateInStartParentKeyShare(void);
static BOOL WH_StateInEndParentKeyShare(void);
static BOOL WH_StateInStartChildKeyShare(void);
static BOOL WH_StateInEndChildKeyShare(void);

/* MP + SetMPData */
static BOOL WH_StateInSetMPData(const void *data, u16 datasize, WHSendCallbackFunc callback);
static void WH_StateOutSetMPData(void *arg);
static void WH_PortReceiveCallback(void *arg);

/* MP-PARENT -> IDLE */
static BOOL WH_StateInEndParentMP(void);
static void WH_StateOutEndParentMP(void *arg);
static BOOL WH_StateInEndParent(void);
static void WH_StateOutEndParent(void *arg);
static BOOL WH_StateInDisconnectChildren(u16 bitmap);
static void WH_StateOutDisconnectChildren(void *arg);

/* MP-CHILD -> IDLE */
static BOOL WH_StateInEndChildMP(void);
static void WH_StateOutEndChildMP(void *arg);
static BOOL WH_StateInEndChild(void);
static void WH_StateOutEndChild(void *arg);

/* IDLE -> STOP -> READY -> (stateless) */
static BOOL WH_StateInPowerOff(void);
static void WH_StateOutPowerOff(void *arg);
static BOOL WH_StateInDisable(void);
static void WH_StateOutDisable(void *arg);
static void WH_StateOutEnd(void *arg);

/* X -> IDLE */
static BOOL WH_StateInReset(void);
static void WH_StateOutReset(void *arg);


/* ======================================================================
   debug codes
   ====================================================================== */
#define WMHIGH_DEBUG

#if defined(WMHIGH_DEBUG)

#define WH_TRACE    if(wh_trace) wh_trace

#define WH_TRACE_STATE OS_TPrintf("%s sSysState = %d\n", __func__, sSysState)

#define WH_REPORT_FAILURE(result)                \
    do{ OS_TPrintf("Failed in %s, %s = %s\n",      \
                  __func__,                      \
                  #result,                       \
                  WH_GetWMErrCodeName(result));  \
        WH_SetError(result); }while(0)

#define WH_ASSERT(exp) \
    (void) ((exp) || (OSi_Panic(__FILE__, __LINE__, "Failed assertion " #exp), 0))

#else  /* defined(WMHIGH_DEBUG) */

#define WH_TRACE_STATE                 /* */
#define WH_TRACE(...)               ((void)0)
#define WH_REPORT_FAILURE(result)   WH_SetError(result)
#define WH_ASSERT(exp)              ((void) 0)

#endif



#if defined(WMHIGH_DEBUG)
static const char *sStateNames[] = {
    "WM_STATE_READY",
    "WM_STATE_STOP",
    "WM_STATE_IDLE",
    "WM_STATE_CLASS1",
    "WM_STATE_TESTMODE",
    "WM_STATE_SCAN",
    "WM_STATE_CONNECT",
    "WM_STATE_PARENT",
    "WM_STATE_CHILD",
    "WM_STATE_MP_PARENT",
    "WM_STATE_MP_CHILD",
    "WM_STATE_DCF_CHILD",
};

static const char *WH_GetWMErrCodeName(int result)
{
    static const char *errnames[] = {
        "WM_ERRCODE_SUCCESS",
        "WM_ERRCODE_FAILED",
        "WM_ERRCODE_OPERATING",
        "WM_ERRCODE_ILLEGAL_STATE",
        "WM_ERRCODE_WM_DISABLE",
        "WM_ERRCODE_NO_DATASET",
        "WM_ERRCODE_INVALID_PARAM",
        "WM_ERRCODE_NO_CHILD",
        "WM_ERRCODE_FIFO_ERROR",
        "WM_ERRCODE_TIMEOUT",
        "WM_ERRCODE_SEND_QUEUE_FULL",
        "WM_ERRCODE_NO_ENTRY",
        "WM_ERRCODE_OVER_MAX_ENTRY",
        "WM_ERRCODE_INVALID_POLLBITMAP",
        "WM_ERRCODE_NO_DATA",
        "WM_ERRCODE_SEND_FAILED",
        "WM_ERRCODE_DCF_TEST",
        "WM_ERRCODE_WL_INVALID_PARAM",
        "WM_ERRCODE_WL_LENGTH_ERR",
        "WM_ERRCODE_FLASH_ERROR",

        "WH_ERRCODE_DISCONNECTED",
        "WH_ERRCODE_PARENT_NOT_FOUND",
        "WH_ERRCODE_NO_RADIO",
    };

    if (0 <= result && result < sizeof(errnames) / sizeof(char *))
    {
        return errnames[result];
    }
    else
    {
        return "N/A";
    }
}

static const char *WH_GetWMStateCodeName(u16 code)
{
    static const char *statenames[] = {
        "WM_STATECODE_PARENT_START",
        "N/A",
        "WM_STATECODE_BEACON_SENT",
        "WM_STATECODE_SCAN_START",
        "WM_STATECODE_PARENT_NOT_FOUND",
        "WM_STATECODE_PARENT_FOUND",
        "WM_STATECODE_CONNECT_START",
        "WM_STATECODE_CONNECTED",
        "WM_STATECODE_BEACON_LOST",
        "WM_STATECODE_DISCONNECTED",
        "WM_STATECODE_MP_START",
        "WM_STATECODE_MPEND_IND",
        "WM_STATECODE_MP_IND",
        "WM_STATECODE_MPACK_IND",
        "WM_STATECODE_DCF_START",
        "WM_STATECODE_DCF_IND",
        "WM_STATECODE_BEACON_RECV",
        "WM_STATECODE_DISASSOCIATE",
        "WM_STATECODE_REASSOCIATE",
        "WM_STATECODE_AUTHENTICATE",
        "WM_STATECODE_PORT_SEND",
        "WM_STATECODE_PORT_RECV",
        "WM_STATECODE_FIFO_ERROR",
        "WM_STATECODE_INFORMATION",
        "WM_STATECODE_UNKNOWN",
        "WM_STATECODE_PORT_INIT",
        "WM_STATECODE_DISCONNECTED_FROM_MYSELF",
    };

    if (0 <= code && code < sizeof(statenames) / sizeof(char *))
    {
        return statenames[code];
    }
    else
    {
        return "N/A";
    }
}

static const char *WH_GetWMApiidName(u16 apiid)
{
    static const char *apinames[] = {
        "WM_APIID_INITIALIZE",
        "WM_APIID_RESET",
        "WM_APIID_END",

        "WM_APIID_ENABLE",
        "WM_APIID_DISABLE",
        "WM_APIID_POWER_ON",
        "WM_APIID_POWER_OFF",

        "WM_APIID_SET_P_PARAM",
        "WM_APIID_START_PARENT",
        "WM_APIID_END_PARENT",
        "WM_APIID_START_SCAN",
        "WM_APIID_END_SCAN",
        "WM_APIID_START_CONNECT",
        "WM_APIID_DISCONNECT",
        "WM_APIID_START_MP",
        "WM_APIID_SET_MP_DATA",
        "WM_APIID_END_MP",
        "WM_APIID_START_DCF",
        "WM_APIID_SET_DCF_DATA",
        "WM_APIID_END_DCF",
        "WM_APIID_SET_WEPKEY",
        "WM_APIID_START_KS",
        "WM_APIID_END_KS",
        "WM_APIID_GET_KEYSET",
        "WM_APIID_SET_GAMEINFO",
        "WM_APIID_SET_BEACON_IND",
        "WM_APIID_START_TESTMODE",
        "WM_APIID_STOP_TESTMODE",
        "WM_APIID_VALARM_MP",
        "WM_APIID_SET_LIFETIME",
        "WM_APIID_MEASURE_CHANNEL",
        "WM_APIID_INIT_W_COUNTER",
        "WM_APIID_GET_W_COUNTER",
        "WM_APIID_SET_ENTRY",
        "WM_APIID_AUTO_DEAUTH",
        "WM_APIID_SET_MP_PARAMETER",
        "WM_APIID_SET_BEACON_PERIOD",
        "WM_APIID_AUTO_DISCONNECT",

        "WM_APIID_KICK_MP_PARENT",
        "WM_APIID_KICK_MP_CHILD",
        "WM_APIID_KICK_MP_RESUME",
    };
    static const char *apinames_indicates[] = {
        "WM_APIID_INDICATION",
        "WM_APIID_PORT_SEND",
        "WM_APIID_PORT_RECV",
        "WM_APIID_READ_STATUS"
    };
    static const char *apiname_unknown = "WM_APIID_UNKNOWN";

    if (0 <= apiid && apiid < sizeof(apinames) / sizeof(char *))
    {
        return apinames[apiid];
    }
    else if (WM_APIID_INDICATION <= apiid &&
             apiid < WM_APIID_INDICATION + sizeof(apinames_indicates) / sizeof(char *))
    {
        return apinames_indicates[apiid - WM_APIID_INDICATION];
    }
    else if (apiid == WM_APIID_UNKNOWN)
    {
        return apiname_unknown;
    }
    else
    {
        return "N/A";
    }
}

static void WH_OutputWMState(void)
{
    WMStatus s;

    if (WM_ReadStatus(&s) == WM_ERRCODE_SUCCESS)
    {
        WH_TRACE("state = %s\n", WH_GetWMStateCodeName(s.state));
        WH_TRACE("apiid = %s\n", WH_GetWMApiidName(s.BusyApiid));
    }
    else
    {
        WH_TRACE("WM not Initialized\n");
    }
}

static void WH_OutputBitmap(u16 bitmap)
{
    u16     i;
    for (i = 15; i >= 0; --i)
    {
        if ((bitmap >> i) & 0x01)
        {
            OS_TPrintf("o");
        }
        else
        {
            OS_TPrintf("-");
        }
    }

    OS_TPrintf("\n");
}

static void WH_ChangeSysState(int state)
{
    static const char *statenames[] = {
        "WH_SYSSTATE_STOP",
        "WH_SYSSTATE_IDLE",
        "WH_SYSSTATE_SCANNING",
        "WH_SYSSTATE_BUSY",
        "WH_SYSSTATE_CONNECTED",
        "WH_SYSSTATE_DATASHARING",
        "WH_SYSSTATE_KEYSHARING",
        "WH_SYSSTATE_MEASURECHANNEL",
        "WH_SYSSTATE_CONNECT_FAIL",
        "WH_SYSSTATE_ERROR",
    };
    int     prev = sSysState;
    WH_TRACE("%s -> ", statenames[sSysState]);
    sSysState = state;
    WH_TRACE("%s\n", statenames[sSysState]);
    if (sStateCallback)
    {
        (*sStateCallback)(sSysState, prev);
    }
}

#else

#define WH_GetWMErrCodeName(result)    ("")
#define WH_GetWMStateCodeName(result)  ("")
#define WH_GetWMApiidName(result)      ("")

static void WH_OutputWMState(void)
{;
}
static void WH_OutputBitmap(void)
{;
}

static void WH_ChangeSysState(int state)
{
    sSysState = state;
}

#endif

static void WH_SetError(int code)
{
    // Does not overwrite if the system is already in an ERROR state.
    if (sSysState == WH_SYSSTATE_ERROR || sSysState == WH_SYSSTATE_FATAL)
    {
        return;
    }

    sErrCode = code;
}



/* ======================================================================
   state functions
   ====================================================================== */

/* ----------------------------------------------------------------------
   state : setparentparam
  ---------------------------------------------------------------------- */
static BOOL WH_StateInSetParentParam(void)
{
    // In this state, the game information held by the parent is passed to the ARM7.
    WMErrCode result;
    WH_TRACE_STATE;

    WH_ChangeSysState(WH_SYSSTATE_BUSY);

    result = WM_SetParentParameter(WH_StateOutSetParentParam, &sParentParam);
    if (result != WM_ERRCODE_OPERATING)
    {
        WH_REPORT_FAILURE(result);
        WH_ChangeSysState(WH_SYSSTATE_ERROR);
        return FALSE;
    }

    return TRUE;
}

static void WH_StateOutSetParentParam(void *arg)
{
    WMCallback *cb = (WMCallback *)arg;
    WH_TRACE_STATE;

    if (cb->errcode != WM_ERRCODE_SUCCESS)
    {
        WH_REPORT_FAILURE(cb->errcode);
        WH_ChangeSysState(WH_SYSSTATE_ERROR);
        return;
    }

    if (sParentWEPKeyGenerator != NULL)
    {
        // If the WEP Key Generator is configured, set the WEP Key
        if (!WH_StateInSetParentWEPKey())
        {
            WH_ChangeSysState(WH_SYSSTATE_ERROR);
        }
    }
    else
    {
        // If progress is normal, moves to the StartParent state.
        if (!WH_StateInStartParent())
        {
            WH_ChangeSysState(WH_SYSSTATE_ERROR);
        }
    }
}

static BOOL WH_StateInSetParentWEPKey(void)
{
    u16     wepmode;
    WMErrCode result;
    WH_TRACE_STATE;

    WH_ChangeSysState(WH_SYSSTATE_BUSY);

    wepmode = (*sParentWEPKeyGenerator) (sWEPKey, &sParentParam);
    result = WM_SetWEPKey(WH_StateOutSetParentWEPKey, wepmode, sWEPKey);
    if (result != WM_ERRCODE_OPERATING)
    {
        WH_REPORT_FAILURE(result);
        WH_ChangeSysState(WH_SYSSTATE_ERROR);
        return FALSE;
    }

    return TRUE;
}

static void WH_StateOutSetParentWEPKey(void *arg)
{
    WMCallback *cb = (WMCallback *)arg;
    WH_TRACE_STATE;

    if (cb->errcode != WM_ERRCODE_SUCCESS)
    {
        WH_REPORT_FAILURE(cb->errcode);
        WH_ChangeSysState(WH_SYSSTATE_ERROR);
        return;
    }

    // If progress is normal, moves to the StartParent state.
    if (!WH_StateInStartParent())
    {
        WH_ChangeSysState(WH_SYSSTATE_ERROR);
    }
}


/* ----------------------------------------------------------------------
   state : StartParent
  ---------------------------------------------------------------------- */
static BOOL WH_StateInStartParent(void)
{
    // In this state, the StartParent function is called, and the settings are started for a parent.

    WMErrCode result;
    WH_TRACE_STATE;

    if ((sSysState == WH_SYSSTATE_CONNECTED)
        || (sSysState == WH_SYSSTATE_KEYSHARING) || (sSysState == WH_SYSSTATE_DATASHARING))
    {
        // For the above case, the parent settings should have already been set.
        return TRUE;
    }

    result = WM_StartParent(WH_StateOutStartParent);

    if (result != WM_ERRCODE_OPERATING)
    {
        WH_REPORT_FAILURE(result);
        return FALSE;
    }

    sMyAid = 0;
    sConnectBitmap = WH_BITMAP_EMPTY;

    return TRUE;
}

static void WH_StateOutStartParent(void *arg)
{
    // The callback functions called by StartParent are when:
    // 1) A beacon is sent
    // 2) A new connection is made to a child
    // 3) A StartParent ends
    // 4) A child disconnection is detected
    // Because of all these cases, differentiation is required.

    WMStartParentCallback *cb = (WMStartParentCallback *)arg;
    const u16 target_bitmap = (u16)(1 << cb->aid);

    if (cb->errcode != WM_ERRCODE_SUCCESS)
    {
        WH_REPORT_FAILURE(cb->errcode);
        WH_ChangeSysState(WH_SYSSTATE_ERROR);
        return;
    }

    switch (cb->state)
    {
        //-----------------------------------
        // Beacon transmission notification
    case WM_STATECODE_BEACON_SENT:
        // Ignores the beacon transmission notification.
        break;

        //-----------------------------------
        // Child connection notification
    case WM_STATECODE_CONNECTED:
        {
            // The connected child device MAC address is in cb->macAddress.
            // cb->ssid is the data the child device set in the ssid argument of WM_StartConnect.
            WH_TRACE("StartParent - new child (aid 0x%x) connected\n", cb->aid);
            // Checks whether the newly connected child satisfies the conditions for connection permission
            if (sJudgeAcceptFunc != NULL)
            {
                if (!sJudgeAcceptFunc(cb))
                {
                    WMErrCode result;
                    // Terminates the connection.
                    result = WM_Disconnect(NULL, cb->aid);
                    if (result != WM_ERRCODE_OPERATING)
                    {
                        WH_REPORT_FAILURE(result);
                        WH_ChangeSysState(WH_SYSSTATE_ERROR);
                    }
                    break;
                }
            }
            sConnectBitmap |= target_bitmap;
            if (sPortExtraCallback)
            {
                (*sPortExtraCallback)(arg);
            }
        }
        break;

        //-----------------------------------
        // Notifies of a child disconnection.
    case WM_STATECODE_DISCONNECTED:
        {
            WH_TRACE("StartParent - child (aid 0x%x) disconnected\n", cb->aid);
            // The disconnected child device MAC address is in cb->macAddress
            sConnectBitmap &= ~target_bitmap;
            if (sPortExtraCallback)
            {
                (*sPortExtraCallback)(arg);
            }
        }
        break;

        //-----------------------------------
        // Child has disconnected from itself
    case WM_STATECODE_DISCONNECTED_FROM_MYSELF:
        {
            WH_TRACE("StartParent - child (aid 0x%x) disconnected from myself\n", cb->aid);
            // Does nothing if the unit disconnects from itself
            // The disconnected child device MAC address is in cb->macAddress
        }
        break;

        //-----------------------------------
        // StartParent processing ends
    case WM_STATECODE_PARENT_START:
        {
            // Changes to the MP communication state.
            if (!WH_StateInStartParentMP())
            {
                WH_ChangeSysState(WH_SYSSTATE_ERROR);
            }
        }
        break;

        //-----------------------------------
    default:
        WH_TRACE("unknown indicate, state = %d\n", cb->state);
    }
}

/* ----------------------------------------------------------------------
   state : StartParentMP
  ---------------------------------------------------------------------- */
static BOOL WH_StateInStartParentMP(void)
{
    // Calls the WM_Start function, and starts a connection using the MP communication protocol.

    WMErrCode result;
    WH_TRACE_STATE;

    if ((sSysState == WH_SYSSTATE_CONNECTED)
        || (sSysState == WH_SYSSTATE_KEYSHARING) || (sSysState == WH_SYSSTATE_DATASHARING))
    {
        return TRUE;
    }

    WH_ChangeSysState(WH_SYSSTATE_CONNECTED);
    result = WM_StartMP(WH_StateOutStartParentMP,
                        (u16 *)sRecvBuffer,
                        (u16)sRecvBufferSize, (u16 *)sSendBuffer, (u16)sSendBufferSize,
                        WH_MP_FREQUENCY);

    if (result != WM_ERRCODE_OPERATING)
    {
        WH_REPORT_FAILURE(result);
        return FALSE;
    }

    return TRUE;
}

static void WH_StateOutStartParentMP(void *arg)
{
    // The callback function specified in StartMP is called:
    // 1) When a mode starts up due to StartMP
    // 2) When the MP sequence ends (parent only)
    // 3) During MP reception (child only)
    // 4) When MP sequence completion notification (ACK receiving) is detected (child only)
    // Because there are four cases, differentiation is required.

    WMStartMPCallback *cb = (WMStartMPCallback *)arg;
    // WH_TRACE_STATE;

    if (cb->errcode != WM_ERRCODE_SUCCESS)
    {
        WH_REPORT_FAILURE(cb->errcode);
        WH_ChangeSysState(WH_SYSSTATE_ERROR);
        return;
    }

    switch (cb->state)
    {
    case WM_STATECODE_MP_START:
        // Notification that StartMP has ended normally.
        // Hereafter, send and receive are enabled.
        if (sConnectMode == WH_CONNECTMODE_KS_PARENT)
        {
            // Transition to StartParentKeyShare if key sharing was specified.
            // 
            if (sSysState == WH_SYSSTATE_CONNECTED)
            {
                // Normal MP connection.
                if (!WH_StateInStartParentKeyShare())
                {
                    WH_TRACE("WH_StateInStartParentKeyShare failed\n");
                    WH_ChangeSysState(WH_SYSSTATE_ERROR);
                }
                return;

            }
            else if (sSysState == WH_SYSSTATE_KEYSHARING)
            {
                // Appears that the key sharing state is already set.
                return;
            }
        }
        else if (sConnectMode == WH_CONNECTMODE_DS_PARENT)
        {
            // StartDataSharing is called if data sharing is specified.
            // This function is a synchronous operating function, so a switch to WH state is not made.
            WMErrCode result;
            u16     aidBitmap;

            aidBitmap = (u16)((1 << (WH_CHILD_MAX + 1)) - 1);   // Bitmap with the lower WH_CHILD_MAX+1 bit==1
            result = WM_StartDataSharing(&sDSInfo, WH_DS_PORT, aidBitmap, WH_DS_DATA_SIZE, sDataSharingDoubleMode);

            if (result != WM_ERRCODE_SUCCESS)
            {
                WH_REPORT_FAILURE(result);
                WH_ChangeSysState(WH_SYSSTATE_ERROR);
                return;
            }
            // WH_TRACE("WM_StartDataSharing OK\n");
            WH_ChangeSysState(WH_SYSSTATE_DATASHARING);
            if (sPortExtraCallback)
            {
                (*sPortExtraCallback)(arg);
            }
            return;
        }

        WH_ChangeSysState(WH_SYSSTATE_CONNECTED);
        if (sPortExtraCallback)
        {
            (*sPortExtraCallback)(arg);
        }
        break;

    case WM_STATECODE_MPEND_IND:
        // Parent receiving compete notification.

        // cb->recvBuf accepts the contents of the data frame received from the child device, but set a callback with WM_SetPortCallback for normal data receiving.
        // 
        // When using data sharing or key sharing, data is received internally, so WM_SetPortCallback does not need to be used.
        // 
        break;

    case WM_STATECODE_MP_IND:
    case WM_STATECODE_MPACK_IND:
        // The parent should not come here.

    default:
        WH_TRACE("unknown indicate, state = %d\n", cb->state);
        break;
    }
}

/* ----------------------------------------------------------------------
  state : StartParentKeyShare
  ---------------------------------------------------------------------- */
static BOOL WH_StateInStartParentKeyShare(void)
{
    WMErrCode result;
    WH_TRACE_STATE;
    WH_ChangeSysState(WH_SYSSTATE_KEYSHARING);

    // Calls the WM_StartKeySharing function and starts key sharing.
    result = WM_StartKeySharing(&sWMKeySetBuf, WH_DS_PORT);

    if (result != WM_ERRCODE_SUCCESS)
    {
        WH_REPORT_FAILURE(result);
        return FALSE;
    }

    return TRUE;
}

/* ----------------------------------------------------------------------
   state : EndParentKeyShare
  ---------------------------------------------------------------------- */
static BOOL WH_StateInEndParentKeyShare(void)
{
    // Calls WM_EndKeySharing and ends key sharing.
    WMErrCode result;
    WH_TRACE_STATE;

    result = WM_EndKeySharing(&sWMKeySetBuf);

    if (result != WM_ERRCODE_SUCCESS)
    {
        WH_REPORT_FAILURE(result);
        return FALSE;
    }

    // Automatically starts MP communication termination processing.
    if (!WH_StateInEndParentMP())
    {
        WH_TRACE("WH_StateInEndParentMP failed\n");
        WH_Reset();
        return FALSE;
    }

    return TRUE;
}

/* ----------------------------------------------------------------------
   state : EndParentMP
  ---------------------------------------------------------------------- */
static BOOL WH_StateInEndParentMP(void)
{
    WMErrCode result;
    WH_TRACE_STATE;

    // Hereafter, send and receive are disabled.
    WH_ChangeSysState(WH_SYSSTATE_BUSY);

    result = WM_EndMP(WH_StateOutEndParentMP);
    if (result != WM_ERRCODE_OPERATING)
    {
        WH_REPORT_FAILURE(result);
        return FALSE;
    }

    return TRUE;
}

static void WH_StateOutEndParentMP(void *arg)
{
    WMCallback *cb = (WMCallback *)arg;
    WH_TRACE_STATE;

    if (cb->errcode != WM_ERRCODE_SUCCESS)
    {
        WH_REPORT_FAILURE(cb->errcode);
        WH_Reset();
        return;
    }

    // Automatically starts termination processing.
    if (!WH_StateInEndParent())
    {
        WH_TRACE("WH_StateInEndParent failed\n");
        WH_Reset();
        return;
    }
}

/* ----------------------------------------------------------------------
   state : EndParent
   ---------------------------------------------------------------------- */
static BOOL WH_StateInEndParent(void)
{
    WMErrCode result;
    WH_TRACE_STATE;

    // Operation as a parent ends here.
    // If children are connected, parent activities stop after discontinuing each individual authentication.
    // 
    result = WM_EndParent(WH_StateOutEndParent);
    if (result != WM_ERRCODE_OPERATING)
    {
        WH_REPORT_FAILURE(result);
        return FALSE;
    }

    return TRUE;
}

static void WH_StateOutEndParent(void *arg)
{
    WMCallback *cb = (WMCallback *)arg;
    WH_TRACE_STATE;

    if (cb->errcode != WM_ERRCODE_SUCCESS)
    {
        WH_REPORT_FAILURE(cb->errcode);
        return;
    }

    // At this point, the parent disconnection processing ends, and the state returns to idle (standby).
    WH_ChangeSysState(WH_SYSSTATE_IDLE);
}

/* ----------------------------------------------------------------------
   Name:        WH_ChildConnectAuto
   Description: Starts the child device connection sequence.
                However, each type of setting specified with WH_ParentConnect or WH_ChildConnect is left to automatic internal processing.
                
   Arguments:   mode    - Start MP as the child if WH_CONNECTMODE_MP_CHILD.
                          Start DataSharing as the child if WH_CONNECTMODE_DS_CHILD.
                          Start KeySharing as the child if WH_CONNECTMODE_KS_CHILD.

                macAddr  - Specifies the MAC address for the connected parent
                          If NULL or 0xFFFFFFFFFFFF, all parents are searched.
                          
                channel - Designate the channel that searches for the parent.
                          If 0, all channels are used.
                          
   ---------------------------------------------------------------------- */
BOOL WH_ChildConnectAuto(int mode, const u8 *macAddr, u16 channel)
{
    WH_TRACE_STATE;

    // Calculation of send and receive buffer size for WM_StartMP()
    // To statically allocate a buffer in advance, use the WM_SIZE_MP_* function macro. If the buffer can be allocated dynamically, use the WM_GetReceiveBufferSize() API after the parent and child are connected and immediately before calling WM_StartMP().
    // 
    // 
    // Similarly, to statically allocate a buffer in advance, use the WM_SIZE_MP_* function macro. If the buffer can be allocated dynamically, use the WM_GetSendBufferSize() API after the parent and child are connected and immediately before calling WM_StartMP().
    // 
    // 
    sRecvBufferSize = WH_CHILD_RECV_BUFFER_SIZE;
    sSendBufferSize = WH_CHILD_SEND_BUFFER_SIZE;

    WH_TRACE("recv buffer size = %d\n", sRecvBufferSize);
    WH_TRACE("send buffer size = %d\n", sSendBufferSize);

    WH_ChangeSysState(WH_SYSSTATE_SCANNING);

    // Detection starts in child mode.
    if (macAddr != NULL)
    {
        *(u16 *)(&sScanExParam.bssid[4]) = *(u16 *)(macAddr + 4);
        *(u16 *)(&sScanExParam.bssid[2]) = *(u16 *)(macAddr + 2);
        *(u16 *)(&sScanExParam.bssid[0]) = *(u16 *)(macAddr + 0);
    }
    else
    {
        MI_CpuFill8(&sScanExParam.bssid, 0xff, WM_SIZE_BSSID);
    }

    sConnectMode = mode;

    sScanCallback = NULL;
    sChannelIndex = channel;
    sScanExParam.channelList = 1;
    sAutoConnectFlag = TRUE;

    if (!WH_StateInStartScan())
    {
        WH_ChangeSysState(WH_SYSSTATE_ERROR);
        return FALSE;
    }

    return TRUE;
}


/*---------------------------------------------------------------------------*
  Name:         WH_TurnOnPictoCatch

  Description:  Enables the Pictochat Search feature.
                This will cause the callback function to be invoked even if Pictochat was found while scanning with WH_StartScan.
                

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WH_TurnOnPictoCatch(void)
{
    sPictoCatchFlag = TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         WH_TurnOffPictoCatch

  Description:  Disables the Pictochat Search feature.
                This will cause Pictochat to be ignored even if it was found while scanning with WH_StartScan.
                

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WH_TurnOffPictoCatch(void)
{
    sPictoCatchFlag = FALSE;
}

/*---------------------------------------------------------------------------*
  Name:         WH_StartScan

  Description:  Function for fetching the parent beacon

  Arguments:    callback - Sets the callback returned when the parent device is found.
                
                macAddr  - Specifies the MAC address for the connected parent
                           If NULL or 0xFFFFFFFFFFFF, all parents are searched.
                           
                channel - Designate the channel on which to search for the parent.
                           If 0, all channels are searched.

  Returns:      None.
 *---------------------------------------------------------------------------*/
BOOL WH_StartScan(WHStartScanCallbackFunc callback, const u8 *macAddr, u16 channel)
{
    WH_TRACE_STATE;
    WH_ASSERT(sSysState == WH_SYSSTATE_IDLE);

    WH_ChangeSysState(WH_SYSSTATE_SCANNING);

    sScanCallback = callback;
    sChannelIndex = channel;
    sScanExParam.channelList = 1;
    sAutoConnectFlag = FALSE;          // Does not automatically connect

    // Sets the conditions for the MAC address to be searched for
    if (macAddr != NULL)
    {
        *(u16 *)(&sScanExParam.bssid[4]) = *(u16 *)(macAddr + 4);
        *(u16 *)(&sScanExParam.bssid[2]) = *(u16 *)(macAddr + 2);
        *(u16 *)(&sScanExParam.bssid[0]) = *(u16 *)(macAddr);
    }
    else
    {
        MI_CpuFill8(&sScanExParam.bssid, 0xff, WM_SIZE_BSSID);
    }

    if (!WH_StateInStartScan())
    {
        WH_ChangeSysState(WH_SYSSTATE_ERROR);
        return FALSE;
    }

    return TRUE;
}

/* ----------------------------------------------------------------------
  state : StartScan
  ---------------------------------------------------------------------- */
static BOOL WH_StateInStartScan(void)
{
    // When in this state, looks for a parent.
    WMErrCode result;
    u16     chanpat;

    WH_ASSERT(sSysState == WH_SYSSTATE_SCANNING);

    chanpat = WM_GetAllowedChannel();

    // Checks if wireless is usable.
    if (chanpat == 0x8000)
    {
        // If 0x8000 is returned, it indicates that wireless is not initialized or there is some other abnormality with the wireless library. Therefore, set to an error.
        // 
        WH_REPORT_FAILURE(WM_ERRCODE_ILLEGAL_STATE);
        return FALSE;
    }
    if (chanpat == 0)
    {
        // In this state, wireless cannot be used.
        WH_REPORT_FAILURE(WH_ERRCODE_NO_RADIO);
        return FALSE;
    }

    if (sChannelIndex == 0)
    {
        /* Search possible channels in ascending order from the current designation */
        u32 chan = MATH_CountTrailingZeros(sScanExParam.channelList) + 1;
        while (TRUE)
        {
            chan++;
            if (chan > 16)
            {
                chan = 1;
            }

            if (chanpat & (0x0001 << (chan - 1)))
            {
                break;
            }
        }
        sScanExParam.channelList = (u16)(1 << (chan - 1));
    }
    else
    {
        sScanExParam.channelList = (u16)(1 << (sChannelIndex - 1));
    }

    sScanExParam.maxChannelTime = WM_GetDispersionScanPeriod();
    sScanExParam.scanBuf = (WMbssDesc *)sScanBuf;
    sScanExParam.scanBufSize = WM_SIZE_SCAN_EX_BUF;
    sScanExParam.scanType = WM_SCANTYPE_PASSIVE;
    sScanExParam.ssidLength = 0;
    MI_CpuFill8(sScanExParam.ssid, 0xFF, sizeof(sScanExParam.ssid));

    result = WM_StartScanEx(WH_StateOutStartScan, &sScanExParam);

    if (result != WM_ERRCODE_OPERATING)
    {
        WH_REPORT_FAILURE(result);
        return FALSE;
    }
    return TRUE;
}


static void WH_StateOutStartScan(void *arg)
{
    WMStartScanExCallback *cb = (WMStartScanExCallback *)arg;
    int i;
    BOOL found;

    // If the scan command fails
    if (cb->errcode != WM_ERRCODE_SUCCESS)
    {
        WH_REPORT_FAILURE(cb->errcode);
        WH_ChangeSysState(WH_SYSSTATE_ERROR);
        return;
    }

    if (sSysState != WH_SYSSTATE_SCANNING)
    {
        // End scan if the state has been changed
        sAutoConnectFlag = FALSE;      // Cancel the automatic connection
        if (!WH_StateInEndScan())
        {
            WH_ChangeSysState(WH_SYSSTATE_ERROR);
        }
        return;
    }

    switch (cb->state)
    {
    case WM_STATECODE_PARENT_NOT_FOUND:
        break;

    case WM_STATECODE_PARENT_FOUND:
        // If a parent is found

        WH_TRACE("WH_StateOutStartScan : bssDescCount=%d\n", cb->bssDescCount);

        if ( cb->bssDescCount > 0 )
        {
            // Because the BssDesc information is written from the ARM7
            // Discards the BssDesc cache that is set in the buffer
            DC_InvalidateRange(&sScanBuf, sizeof(sScanBuf));
        }

        found = FALSE;
        for ( i = 0; i < cb->bssDescCount; i++ )
        {
            WMBssDesc* bd = cb->bssDesc[i];

            // GUIDELINE: Guideline compliance items (6.3.5)
            // Compares ggids and fails if different.
            // First, check WMBssDesc.gameInfoLength.
            // Compare if ggid is a valid value.
            WH_TRACE(" parent: MAC=%02x%02x%02x%02x%02x%02x ",
                     bd->bssid[0], bd->bssid[1], bd->bssid[2],
                     bd->bssid[3], bd->bssid[4], bd->bssid[5]);

            // Determines if the parent is using pictochat
            if (sPictoCatchFlag)
            {
                if (CHT_IsPictochatParent(bd))
                {
                    // If the parent is using pictochat
                    WH_TRACE("pictochat parent find\n");
                    if (sScanCallback != NULL)
                    {
                        sScanCallback(bd);
                    }
                    continue;
                }
            }

            if ((!WM_IsValidGameInfo(&bd->gameInfo, bd->gameInfoLength))
                || bd->gameInfo.ggid != sParentParam.ggid)
            {
                // If GGIDs are different, this is ignored
                WH_TRACE("not my parent ggid \n");
                continue;
            }

            // If the entry flag is not up, the child is not receiving, so this is ignored
            // Also, if the multiboot flag is enabled, it is the DS download parent device, so ignore.
            if ((bd->gameInfo.gameNameCount_attribute & (WM_ATTR_FLAG_ENTRY | WM_ATTR_FLAG_MB))
                != WM_ATTR_FLAG_ENTRY)
            {
                WH_TRACE("not recieve entry\n");
                continue;
            }

            WH_TRACE("parent find\n");
            MI_CpuCopy8(bd, &sBssDesc, sizeof(WMBssDesc)); // Cache-safe copy to the buffer.
            found = TRUE;
            break;
        }

        if (found)
        {
            // call if callback is necessary
            if (sScanCallback != NULL)
            {
                sScanCallback(&sBssDesc);
            }

            // end scan for automatic connection to found parent
            if (sAutoConnectFlag)
            {
                if (!WH_StateInEndScan())
                {
                    WH_ChangeSysState(WH_SYSSTATE_ERROR);
                }
                return;
            }
        }
        break;
    }

    // Changes the channel and starts another scan.
    if (!WH_StateInStartScan())
    {
        WH_ChangeSysState(WH_SYSSTATE_ERROR);
    }
}

/* ----------------------------------------------------------------------
   state : EndScan
  ---------------------------------------------------------------------- */

/*---------------------------------------------------------------------------*
  Name:         WH_EndScan

  Description:  Function for fetching the parent beacon

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
BOOL WH_EndScan(void)
{
    if (sSysState != WH_SYSSTATE_SCANNING)
    {
        return FALSE;
    }

    sAutoConnectFlag = FALSE;
    WH_ChangeSysState(WH_SYSSTATE_BUSY);
    return TRUE;
}


static BOOL WH_StateInEndScan(void)
{
    WMErrCode result;
    WH_TRACE_STATE;

    // In this state, scan end processing is carried out.
    result = WM_EndScan(WH_StateOutEndScan);
    if (result != WM_ERRCODE_OPERATING)
    {
        WH_REPORT_FAILURE(result);
        return FALSE;
    }

    return TRUE;
}

static void WH_StateOutEndScan(void *arg)
{
    WMCallback *cb = (WMCallback *)arg;
    WH_TRACE_STATE;

    if (cb->errcode != WM_ERRCODE_SUCCESS)
    {
        WH_REPORT_FAILURE(cb->errcode);
        return;
    }

    WH_ChangeSysState(WH_SYSSTATE_IDLE);

    if (!sAutoConnectFlag)
    {
        // If only a scan was indicated, complete processing here.
        return;
    }

    // The automatic connection destination has been established; thus, cancel automatic connection mode.
    sAutoConnectFlag = FALSE;

    if (sChildWEPKeyGenerator != NULL)
    {
        // If the WEP Key Generator is configured, set the WEP Key
        if (!WH_StateInSetChildWEPKey())
        {
            WH_ChangeSysState(WH_SYSSTATE_ERROR);
        }
    }
    else
    {
        // Scan end processing is complete, so begin child activities unchanged.
        // 
        if (!WH_StateInStartChild())
        {
            WH_TRACE("WH_StateOutEndScan : startchild failed\n");
            WH_ChangeSysState(WH_SYSSTATE_ERROR);
        }
    }
}

static BOOL WH_StateInSetChildWEPKey(void)
{
    u16     wepmode;
    WMErrCode result;
    WH_TRACE_STATE;

    WH_ChangeSysState(WH_SYSSTATE_BUSY);

    wepmode = (*sChildWEPKeyGenerator) (sWEPKey, &sBssDesc);
    result = WM_SetWEPKey(WH_StateOutSetChildWEPKey, wepmode, sWEPKey);
    if (result != WM_ERRCODE_OPERATING)
    {
        WH_REPORT_FAILURE(result);
        WH_ChangeSysState(WH_SYSSTATE_ERROR);
        return FALSE;
    }

    return TRUE;
}

static void WH_StateOutSetChildWEPKey(void *arg)
{
    WMCallback *cb = (WMCallback *)arg;
    WH_TRACE_STATE;

    if (cb->errcode != WM_ERRCODE_SUCCESS)
    {
        WH_REPORT_FAILURE(cb->errcode);
        WH_ChangeSysState(WH_SYSSTATE_ERROR);
        return;
    }

    // Connect to parent as a child
    if (!WH_StateInStartChild())
    {
        WH_TRACE("WH_StateOutSetChildWEPKey : startchild failed\n");
        WH_ChangeSysState(WH_SYSSTATE_ERROR);
    }
}


/* ----------------------------------------------------------------------
   state : StartChild
  ---------------------------------------------------------------------- */
static BOOL WH_StateInStartChild(void)
{
    WMErrCode result;
    WH_TRACE_STATE;

    if ((sSysState == WH_SYSSTATE_CONNECTED)
        || (sSysState == WH_SYSSTATE_KEYSHARING) || (sSysState == WH_SYSSTATE_DATASHARING))
    {
        // Already connected.
        WH_TRACE("WH_StateInStartChild : already connected?\n");
        return TRUE;
    }

    WH_ChangeSysState(WH_SYSSTATE_BUSY);

    result = WM_StartConnectEx(WH_StateOutStartChild, &sBssDesc, sConnectionSsid, TRUE,
                               (u16)((sChildWEPKeyGenerator !=
                                      NULL) ? WM_AUTHMODE_SHARED_KEY : WM_AUTHMODE_OPEN_SYSTEM));
    if (result != WM_ERRCODE_OPERATING)
    {
        WH_REPORT_FAILURE(result);
        return FALSE;
    }

    return TRUE;
}

static void WH_StateOutStartChild(void *arg)
{
    // The callback set with StartConnect is called in any of the below cases, so differentiation is needed.
    // 1) When the connection processing begins
    // 2) When authentication ends
    // 3) When the parent disconnects after a connection is complete
    // 4) When more than the maximum number of devices try to connect
    // 
    // For this function, advancing to the next item is possible only with 2).

    WMStartConnectCallback *cb = (WMStartConnectCallback *)arg;
    WH_TRACE_STATE;

    if (cb->errcode != WM_ERRCODE_SUCCESS)
    {
        WH_REPORT_FAILURE(cb->errcode);

        if (cb->errcode == WM_ERRCODE_OVER_MAX_ENTRY)
        {
            // GUIDELINE: Guideline compliance items (6.3.7)
            // If there are more children than the parent can handle.
            // Here, this qualifies as a fatal error.
            // At this time, something needs to be displayed by main.
            // Whether the error code was WM_ERRCODE_OVER_MAX_ENTRY can be checked with the WH_GetLastError function.
            // 
            WH_ChangeSysState(WH_SYSSTATE_ERROR);
            return;
        }
        else if (cb->errcode == WM_ERRCODE_NO_ENTRY)
        {
            // The case where a parent, to which a child tried to connect itself, has not received an entry.
            // 
            // Here, this qualifies as a fatal error.
            // At this time, something needs to be displayed by main.
            // Whether the error code was WM_ERRCODE_NO_ENTRY can be checked with the WH_GetLastError function.
            // 
            WH_ChangeSysState(WH_SYSSTATE_ERROR);
            return;
        }
        else if (cb->errcode == WM_ERRCODE_FAILED)
        {
            // The case where a connection request timed out, due to a reason such as that the parent to which the child tried to connect itself is no longer present.
            // 
            // Here, this qualifies as a fatal error.
            // Here, either the main needs to reset and the connection retried,
            // or something needs to be displayed.
            // Whether the error code was WM_ERRCODE_FAILED can be checked with the WH_GetLastError function.
            // 
            WH_ChangeSysState(WH_SYSSTATE_CONNECT_FAIL);
            return;
        }
        else
        {
            // If the channel setting of the parent is invalid, WM_ERRCODE_INVALID_PARAM may be returned.
            // 
            WH_ChangeSysState(WH_SYSSTATE_ERROR);
        }
        return;
    }

    if (cb->state == WM_STATECODE_BEACON_LOST)
    {
        // Beacon reception failed after 16 attempts from the parent being connected.
        // If the beacon cannot be found, it is possible that the V-blank sync could break down or that it will not be noticed that the parent has started a new session (the TGID has changed).
        // 
        // 

        // Nothing is executed in the demo.
        return;
    }

    if (cb->state == WM_STATECODE_CONNECTED)
    {
        // When authentication is complete.
        // Self-allocated AID goes in cb->aid
        WH_TRACE("Connect to Parent\n");
        WH_ChangeSysState(WH_SYSSTATE_CONNECTED);
        if (!WH_StateInStartChildMP())
        {
            /* FIXME : Is it OK to put this here while BUSY? */
            WH_TRACE("WH_StateInStartChildMP failed\n");
            WH_ChangeSysState(WH_SYSSTATE_BUSY);
            return;
        }

        // Saves own aid.
        sMyAid = cb->aid;
        return;

    }
    else if (cb->state == WM_STATECODE_CONNECT_START)
    {
        // When the connection processing begins.
        // When the parent is no longer present (and similar situations), WM_ERRCODE_FAILED is returned in cb->errcode. If the parent does not accept the entry, WM_ERRCODE_NO_ENTRY is returned in cb->errcode. If all of the parent connections are taken, WM_ERRCODE_OVER_MAX_ENTRY is returned in cb->errcode.
        // 
        // 
        // 
        // The process does nothing and waits for authentication to end.
        return;

    }
    else if (cb->state == WM_STATECODE_DISCONNECTED)
    {
        // GUIDELINE: Guideline compliance items (6.3.1)
        // When disconnected by the parent.
        // Processing like that above (6.3.7) is needed (to display something).
        // The error code here is WH_ERRCODE_DISCONNECTED
        // (Note that it is not 'WM_'.)
        // In such cases (disconnect from the parent), it is possible that an ordinary game is in play, but here, this is treated as an error to be handled by main.
        // 
        // 

        WH_TRACE("Disconnected from Parent\n");
        WH_SetError(WH_ERRCODE_DISCONNECTED);
        WH_ChangeSysState(WH_SYSSTATE_ERROR);
        return;
    }
    else if (cb->state == WM_STATECODE_DISCONNECTED_FROM_MYSELF)
    {
        // Does nothing if the unit disconnects from itself
        return;
    }

    WH_TRACE("unknown state %d, %s\n", cb->state, WH_GetWMStateCodeName(cb->state));
    WH_ChangeSysState(WH_SYSSTATE_ERROR);
}

/* ----------------------------------------------------------------------
   state : StartChildMP
   ---------------------------------------------------------------------- */
static BOOL WH_StateInStartChildMP(void)
{
    WMErrCode result;
    WH_TRACE_STATE;

    result = WM_StartMP(WH_StateOutStartChildMP,
                        (u16 *)sRecvBuffer,
                        (u16)sRecvBufferSize, (u16 *)sSendBuffer, (u16)sSendBufferSize,
                        WH_MP_FREQUENCY);

    if (result != WM_ERRCODE_OPERATING)
    {
        WH_REPORT_FAILURE(result);
        return FALSE;
    }

    return TRUE;
}

static void WH_StateOutStartChildMP(void *arg)
{
    WMStartMPCallback *cb = (WMStartMPCallback *)arg;
    // WH_TRACE_STATE;

    if (cb->errcode != WM_ERRCODE_SUCCESS)
    {

        if (cb->errcode == WM_ERRCODE_SEND_FAILED)
        {
            // The case where notification of a parent receiving error is made by the MPACK frame in WM_STATECODE_MPACK_IND.
            // 
            // Since retransmission and other processing is handled in a separate location, nothing special needs to be executed here.
            // This is for debugging.
            return;

        }
        else if (cb->errcode == WM_ERRCODE_TIMEOUT)
        {
            // This is the case where after receiving the MP frame, the MPACK frame cannot be received even after a fixed period of time passes.
            // (cb->state == WM_STATECODE_MPACK_IND)
            // Since retransmission and other processing is handled in a separate location, nothing special needs to be executed here.
            // This is for debugging.
            return;

        }
        else if (cb->errcode == WM_ERRCODE_INVALID_POLLBITMAP)
        {
            // The case where a frame is received for another machine in WM_STATECODE_MP_IND or WM_STATECODE_MPACK_IND.
            // 
            // Communication with three or more machines occurs regularly, so
            // this should not be made a fatal error.
            // Nothing special needs to be executed here. This is for debugging.
            return;
        }

        WH_REPORT_FAILURE(cb->errcode);
        WH_ChangeSysState(WH_SYSSTATE_ERROR);
        return;
    }

    switch (cb->state)
    {
    case WM_STATECODE_MP_START:
        // Notification that the StartMP has ended normally.
        // Hereafter, send and receive are enabled.

        if (sConnectMode == WH_CONNECTMODE_KS_CHILD)
        {
            // If key sharing was specified.
            if (sSysState == WH_SYSSTATE_KEYSHARING)
            {
                // The key sharing state is already set, so nothing happens.
                return;
            }

            if (sSysState == WH_SYSSTATE_CONNECTED)
            {
                // Switches to StartChildKeyShare.
                if (!WH_StateInStartChildKeyShare())
                {
                    WH_TRACE("WH_StateInStartChildKeyShare failed\n");
                    (void)WH_Finalize();
                }
                return;
            }

        }
        else if (sConnectMode == WH_CONNECTMODE_DS_CHILD)
        {
            // Calls WM_StartDataSharing if data sharing was specified.
            // This function is synchronous, so no switch has been made to a WH state.
            WMErrCode result;
            u16     aidBitmap;

            aidBitmap = (u16)((1 << (WH_CHILD_MAX + 1)) - 1);   // Bitmap with the lower WH_CHILD_MAX+1 bit==1
            result = WM_StartDataSharing(&sDSInfo, WH_DS_PORT, aidBitmap, WH_DS_DATA_SIZE, sDataSharingDoubleMode);
            if (result != WM_ERRCODE_SUCCESS)
            {
                WH_REPORT_FAILURE(result);
                (void)WH_Finalize();
                return;
            }

            WH_TRACE("WH_StateOutStartChildMP : WM_StartDataSharing OK\n");
            WH_ChangeSysState(WH_SYSSTATE_DATASHARING);
            if (sPortExtraCallback)
            {
                (*sPortExtraCallback)(arg);
            }
            return;
        }

        WH_ChangeSysState(WH_SYSSTATE_CONNECTED);
        if (sPortExtraCallback)
        {
            (*sPortExtraCallback)(arg);
        }
        break;

    case WM_STATECODE_MP_IND:
        // The child receiving is complete.

        // The contents of the data frame received from the parent device are taken in by cb->recvBuf, but
        // set a callback with WM_SetPortCallback for normal data receiving.
        // When using data sharing or key sharing, data is received internally, so WM_SetPortCallback does not need to be used.
        // 

        break;

    case WM_STATECODE_MPACK_IND:
        // Notification of the MPACK frame. This is for debugging.
        break;

    case WM_STATECODE_MPEND_IND:
        // The child should not come here.

    default:
        WH_TRACE("unknown indicate, state = %d\n", cb->state);
        break;
    }
}

/* ----------------------------------------------------------------------
   state : StartChildKeyShare
  ---------------------------------------------------------------------- */
static BOOL WH_StateInStartChildKeyShare(void)
{
    WMErrCode result;
    WH_TRACE_STATE;

    if (sSysState == WH_SYSSTATE_KEYSHARING)
    {
        // Key sharing is already being used.
        return TRUE;
    }

    if (sSysState != WH_SYSSTATE_CONNECTED)
    {
        // Not connected.
        return FALSE;
    }

    WH_ChangeSysState(WH_SYSSTATE_KEYSHARING);
    result = WM_StartKeySharing(&sWMKeySetBuf, WH_DS_PORT);

    if (result != WM_ERRCODE_SUCCESS)
    {
        WH_REPORT_FAILURE(result);
        return FALSE;
    }

    return TRUE;
}

/* ----------------------------------------------------------------------
   state : EndChildKeyShare
   ---------------------------------------------------------------------- */
static BOOL WH_StateInEndChildKeyShare(void)
{
    // Ends key sharing.
    WMErrCode result;
    WH_TRACE_STATE;

    if (sSysState != WH_SYSSTATE_KEYSHARING)
    {
        return FALSE;
    }

    WH_ChangeSysState(WH_SYSSTATE_BUSY);
    result = WM_EndKeySharing(&sWMKeySetBuf);

    if (result != WM_ERRCODE_SUCCESS)
    {
        WH_REPORT_FAILURE(result);
        return FALSE;
    }

    if (!WH_StateInEndChildMP())
    {
        return FALSE;
    }

    return TRUE;
}

/* ----------------------------------------------------------------------
   state : EndChildMP
  ---------------------------------------------------------------------- */
static BOOL WH_StateInEndChildMP(void)
{
    // Ends MP communication.
    WMErrCode result;
    WH_TRACE_STATE;

    WH_ChangeSysState(WH_SYSSTATE_BUSY);

    result = WM_EndMP(WH_StateOutEndChildMP);
    if (result != WM_ERRCODE_OPERATING)
    {
        WH_REPORT_FAILURE(result);
        return FALSE;
    }
    return TRUE;
}

static void WH_StateOutEndChildMP(void *arg)
{
    WMCallback *cb = (WMCallback *)arg;
    WH_TRACE_STATE;

    if (cb->errcode != WM_ERRCODE_SUCCESS)
    {
        WH_REPORT_FAILURE(cb->errcode);
        (void)WH_Finalize();
        return;
    }

    if (!WH_StateInEndChild())
    {
        WH_ChangeSysState(WH_SYSSTATE_ERROR);
    }
}

/* ----------------------------------------------------------------------
  state : EndChild
  ---------------------------------------------------------------------- */
static BOOL WH_StateInEndChild(void)
{
    WMErrCode result;
    WH_TRACE_STATE;

    WH_ChangeSysState(WH_SYSSTATE_BUSY);

    // Terminates the connection with the parent.
    result = WM_Disconnect(WH_StateOutEndChild, 0);
    if (result != WM_ERRCODE_OPERATING)
    {
        WH_REPORT_FAILURE(result);
        WH_Reset();
        return FALSE;
    }

    return TRUE;
}

static void WH_StateOutEndChild(void *arg)
{
    WMCallback *cb = (WMCallback *)arg;
    WH_TRACE_STATE;
    if (cb->errcode != WM_ERRCODE_SUCCESS)
    {
        WH_REPORT_FAILURE(cb->errcode);
        return;
    }
    // At this point, the child disconnection processing ends, and the state returns to idle (standby).
    WH_ChangeSysState(WH_SYSSTATE_IDLE);
}

/* ----------------------------------------------------------------------
  state : Reset
  ---------------------------------------------------------------------- */
static BOOL WH_StateInReset(void)
{
    // This state is common to parents and children.
    // Returns the system to its initial state.
    WMErrCode result;
    WH_TRACE_STATE;

    WH_ChangeSysState(WH_SYSSTATE_BUSY);
    result = WM_Reset(WH_StateOutReset);
    if (result != WM_ERRCODE_OPERATING)
    {
        WH_REPORT_FAILURE(result);
        return FALSE;
    }
    return TRUE;
}

static void WH_StateOutReset(void *arg)
{
    WMCallback *cb = (WMCallback *)arg;
    WH_TRACE_STATE;
    if (cb->errcode != WM_ERRCODE_SUCCESS)
    {
        WH_ChangeSysState(WH_SYSSTATE_ERROR);
        WH_REPORT_FAILURE(cb->errcode);
        return;
    }
    if (sPortExtraCallback)
    {
        (*sPortExtraCallback)(arg);
    }
    // Reset sets to the idling (standby) state without starting the next state.
    WH_ChangeSysState(WH_SYSSTATE_IDLE);
}

/* ----------------------------------------------------------------------
   Disconnect
  ---------------------------------------------------------------------- */
static BOOL WH_StateInDisconnectChildren(u16 bitmap)
{
    // In this state, the connection with the child set with the argument is cut.
    WMErrCode result;
    WH_TRACE_STATE;

    result = WM_DisconnectChildren(WH_StateOutDisconnectChildren, bitmap);

    if (result == WM_ERRCODE_NO_CHILD)
    {
        return FALSE;
    }

    if (result != WM_ERRCODE_OPERATING)
    {
        WH_REPORT_FAILURE(result);
        return FALSE;
    }

    {
        OSIntrMode enabled = OS_DisableInterrupts();
        sConnectBitmap &= ~bitmap;
        (void)OS_RestoreInterrupts(enabled);
    }
    return TRUE;
}

static void WH_StateOutDisconnectChildren(void *arg)
{
    WMCallback *cb = (WMCallback *)arg;
    WH_TRACE_STATE;
    if (cb->errcode != WM_ERRCODE_SUCCESS)
    {
        WH_REPORT_FAILURE(cb->errcode);
        return;
    }
}

/* ----------------------------------------------------------------------
   power off
  ---------------------------------------------------------------------- */
static BOOL WH_StateInPowerOff(void)
{
    // Ends the power supply to the wireless hardware.
    // This state is common to parents and children.
    WMErrCode result;
    WH_TRACE_STATE;

    result = WM_PowerOff(WH_StateOutPowerOff);
    if (result != WM_ERRCODE_OPERATING)
    {
        WH_REPORT_FAILURE(result);
        return FALSE;
    }
    return TRUE;
}

static void WH_StateOutPowerOff(void *arg)
{
    // This state is after power disconnect.
    WMCallback *cb = (WMCallback *)arg;
    WH_TRACE_STATE;

    if (cb->errcode != WM_ERRCODE_SUCCESS)
    {
        WH_REPORT_FAILURE(cb->errcode);
        return;
    }

    if (!WH_StateInDisable())
    {
        WH_ChangeSysState(WH_SYSSTATE_FATAL);
    }
}

/* ----------------------------------------------------------------------
   disable
  ---------------------------------------------------------------------- */
static BOOL WH_StateInDisable(void)
{
    // Communicates that the wireless hardware use has ended.
    // This state is common to parents and children.
    WMErrCode result;
    WH_TRACE_STATE;

    result = WM_Disable(WH_StateOutDisable);
    if (result != WM_ERRCODE_OPERATING)
    {
        WH_REPORT_FAILURE(result);
        return FALSE;
    }
    return TRUE;
}

static void WH_StateOutDisable(void *arg)
{
    // Everything has ended.
    WMCallback *cb = (WMCallback *)arg;
    WH_TRACE_STATE;

    if (cb->errcode != WM_ERRCODE_SUCCESS)
    {
        WH_REPORT_FAILURE(cb->errcode);
    }
}

/* ----------------------------------------------------------------------
  state : SetMPData
  ---------------------------------------------------------------------- */
static BOOL WH_StateInSetMPData(const void *data, u16 datasize, WHSendCallbackFunc callback)
{
    // This state is common to parents and children.
    // Sets and sends the data.
    WMErrCode result;
    // WH_TRACE_STATE;

    DC_FlushRange(sSendBuffer, (u32)sSendBufferSize);
    /* The I/O register is accessed using PXI operation, so cache wait is not needed */
    // DC_WaitWriteBufferEmpty();
    result = WM_SetMPDataToPortEx(WH_StateOutSetMPData,
                                  (void *)callback,
                                  data, datasize, 0xffff, WH_DATA_PORT, WH_DATA_PRIO);
    if (result != WM_ERRCODE_OPERATING)
    {
        WH_TRACE("WH_StateInSetMPData failed - %s\n", WH_GetWMErrCodeName(result));
        return FALSE;
    }
    return TRUE;
}

static void WH_StateOutSetMPData(void *arg)
{
    WMPortSendCallback *cb = (WMPortSendCallback *)arg;
    // WH_TRACE_STATE;

    // The memory region for the transmission data specified with SetMPDataToPort should not be overwritten until this callback is called.
    // 

    // If ports 0 to 7 are used and the transmission fails,
    // WM_ERRCODE_SEND_FAILED is returned.
    // If the transmission queue is full,
    // WM_ERRCODE_SEND_QUEUE_FULL is returned.

    if (sPortExtraCallback)
    {
        (*sPortExtraCallback)(arg);
    }
    if (cb->errcode != WM_ERRCODE_SUCCESS && cb->errcode != WM_ERRCODE_SEND_FAILED)
    {
        WH_REPORT_FAILURE(cb->errcode);
        return;
    }

    if (cb->arg != NULL)
    {
        WHSendCallbackFunc callback = (WHSendCallbackFunc) cb->arg;
        // Change the WHSendCallbackFunc type as necessary, and cause cb->aid and cb->data to be received.
        // 
        (*callback) ((cb->errcode == WM_ERRCODE_SUCCESS));
    }
}

static void WH_PortReceiveCallback(void *arg)
{
    WMPortRecvCallback *cb = (WMPortRecvCallback *)arg;

    if (cb->errcode != WM_ERRCODE_SUCCESS)
    {
        WH_REPORT_FAILURE(cb->errcode);
    }
    else if (sReceiverFunc != NULL)
    {
        if (cb->state == WM_STATECODE_PORT_INIT)
        {
            // Does nothing during initialization.
            // The aid of the other parties connected at time of initialization are set in cb->aidBitmap.
            // 
        }
        if (cb->state == WM_STATECODE_PORT_RECV)
        {
            // Data was received, so the callback is called.
            (*sReceiverFunc) (cb->aid, cb->data, cb->length);
        }
        else if (cb->state == WM_STATECODE_DISCONNECTED)
        {
            // The fact that there was a disconnection is communicated with NULL transmission.
            (*sReceiverFunc) (cb->aid, NULL, 0);
        }
        else if (cb->state == WM_STATECODE_DISCONNECTED_FROM_MYSELF)
        {
            // For backward compatibility, no notification is made regarding self-disconnection.
        }
        else if (cb->state == WM_STATECODE_CONNECTED)
        {
            // Do nothing when connected.
            // The aid of the other party that has make a connection is set in cb->aid. The MAC address of the other party is set in cb->macAddress.
            // 
            // 
        }
    }
}


/* ----------------------------------------------------------------------
  state : End
  ---------------------------------------------------------------------- */

/* ----------------------------------------------------------------------
  state : WM_End
  ---------------------------------------------------------------------- */
static void WH_StateOutEnd(void *arg)
{
    WMCallback *cb = (WMCallback *)arg;
    if (cb->errcode != WM_ERRCODE_SUCCESS)
    {
        WH_ChangeSysState(WH_SYSSTATE_FATAL);
        return;
    }
    WH_ChangeSysState(WH_SYSSTATE_STOP);
}


/* ======================================================================
   Public Interfaces
   ====================================================================== */


/**************************************************************************
 * The functions below alter each type of WH configuration values.
 **************************************************************************/

/*---------------------------------------------------------------------------*
  Name:         WH_SetGgid

  Description:  Configures the game group ID.
                Call before making a connection to the parent device.

  Arguments:    ggid    The game group ID to configure.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WH_SetGgid(u32 ggid)
{
    sParentParam.ggid = ggid;
}

/*---------------------------------------------------------------------------*
  Name:         WH_SetSsid

  Description:  Sets the SSID specified during child connection.
                Call before making a connection from the child device.

  Arguments:    ssid    Buffer in which the SSID to be set has been stored.
                length  data length of the SSID to be set.
                        If less than WM_SIZE_CHILD_SSID (24-byte), fill the subsequent whitespace with 0s. If more than WM_SIZE_CHILD_SSID, truncate.
                        

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WH_SetSsid(const void *ssid, u32 length)
{
    length = (u32)MATH_MIN(length, WM_SIZE_CHILD_SSID);
    MI_CpuCopy8(ssid, sConnectionSsid, length);
    MI_CpuClear8(sConnectionSsid + length, (u32)(WM_SIZE_CHILD_SSID - length));
}

/*---------------------------------------------------------------------------*
  Name:         WH_SetUserGameInfo

  Description:  Configures user-defined parent device information
                Call before making a connection to the parent device.

  Arguments:    userGameInfo   pointer to user-defined parent device information
                length         the size of the user-defined parent device information

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WH_SetUserGameInfo(u16 *userGameInfo, u16 length)
{
    SDK_ASSERT(length <= WM_SIZE_USER_GAMEINFO);
    SDK_ASSERT((userGameInfo != NULL) || (length > 0));
    SDK_ASSERT(sSysState == WH_SYSSTATE_IDLE);

    // Specify user definition data here if to be used in the beacon.
    // When you want to display the parent device's nickname on the parent device selection screen of the child device (or other such occasions), set the information here, and it will be conveyed from the parent device to the child device.
    // 
    sParentParam.userGameInfo = userGameInfo;
    sParentParam.userGameInfoLength = length;
}

/*---------------------------------------------------------------------------*
  Name:         WH_SetDebugOutput

  Description:  Configures the function for outputting the debug character string.

  Arguments:    func    Function for the set debug character string output.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WH_SetDebugOutput(void (*func) (const char *, ...))
{
    OSIntrMode enabled = OS_DisableInterrupts();
    wh_trace = func;
    (void)OS_RestoreInterrupts(enabled);
}


/**************************************************************************
 * Wrapper functions for fetching the WM library state are given below.
 **************************************************************************/

/* ----------------------------------------------------------------------
  Name:        WH_GetLinkLevel
  Description: Gets the radio reception strength.
  Arguments:   None.
  Returns:     Returns the numeric value of WMLinkLevel.
  ---------------------------------------------------------------------- */
int WH_GetLinkLevel(void)
{
    return (int)WM_GetLinkLevel();
}

/* ----------------------------------------------------------------------
   Name:        WH_GetAllowedChannel
   Description: Gets a bit pattern of channels that can be used for connection.
   Arguments:   None.
   Returns:     channel pattern
   ---------------------------------------------------------------------- */
u16 WH_GetAllowedChannel(void)
{
    return WM_GetAllowedChannel();
}


/**************************************************************************
 * The functions below take the WH state.
 **************************************************************************/

/* ----------------------------------------------------------------------
   Name:        WH_GetBitmap
   Description: Gets the bit pattern for displaying the connection state.
   Arguments:   None.
   Returns:     bitmap pattern
   ---------------------------------------------------------------------- */
u16 WH_GetBitmap(void)
{
    return sConnectBitmap;
}

/* ----------------------------------------------------------------------
   Name:        WH_GetSystemState
   Description: Gets the WH internal state.
   Arguments:   None.
   Returns:     The internal state (WH_SYSSTATE_XXXX).
   ---------------------------------------------------------------------- */
int WH_GetSystemState(void)
{
    return sSysState;
}

/* ----------------------------------------------------------------------
   Name:        WH_GetConnectMode
   Description: Gets the connection information.
   Arguments:   None.
   Returns:     The connection information (WH_CONNECTMODE_XX_XXXX).
   ---------------------------------------------------------------------- */
int WH_GetConnectMode(void)
{
    return sConnectMode;
}

/* ----------------------------------------------------------------------
   Name:        WH_GetLastError
   Description: Gets the error code that most recently occurred.
   Arguments:   None.
   Returns:     The error code.
   ---------------------------------------------------------------------- */
int WH_GetLastError(void)
{
    return sErrCode;
}

/*---------------------------------------------------------------------------*
  Name:         WH_PrintBssDesc

  Description:  Debug outputs the members of the WMBssDesc structure.

  Arguments:    info    The pointer to the BssDesc to be debug output.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WH_PrintBssDesc(WMBssDesc *info)
{
#pragma unused( info )
    u16     i;

    OS_TPrintf("length = 0x%04x\n", info->length);
    OS_TPrintf("rssi   = 0x%04x\n", info->rssi);
    OS_TPrintf("bssid = %02x%02x%02x%02x%02x%02x\n", info->bssid[0], info->bssid[1], info->bssid[2],
               info->bssid[3], info->bssid[4], info->bssid[5]);
    OS_TPrintf("ssidLength = 0x%04x\n", info->ssidLength);
    OS_TPrintf("ssid = ");
    for (i = 0; i < 32; i++)
    {
        OS_TPrintf("0x%02x", info->ssid[i]);
    }
    OS_TPrintf("\n");
    OS_TPrintf("capaInfo        = 0x%04x\n", info->capaInfo);
    OS_TPrintf("rateSet.basic   = 0x%04x\n", info->rateSet.basic);
    OS_TPrintf("rateSet.support = 0x%04x\n", info->rateSet.support);
    OS_TPrintf("beaconPeriod    = 0x%04x\n", info->beaconPeriod);
    OS_TPrintf("dtimPeriod      = 0x%04x\n", info->dtimPeriod);
    OS_TPrintf("channel         = 0x%04x\n", info->channel);
    OS_TPrintf("cfpPeriod       = 0x%04x\n", info->cfpPeriod);
    OS_TPrintf("cfpMaxDuration  = 0x%04x\n", info->cfpMaxDuration);
    OS_TPrintf("gameInfoLength  = 0x%04x\n", info->gameInfoLength);
    OS_TPrintf("gameInfo.magicNumber = 0x%04x\n", info->gameInfo.magicNumber);
    OS_TPrintf("gameInfo.ver    = 0x%02x\n", info->gameInfo.ver);
    OS_TPrintf("gameInfo.ggid   = 0x%08x\n", info->gameInfo.ggid);
    OS_TPrintf("gameInfo.tgid   = 0x%04x\n", info->gameInfo.tgid);
    OS_TPrintf("gameInfo.userGameInfoLength = 0x%02x\n", info->gameInfo.userGameInfoLength);
    OS_TPrintf("gameInfo.gameNameCount_attribute = 0x%02x\n",
               info->gameInfo.gameNameCount_attribute);
    OS_TPrintf("gameInfo.parentMaxSize   = 0x%04x\n", info->gameInfo.parentMaxSize);
    OS_TPrintf("gameInfo.childMaxSize    = 0x%04x\n", info->gameInfo.childMaxSize);
}


/**************************************************************************
 * The functions below perform channel-related processes.
 **************************************************************************/

/*---------------------------------------------------------------------------*
  Name:         WH_StartMeasureChannel

  Description:  Starts an examination of the channel signal use rate

  Arguments:    None.

  Returns:     TRUE if the sequence start was a success.
 *---------------------------------------------------------------------------*/
BOOL WH_StartMeasureChannel(void)
{
#define MAX_RATIO 100                  // The channel use rate is between 0 and 100
    u16     result;
    u8      macAddr[6];

    OS_GetMacAddress(macAddr);
    RAND_INIT(OS_GetVBlankCount() + *(u16 *)&macAddr[0] + *(u16 *)&macAddr[2] + *(u16 *)&macAddr[4]);   // Random number initialization
    RAND();

    sChannel = 0;
    sChannelBusyRatio = MAX_RATIO + 1;

    WH_ChangeSysState(WH_SYSSTATE_BUSY);

    result = WH_StateInMeasureChannel(1);

    if (result == WH_ERRCODE_NOMORE_CHANNEL)
    {
        // There are not any channels available
        // A special state will soon occur where the signal cannot be used, so an error is returned here.
        // 
        // (Generally, something should be displayed at this point.)
        WH_REPORT_FAILURE(WH_ERRCODE_NOMORE_CHANNEL);
        WH_ChangeSysState(WH_SYSSTATE_ERROR);
        return FALSE;
    }

    if (result != WM_ERRCODE_OPERATING)
    {
        // Error complete
        WH_REPORT_FAILURE(result);
        WH_ChangeSysState(WH_SYSSTATE_ERROR);
        return FALSE;
    }
    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         WH_StateInMeasureChannel

  Arguments:    channel     The channel number to start the search at

  Returns:      WM_ERRCODE_SUCCESS        - Searching
                WM_ERRCODE_NOMORE_CHANNEL - There are no more channels to search
                WM_ERRCODE_API_ERR        - WM_MeasureChannel API call failure
 *---------------------------------------------------------------------------*/
static u16 WH_StateInMeasureChannel(u16 channel)
{
    u16     allowedChannel;
    u16     result;

    allowedChannel = WM_GetAllowedChannel();

    if (allowedChannel == 0x8000)
    {
        // If 0x8000 is returned, it indicates that wireless is not initialized or there is some other abnormality with the wireless library. Therefore, set to an error.
        // 
        WH_REPORT_FAILURE(WM_ERRCODE_ILLEGAL_STATE);
        WH_ChangeSysState(WH_SYSSTATE_ERROR);
        return WM_ERRCODE_ILLEGAL_STATE;
    }
    if (allowedChannel == 0)
    {
        // If 0 is returned, there will soon be a special state where the signal cannot be used, so return indicating that there are no available channels.
        // 
        WH_REPORT_FAILURE(WH_ERRCODE_NO_RADIO);
        WH_ChangeSysState(WH_SYSSTATE_ERROR);
        return WH_ERRCODE_NOMORE_CHANNEL;
    }

    while (((1 << (channel - 1)) & allowedChannel) == 0)
    {
        channel++;
        if (channel > 16)
        {
            /* When finished searching all allowed channels */
            return WH_ERRCODE_NOMORE_CHANNEL;
        }
    }

    result = WHi_MeasureChannel(WH_StateOutMeasureChannel, channel);
    if (result != WM_ERRCODE_OPERATING)
    {
        return result;
    }
    return result;
}

/*---------------------------------------------------------------------------*
  Name:         WH_StateOutMeasureChannel

  Arguments:    arg     The WMMeasureChannelCallback structure for notification of the search results

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WH_StateOutMeasureChannel(void *arg)
{
    u16     result;
    u16     channel;
    WMMeasureChannelCallback *cb = (WMMeasureChannelCallback *)arg;
    WH_TRACE_STATE;

    if (cb->errcode != WM_ERRCODE_SUCCESS)
    {
        // If there is anything wrong
        // It can be assumed that the signal cannot be used if MeasureChannel fails, so an error state results.
        // 
        WH_REPORT_FAILURE(cb->errcode);
        WH_ChangeSysState(WH_SYSSTATE_ERROR);
        return;
    }

    WH_TRACE("channel %d bratio = 0x%x\n", cb->channel, cb->ccaBusyRatio);

    channel = cb->channel;

    /* Get a channel with a lower usage rate (Initial value is 101%, so be sure to select the top) */
    if (sChannelBusyRatio > cb->ccaBusyRatio)
    {
        sChannelBusyRatio = cb->ccaBusyRatio;
        sChannelBitmap = (u16)(1 << (channel - 1));
    }
    else if (sChannelBusyRatio == cb->ccaBusyRatio)
    {
        sChannelBitmap |= 1 << (channel - 1);
    }

    result = WH_StateInMeasureChannel(++channel);

    if (result == WH_ERRCODE_NOMORE_CHANNEL)
    {
        // The channel search ends
        WH_ChangeSysState(WH_SYSSTATE_MEASURECHANNEL);
        return;
    }

    if (result != WM_ERRCODE_OPERATING)
    {
        // Error complete
        WH_ChangeSysState(WH_SYSSTATE_ERROR);
        return;
    }
}

/* ----------------------------------------------------------------------
  Checks the signal use rate
  ---------------------------------------------------------------------- */
static WMErrCode WHi_MeasureChannel(WMCallbackFunc func, u16 channel)
{
#define WH_MEASURE_TIME         30     // The time interval in ms for picking up the signal for making one communication for one frame
#define WH_MEASURE_CS_OR_ED     3      // The logical OR of the carrier sense and the ED value
#define WH_MEASURE_ED_THRESHOLD 17     // The recommended ED threshold value which has been empirically shown to be effective.

    /*
     * An experimental value considered to be empirically valid has been used as a parameter for obtaining the signal usage rate.
     * 
     */
    return WM_MeasureChannel(func,     /* callback settings */
                             WH_MEASURE_CS_OR_ED,       /* CS or ED */
                             WH_MEASURE_ED_THRESHOLD,   /* Invalid only when the 2nd argument is carrier sense */
                             channel,  /* current search channel */
                             WH_MEASURE_TIME);  /*search time [in ms] of each channel */
}


/*---------------------------------------------------------------------------*
  Name:         WH_GetMeasureChannel

  Description:  Returns the channel with the lowest usage rate from all the usable channels.
                The WH_MeasureChannel operations must be complete and it must be in a WH_SYSSTATE_MEASURECHANNEL state.
                
                When this function is called, the channel with the lowest use rate is returned and the state transitions to WH_SYSSTATE_IDLE.
                
                
  Arguments:    None.

  Returns:      The channel number of the usable channel with the lowest usage rate
 *---------------------------------------------------------------------------*/
u16 WH_GetMeasureChannel(void)
{
    WH_ASSERT(sSysState == WH_SYSSTATE_MEASURECHANNEL);

    WH_ChangeSysState(WH_SYSSTATE_IDLE);
    sChannel = (u16)SelectChannel(sChannelBitmap);
    WH_TRACE("decided channel = %d\n", sChannel);
    return sChannel;
}


/*---------------------------------------------------------------------------*
  Name:         SelectChannel

  Description:  Obtains the channel with the lowest signal usage rate.
                If there are multiple channels with the lowest signal usage rate, use a random number.
                
                
  Arguments:    Channel bit map

  Returns:      The channel number of the usable channel with the lowest usage rate
 *---------------------------------------------------------------------------*/
static s16 SelectChannel(u16 bitmap)
{
    s16     i;
    s16     channel = 0;
    u16     num = 0;
    u16     select;

    for (i = 0; i < 16; i++)
    {
        if (bitmap & (1 << i))
        {
            channel = (s16)(i + 1);
            num++;
        }
    }

    if (num <= 1)
    {
        return channel;
    }

    // If there are multiple channels of the same signal usage rate
    select = (u16)(((RAND() & 0xFF) * num) / 0x100);

    channel = 1;

    for (i = 0; i < 16; i++)
    {
        if (bitmap & 1)
        {
            if (select == 0)
            {
                return (s16)(i + 1);
            }
            select--;
        }
        bitmap >>= 1;
    }

    return 0;
}


/**************************************************************************
 * Functions initializing wireless and switching to a communication enabled state are given below.
 **************************************************************************/

/* ----------------------------------------------------------------------
   Name:        WH_Initialize
   Description: Performs initialization operations, and starts the initialization sequence.
   Arguments:   None.
   Returns:     TRUE if the sequence start was a success.
   ---------------------------------------------------------------------- */
BOOL WH_Initialize(void)
{
    if (whInitialized)
    {
        return FALSE;
    }

    if (sIndicationCallback == NULL)
    {
        sIndicationCallback = WH_IndicateHandler;
    }

    sRecvBufferSize = 0;
    sSendBufferSize = 0;

    sReceiverFunc = NULL;
    sMyAid = 0;
    sConnectBitmap = WH_BITMAP_EMPTY;
    sErrCode = WM_ERRCODE_SUCCESS;

    sParentParam.userGameInfo = NULL;
    sParentParam.userGameInfoLength = 0;

    MI_CpuClear8(sScanBuf, sizeof(sScanBuf));
    MI_CpuClear8(&sBssDesc, sizeof(sBssDesc));

    MI_CpuClear8(sConnectionSsid, sizeof(sConnectionSsid));
    // Sets the connected child user determination function to NULL (multiboot)
    sJudgeAcceptFunc = NULL;

    // Starts the initialization sequence.
    if (!WH_StateInInitialize())
    {
        return FALSE;
    }

    whInitialized = 1;

    return TRUE;
}

/* ----------------------------------------------------------------------
   Indicate handler
   ---------------------------------------------------------------------- */
static void WH_IndicateHandler(void *arg)
{
    WMindCallback *cb = (WMindCallback *)arg;

    if (cb->errcode == WM_ERRCODE_FIFO_ERROR)
    {
        // In case an unrecoverable error occurred.
        // Display a screen to notify the player, and stop the wireless communications process.
        // 
        WH_ChangeSysState(WH_SYSSTATE_ERROR);
        OS_TPanic("Wireless FATAL error occured.\n");
    }
}

/* ----------------------------------------------------------------------
   state : Initialize
   ---------------------------------------------------------------------- */
static BOOL WH_StateInInitialize(void)
{
    // Starts the initialization sequence.
    WMErrCode result;
    WH_TRACE_STATE;

#ifndef WH_USE_DETAILED_INITIALIZE
    WH_ChangeSysState(WH_SYSSTATE_BUSY);
    result = WM_Initialize(&sWmBuffer, WH_StateOutInitialize, WH_DMA_NO);
    if (result != WM_ERRCODE_OPERATING)
    {
        WH_REPORT_FAILURE(result);
        WH_ChangeSysState(WH_SYSSTATE_FATAL);
        return FALSE;
    }

#else
    // WM_Init is a synchronous function.
    result = WM_Init(&sWmBuffer, WH_DMA_NO);
    if (result != WM_ERRCODE_SUCCESS)
    {
        WH_REPORT_FAILURE(result);
        return FALSE;
    }

    WH_ChangeSysState(WH_SYSSTATE_BUSY);

    if (!WH_StateInEnable())
    {
        WH_ChangeSysState(WH_SYSSTATE_FATAL);
        return FALSE;
    }
#endif

    return TRUE;
}


#ifndef WH_USE_DETAILED_INITIALIZE

static void WH_StateOutInitialize(void *arg)
{
    // State after power-on.
    WMErrCode result;
    WMCallback *cb = (WMCallback *)arg;
    WH_TRACE_STATE;

    if (cb->errcode != WM_ERRCODE_SUCCESS)
    {
        WH_REPORT_FAILURE(cb->errcode);
        WH_ChangeSysState(WH_SYSSTATE_FATAL);
        return;
    }
    // Sets the callback function to receive notifications that are generated irregularly.
    result = WM_SetIndCallback(sIndicationCallback);
    if (result != WM_ERRCODE_SUCCESS)
    {
        WH_REPORT_FAILURE(result);
        WH_ChangeSysState(WH_SYSSTATE_FATAL);
        return;
    }

    // Changes the system state to idling (waiting).
    WH_ChangeSysState(WH_SYSSTATE_IDLE);

    // Does not set the next state, so the sequence ends here.
    // If WH_Connect is called while in this state, it changes to the connection operation.
}

#else

/* ----------------------------------------------------------------------
   enable
  ---------------------------------------------------------------------- */
static BOOL WH_StateInEnable(void)
{
    // Makes the hardware usable. (Obtains the permission to use.)
    WMErrCode result;
    WH_TRACE_STATE;

    result = WM_Enable(WH_StateOutEnable);
    if (result != WM_ERRCODE_OPERATING)
    {
        WH_REPORT_FAILURE(result);
        WH_ChangeSysState(WH_SYSSTATE_FATAL);
        return FALSE;
    }
    return TRUE;
}

static void WH_StateOutEnable(void *arg)
{
    // When the hardware use is permitted, it changes to the power-on state.
    WMCallback *cb = (WMCallback *)arg;
    WH_TRACE_STATE;

    if (cb->errcode != WM_ERRCODE_SUCCESS)
    {
        WH_REPORT_FAILURE(cb->errcode);
        WH_ChangeSysState(WH_SYSSTATE_FATAL);
        return;
    }

    if (!WH_StateInPowerOn())
    {
        WH_ChangeSysState(WH_SYSSTATE_FATAL);
        return;
    }
}

/* ----------------------------------------------------------------------
   power on
  ---------------------------------------------------------------------- */
static BOOL WH_StateInPowerOn(void)
{
    // Since the wireless hardware became usable, start supplying power.
    WMErrCode result;
    WH_TRACE_STATE;

    result = WM_PowerOn(WH_StateOutPowerOn);
    if (result != WM_ERRCODE_OPERATING)
    {
        WH_REPORT_FAILURE(result);
        WH_ChangeSysState(WH_SYSSTATE_FATAL);
        return FALSE;
    }

    return TRUE;
}

static void WH_StateOutPowerOn(void *arg)
{
    // State after power-on.
    WMErrCode result;
    WMCallback *cb = (WMCallback *)arg;
    WH_TRACE_STATE;

    if (cb->errcode != WM_ERRCODE_SUCCESS)
    {
        WH_REPORT_FAILURE(cb->errcode);
        WH_ChangeSysState(WH_SYSSTATE_FATAL);
        return;
    }

    // Sets the callback function to receive notifications that are generated irregularly.
    result = WM_SetIndCallback(sIndicationCallback);
    if (result != WM_ERRCODE_SUCCESS)
    {
        WH_REPORT_FAILURE(result);
        WH_ChangeSysState(WH_SYSSTATE_FATAL);
        return;
    }

    // Changes the system state to idling (waiting).
    WH_ChangeSysState(WH_SYSSTATE_IDLE);

    // Does not set the next state, so the sequence ends here.
    // If WH_Connect is called while in this state, it changes to the connection operation.
}

#endif // #ifdef WH_USE_DETAILED_INITIALIZE


/* ----------------------------------------------------------------------
  Name:        WH_ParentConnect
  Description: Starts the connection sequence.
  Arguments:   mode    - If WH_CONNECTMODE_MP_PARENT, MP starts as a parent.
                         If WH_CONNECTMODE_DS_PARENT, DataSharing starts as a parent.
                         If WH_CONNECTMODE_KS_PARENT, KeySharing starts as a parent.
               tgid    - The parent device communications tgid
               channel - The parent device communications channel
  Returns:     TRUE if the connection sequence is successful
  ---------------------------------------------------------------------- */
BOOL WH_ParentConnect(int mode, u16 tgid, u16 channel)
{
    // If not in a standby state, the connection sequence cannot be started.
    WH_ASSERT(sSysState == WH_SYSSTATE_IDLE);

    // Calculation of send and receive buffer size for WM_StartMP()
    // To statically allocate a buffer in advance, use the WM_SIZE_MP_* function macro. If the buffer can be allocated dynamically, use the WM_GetReceiveBufferSize() API after the parent and child are connected and immediately before calling WM_StartMP().
    // 
    // 
    // Similarly, to statically allocate a buffer in advance, use the WM_SIZE_MP_* function macro. If the buffer can be allocated dynamically, use the WM_GetSendBufferSize() API after the parent and child are connected and immediately before calling WM_StartMP().
    // 
    // 
    sRecvBufferSize = WH_PARENT_RECV_BUFFER_SIZE;
    sSendBufferSize = WH_PARENT_SEND_BUFFER_SIZE;

    WH_TRACE("recv buffer size = %d\n", sRecvBufferSize);
    WH_TRACE("send buffer size = %d\n", sSendBufferSize);

    sConnectMode = mode;
    WH_ChangeSysState(WH_SYSSTATE_BUSY);

    sParentParam.tgid = tgid;
    sParentParam.channel = channel;
    sParentParam.beaconPeriod = WM_GetDispersionBeaconPeriod();
    sParentParam.parentMaxSize = WH_PARENT_MAX_SIZE;
    sParentParam.childMaxSize = WH_CHILD_MAX_SIZE;
    sParentParam.maxEntry = WH_CHILD_MAX;
    sParentParam.CS_Flag = 0;
    sParentParam.multiBootFlag = 0;
    sParentParam.entryFlag = 1;
    sParentParam.KS_Flag = (u16)((mode == WH_CONNECTMODE_KS_PARENT) ? 1 : 0);

    switch (mode)
    {
    case WH_CONNECTMODE_MP_PARENT:
    case WH_CONNECTMODE_KS_PARENT:
    case WH_CONNECTMODE_DS_PARENT:
        // Starts the connection in child mode.
        return WH_StateInSetParentParam();
    default:
        break;
    }

    WH_TRACE("unknown connect mode %d\n", mode);
    return FALSE;
}


/* ----------------------------------------------------------------------
  Name:        WH_ChildConnect
  Description: Starts the connection sequence.
  Arguments:   mode    - Start MP as the child if WH_CONNECTMODE_MP_CHILD.
                         Start DataSharing as the child if WH_CONNECTMODE_DS_CHILD.
                         Start KeySharing as the child if WH_CONNECTMODE_KS_CHILD.
               bssDesc - The bssDesc of the parent device to connect to.
                
  Returns:     TRUE if the connection sequence is successful
  ---------------------------------------------------------------------- */
BOOL WH_ChildConnect(int mode, WMBssDesc *bssDesc)
{
    // If not in a standby state, the connection sequence cannot be started.
    WH_ASSERT(sSysState == WH_SYSSTATE_IDLE);

    // Calculation of send and receive buffer size for WM_StartMP()
    // To statically allocate a buffer in advance, use the WM_SIZE_MP_* function macro. If the buffer can be allocated dynamically, use the WM_GetReceiveBufferSize() API after the parent and child are connected and immediately before calling WM_StartMP().
    // 
    // 
    // Similarly, to statically allocate a buffer in advance, use the WM_SIZE_MP_* function macro. If the buffer can be allocated dynamically, use the WM_GetSendBufferSize() API after the parent and child are connected and immediately before calling WM_StartMP().
    // 
    // 
    sRecvBufferSize = WH_CHILD_RECV_BUFFER_SIZE;
    sSendBufferSize = WH_CHILD_SEND_BUFFER_SIZE;

    WH_TRACE("recv buffer size = %d\n", sRecvBufferSize);
    WH_TRACE("send buffer size = %d\n", sSendBufferSize);

    sConnectMode = mode;
    WH_ChangeSysState(WH_SYSSTATE_BUSY);

    switch (mode)
    {
    case WH_CONNECTMODE_MP_CHILD:
    case WH_CONNECTMODE_KS_CHILD:
    case WH_CONNECTMODE_DS_CHILD:
        // Starts the connection in child mode.
        // Uses the saved BssDesc for the parent and connects without scanning.
        MI_CpuCopy8(bssDesc, &sBssDesc, sizeof(WMBssDesc));
        if (sChildWEPKeyGenerator != NULL)
        {
            // If the WEP Key Generator is configured, set the WEP Key
            return WH_StateInSetChildWEPKey();
        }
        else
        {
            return WH_StateInStartChild();
        }
    default:
        break;
    }

    WH_TRACE("unknown connect mode %d\n", mode);
    return FALSE;
}

/*---------------------------------------------------------------------------*
  Name:         WH_SetJudgeAcceptFunc

  Description:  Sets the functions used to determine whether to accept the child device connection.

  Arguments:    Set the child device connection determination functions.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WH_SetJudgeAcceptFunc(WHJudgeAcceptFunc func)
{
    sJudgeAcceptFunc = func;
}


/**************************************************************************
 * The functions below are for direct MP communications that use the WH_DATA_PORT port.
 **************************************************************************/

/* ----------------------------------------------------------------------
   Name:        WH_SetReceiver
   Description: Configures the data reception callback in the WH_DATA_PORT port.
   Arguments:   proc - The data reception callback
   Returns:     None.
   ---------------------------------------------------------------------- */
void WH_SetReceiver(WHReceiverFunc proc)
{
    sReceiverFunc = proc;
    if (WM_SetPortCallback(WH_DATA_PORT, WH_PortReceiveCallback, NULL) != WM_ERRCODE_SUCCESS)
    {
        WH_ChangeSysState(WH_SYSSTATE_ERROR);
        WH_TRACE("WM not Initialized\n");
    }
}

/* ----------------------------------------------------------------------
   Name:        WH_SendData
   Description: Starts sending data to the WH_DATA_PORT port.
               (For MP communications. There is no need to call this function while data sharing, etc.)
   Arguments:   size - The data size
   Returns:     Returns TRUE if starting the send is successful.
   ---------------------------------------------------------------------- */
BOOL WH_SendData(const void *data, u16 size, WHSendCallbackFunc callback)
{
    return WH_StateInSetMPData(data, size, callback);
}


/**************************************************************************
 * The functions below control data sharing communications.
 **************************************************************************/

/* ----------------------------------------------------------------------
   Name:        WH_GetKeySet
   Description: Reads the common key data.
   Arguments:   keyset - Designates the data storage destination
   Returns:     If it succeeds returns TRUE.
   ---------------------------------------------------------------------- */
BOOL WH_GetKeySet(WMKeySet *keyset)
{
    WMErrCode result;

    if (sSysState != WH_SYSSTATE_KEYSHARING)
    {
        WH_TRACE("WH_GetKeySet failed (invalid system state)\n");
        return FALSE;
    }

    if ((sConnectMode != WH_CONNECTMODE_KS_CHILD) && (sConnectMode != WH_CONNECTMODE_KS_PARENT))
    {
        WH_TRACE("WH_GetKeySet failed (invalid connect mode)\n");
        return FALSE;
    }

    result = WM_GetKeySet(&sWMKeySetBuf, keyset);
    if (result != WM_ERRCODE_SUCCESS)
    {
        WH_REPORT_FAILURE(result);
        return FALSE;
    }

    return TRUE;
}

/* ----------------------------------------------------------------------
   Name:        WH_GetSharedDataAdr
  Description: Calculates and gets the data address obtained from the machine with the designated aid from the shared data address.
                
   Arguments:   aid - The machine designation
   Returns:     NULL on failure.
   ---------------------------------------------------------------------- */
u16    *WH_GetSharedDataAdr(u16 aid)
{
    return WM_GetSharedDataAddress(&sDSInfo, &sDataSet, aid);
}

/* ----------------------------------------------------------------------
   Name:        WH_StepDS
   Description: Proceeds to the next synchronized data sharing process.
                If communication is performed for each frame, this function must also be called for each frame.
                
   Arguments:   data - The data to send
   Returns:     If it succeeds returns TRUE.
   ---------------------------------------------------------------------- */
BOOL WH_StepDS(const void *data)
{
    WMErrCode result;

    result = WM_StepDataSharing(&sDSInfo, (const u16 *)data, &sDataSet);

    if (result == WM_ERRCODE_NO_CHILD)
    {
        // A parent with no children (This can be made an error if desired)
        return TRUE;
    }

    if (result == WM_ERRCODE_NO_DATASET)
    {
        WH_TRACE("WH_StepDataSharing - Warning No DataSet\n");
        WH_SetError(result);
        return FALSE;
    }

    if (result != WM_ERRCODE_SUCCESS)
    {
        WH_REPORT_FAILURE(result);
        return FALSE;
    }

    return TRUE;
}


/**************************************************************************
 * The functions below end communications and transition to initialization states.
 **************************************************************************/

/* ----------------------------------------------------------------------
   Name:        WH_Reset
   Description: Starts the reset sequence.
                When this function is called, it resets regardless of the current state.
        It is used for forced recovery from errors.
   Arguments:   None.
   Returns:     None.
   ---------------------------------------------------------------------- */
void WH_Reset(void)
{
    WMErrCode result;

    if (sSysState == WH_SYSSTATE_DATASHARING)
    {
        result = WM_EndDataSharing(&sDSInfo);
        if (result != WM_ERRCODE_SUCCESS)
        {
            WH_REPORT_FAILURE(result);
        }
    }

    if (!WH_StateInReset())
    {
        WH_ChangeSysState(WH_SYSSTATE_FATAL);
    }
}

/* ----------------------------------------------------------------------
   Name:        WH_Finalize
   Description: Starts the post-processing / end sequence.
                When this function is called, the current state is referenced and an appropriate end sequence is executed.
                
                This function is used in the normal end process (not WH_Reset).
   Arguments:   None.
   Returns:     None.
   ---------------------------------------------------------------------- */
void WH_Finalize(void)
{
    if (sSysState == WH_SYSSTATE_IDLE)
    {
        WH_TRACE("already WH_SYSSTATE_IDLE\n");
        return;
    }

    WH_TRACE("WH_Finalize, state = %d\n", sSysState);

    if (sSysState == WH_SYSSTATE_SCANNING)
    {
        if (!WH_EndScan())
        {
            WH_Reset();
        }
        return;
    }


    if ((sSysState != WH_SYSSTATE_KEYSHARING) &&
        (sSysState != WH_SYSSTATE_DATASHARING) && (sSysState != WH_SYSSTATE_CONNECTED))
    {
        // Resets if there is no connection, there is an error, etc.
        WH_ChangeSysState(WH_SYSSTATE_BUSY);
        WH_Reset();
        return;
    }

    WH_ChangeSysState(WH_SYSSTATE_BUSY);

    switch (sConnectMode)
    {
    case WH_CONNECTMODE_KS_CHILD:
        if (!WH_StateInEndChildKeyShare())
        {
            WH_Reset();
        }
        break;

    case WH_CONNECTMODE_DS_CHILD:
        if (WM_EndDataSharing(&sDSInfo) != WM_ERRCODE_SUCCESS)
        {
            WH_Reset();
            break;
        }
        // don't break;

    case WH_CONNECTMODE_MP_CHILD:
        if (!WH_StateInEndChildMP())
        {
            WH_Reset();
        }
        break;

    case WH_CONNECTMODE_KS_PARENT:
        if (!WH_StateInEndParentKeyShare())
        {
            WH_Reset();
        }
        break;

    case WH_CONNECTMODE_DS_PARENT:
        if (WM_EndDataSharing(&sDSInfo) != WM_ERRCODE_SUCCESS)
        {
            WH_Reset();
            break;
        }
        // don't break;

    case WH_CONNECTMODE_MP_PARENT:
        if (!WH_StateInEndParentMP())
        {
            WH_Reset();
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         WH_End

  Description:  Ends wireless communications.

  Arguments:    None.

  Returns:      If it succeeds returns TRUE.
 *---------------------------------------------------------------------------*/
BOOL WH_End(void)
{
    WH_ASSERT(sSysState == WH_SYSSTATE_IDLE);

    WH_ChangeSysState(WH_SYSSTATE_BUSY);
    if (WM_End(WH_StateOutEnd) != WM_ERRCODE_OPERATING)
    {
        WH_ChangeSysState(WH_SYSSTATE_ERROR);

        return FALSE;
    }

    whInitialized = 0;

    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         WH_GetCurrentAid

  Description:  Acquires its own current AID.
                Children may change when they connect or disconnect.

  Arguments:    None.

  Returns:      AID value
 *---------------------------------------------------------------------------*/
u16 WH_GetCurrentAid(void)
{
    return sMyAid;
}

/*---------------------------------------------------------------------------*
  Name:         WH_SetParentWEPKeyGenerator

  Description:  Sets the function that generates WEP Key.
                If this function is called, WEP will be used for authentication during connection.
                Using a unique algorithm for each game application, it sets the parent and child to the same value before connection.
                
                This function is for parents.

  Arguments:    func    Pointer to the function that generates the WEP Key
                        If NULL is specified, WEP Key is not used

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WH_SetParentWEPKeyGenerator(WHParentWEPKeyGeneratorFunc func)
{
    sParentWEPKeyGenerator = func;
}

/*---------------------------------------------------------------------------*
  Name:         WH_SetChildWEPKeyGenerator

  Description:  Sets the function that generates WEP Key.
                If this function is called, WEP will be used for authentication during connection.
                Using a unique algorithm for each game application, it sets the parent and child to the same value before connection.
                
                This function is for children.

  Arguments:    func    Pointer to the function that generates the WEP Key
                        If NULL is specified, WEP Key is not used

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WH_SetChildWEPKeyGenerator(WHChildWEPKeyGeneratorFunc func)
{
    sChildWEPKeyGenerator = func;
}

/*---------------------------------------------------------------------------*
  Name:         WH_SetIndCallback

  Description:  Sets the callback function specified in the WM_SetIndCallback function called by WH_Initialize.
                
                This function should be called before WH_Initialize is called.
                If a callback function is not specified by this function, the default WH_IndicateHandler will be set as the callback.
                

  Arguments:    callback    Call back for the indication notification specified by WM_SetIndCallback.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WH_SetIndCallback(WMCallbackFunc callback)
{
    sIndicationCallback = callback;
}


#if defined(WM_DEMO_WIRELESS_SHARED_WC_WH_CONFIG_H_)
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// This is a wrapper layer that implements, in WH, the same features as the existing WC sample module.
// Existing interfaces made public in wc.h are supported unchanged.

// The major differences between WC and WH are as follows.
//
//   - WC specifies WMParentParam unchanged at initialization, but WH breaks up the parameters and specifies them as function arguments whenever a state transition occurs.
//     
//
//   - Past versions of WC were structured such that if the user changed the contents of the pointer given to WcInit(), the driver's internal state could become unstable. In the future, this type of behavior will be avoided and pointer contents will be internally duplicated.
//     
//     
//
//   - For WH, WH_SYSSTATE_IDLE (the IDLE state) indicates a steady wireless state. For WC, WC_STATUS_READY (the READY state) indicates a steady state.
//     
//     In the future, the IDLE state will be made the steady state for WC, as well.
//

/*---------------------------------------------------------------------------*
  Name:         WcIndicateCallback

  Description:  Indication callback from WH.

  Arguments:    arg:         WMCallback

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WcIndicateCallback(void *arg)
{
    WHCompatibleInfoForWC  *info = infoForWC;
    WMindCallback *cb = (WMindCallback *)arg;
    if (cb->errcode == WM_ERRCODE_FIFO_ERROR)
    {
        info->wcTarget = WC_STATUS_ERROR;
        info->wcStatus = WC_STATUS_ERROR;
    }
}

/*---------------------------------------------------------------------------*
  Name:         WcStateScheduler

  Description:  Executes actions that are currently possible for the target state.

  Arguments:    target:      The final state of the transition.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WcStateScheduler(WCStatus target)
{
    WHCompatibleInfoForWC  *info = infoForWC;
    // If there is no error, change the state to which to transition.
    if (info->wcStatus != WC_STATUS_ERROR)
    {
        info->wcTarget = target;
        // If possible, perform an action here.
        if ((info->wcStatus != WC_STATUS_BUSY) &&
            (info->wcStatus != target))
        {
            // State transition to the MP parent device.
            if (target == WC_STATUS_PARENT)
            {
                // If currently READY, MP can be started.
                if (info->wcStatus == WC_STATUS_READY)
                {
                    info->wcStatus = WC_STATUS_BUSY;
                    (void)WH_ParentConnect(info->useDS ? WH_CONNECTMODE_DS_PARENT : WH_CONNECTMODE_MP_PARENT,
                                           ++info->tmpTgid, info->targetChannel);
                }
                // If run as an MP child device, change to READY for a moment.
                else if (info->wcStatus == WC_STATUS_CHILD)
                {
                    info->wcStatus = WC_STATUS_BUSY;
                    WH_Reset();
                }
            }
            // State transition to the MP child device.
            else if (target == WC_STATUS_CHILD)
            {
                // If currently READY, MP can be started.
                if (info->wcStatus == WC_STATUS_READY)
                {
                    info->wcStatus = WC_STATUS_BUSY;
                    {
                        static const u8 any[] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,};
                        // Simple Pictochat operation.
                        if (info->pictoCatchCallback)
                        {
                            (void)WH_StartScan((WHStartScanCallbackFunc)info->pictoCatchCallback,
                                               any, 0);
                        }
                        // Actual MP child device.
                        else
                        {
                            (void)WH_ChildConnectAuto(info->useDS ? WH_CONNECTMODE_DS_CHILD : WH_CONNECTMODE_MP_CHILD,
                                                      any, info->parentParam.channel);
                        }
                    }
                }
                // If run as an MP parent device, change to READY for a moment.
                else if (info->wcStatus == WC_STATUS_PARENT)
                {
                    info->wcStatus = WC_STATUS_BUSY;
                    WH_Reset();
                }
            }
            // State transition to READY.
            else if (target == WC_STATUS_READY)
            {
                // At any rate, end if MP communication is being performed.
                if (info->wcStatus == WC_STATUS_PARENT)
                {
                    info->wcStatus = WC_STATUS_BUSY;
                    WH_Reset();
                }
                else if (info->wcStatus == WC_STATUS_CHILD)
                {
                    info->wcStatus = WC_STATUS_BUSY;
                    WH_Reset();
                }
            }
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         WcStateCallback

  Description:  A callback invoked whenever the WH state is updated.

  Arguments:    state:       The current state.
                prev:        The previous state.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WcStateCallback(int state, int prev)
{
    WHCompatibleInfoForWC  *info = infoForWC;
    BOOL                    needToNotify = TRUE;
    // General WM error.
    if (state == WH_SYSSTATE_ERROR)
    {
        // Terminate wireless if the child devices being communicated with were disconnected.
        if ((info->wcTarget == WC_STATUS_CHILD) &&
            (WH_GetLastError() == WH_ERRCODE_DISCONNECTED))
        {
            info->wcTarget = WC_STATUS_READY;
            if (info->wcStatus != WC_STATUS_BUSY)
            {
                WH_Reset();
            }
        }
        // Otherwise, there is a margin to insert a WH_Reset() and retry.
        else
        {
            WH_Reset();
        }
    }
    // Completed the transition to the MP state, as well as CONNECT.
    // NOTE: The second time is an MP notification.
    // WC state transition.
    else if ((state == WH_SYSSTATE_CONNECTED) &&
             (prev == WH_SYSSTATE_CONNECTED))
    {
        info->wcStatus = (info->wcTarget == WC_STATUS_PARENT) ?
                          WC_STATUS_PARENT : WC_STATUS_CHILD;
        WcStateScheduler(info->wcTarget);
    }
    // WM_StartDataSharing() complete.
    // WC state transition.
    else if (state == WH_SYSSTATE_DATASHARING)
    {
        info->wcStatus = (info->wcTarget == WC_STATUS_PARENT) ?
                          WC_STATUS_PARENT : WC_STATUS_CHILD;
        WcStateScheduler(info->wcTarget);
    }
    // Reached the IDLE state.
    // WC state transition.
    else if (state == WH_SYSSTATE_IDLE)
    {
        if (!sAutoConnectFlag)
        {
            info->wcStatus = WC_STATUS_READY;
            WcStateScheduler(info->wcTarget);
        }
    }
    // WM_StartConnect() failure.
    // CAUTION!
    //     Originally, processing for the number of retries must be done.
    // Insert WH_Reset() and retry.
    else if (state == WH_SYSSTATE_CONNECT_FAIL)
    {
        WH_Reset();
    }
    // WM initialization and deallocation, as well as indicator setting functions.
    // An error indicating a difficulty in continuing processing because of some failure.
    // WC_STATUS_ERROR handling.
    else if (state == WH_SYSSTATE_FATAL)
    {
        info->wcStatus = WC_STATUS_ERROR;
        WcStateScheduler(info->wcTarget);
    }
    // Finish termination processing.
    else if (state == WH_SYSSTATE_STOP)
    {
        info->wcStatus = WC_STATUS_READY;
    }
    // MeasureChannel completion.
    // Read only the value and immediately force a transition to IDLE.
    // This callback is recursively notified.
    else if (state == WH_SYSSTATE_MEASURECHANNEL)
    {
        info->targetChannel = WH_GetMeasureChannel();
        needToNotify = FALSE;
    }
    // Anything else is a notification that does not require anything in particular to be done.
    else
    {
        // - WH_SYSSTATE_BUSY
        // - WH_SYSSTATE_SCANNING
        // - WH_SYSSTATE_KEYSHARING
        needToNotify = FALSE;
    }
    // may be on standby in WcWaitForState(), so send a notification
    if (needToNotify)
    {
        OS_WakeupThread(info->queue);
    }
}

/*---------------------------------------------------------------------------*
  Name:         WcWaitForState

  Description:  Blocks until a transition to the specified state, or until the transition fails.
                This is only used in WcFinish().

  Arguments:    target:      State to which a transition must be done.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static BOOL WcWaitForState(int target)
{
    BOOL    succeeded = FALSE;
    OSIntrMode  bak_cpsr = OS_DisableInterrupts();
    for (;;)
    {
        int     state = WH_GetSystemState();
        if (state == target)
        {
            succeeded = TRUE;
            break;
        }
        // StartConnect failure.
        // Insert WH_Reset() and retry.
        // Here, we must add the fact that retries have been exceeded.
        else if (state == WH_SYSSTATE_CONNECT_FAIL)
        {
            break;
        }
        // WM_API overall errors, such as WM_StartParent.
        // Insert WH_Reset() and retry.
        else if (state == WH_SYSSTATE_ERROR)
        {
            break;
        }
        // Failure in initialization processing, end processing, or indicator setting.
        // WC_STATUS_ERROR handling.
        else if (state == WH_SYSSTATE_FATAL)
        {
            break;
        }
        OS_SleepThread(infoForWC->queue);
    }
    (void)OS_RestoreInterrupts(bak_cpsr);
    return succeeded;
}

SDK_WEAK_SYMBOL void WcInit(const WMParentParam *pp, BOOL dsFlag, BOOL btFlag, u16 framePeriod)
{
    WHCompatibleInfoForWC  *info = infoForWC;
    // Save information here that is used by WC state transitions.
    info->parentParam = *pp;
    info->useDS = dsFlag;
    (void)btFlag;   // not referenced
    info->targetChannel = info->parentParam.channel;
    OS_InitThreadQueue(info->queue);
    // set WH members, extended for WC
    sStateCallback = WcStateCallback;
    sDataSharingDoubleMode = (framePeriod <= 1);
    sPortExtraCallback = NULL;
    WH_SetGgid(info->parentParam.ggid);
    WH_SetIndCallback(WcIndicateCallback);
    // begin transitioning to the IDLE state, if necessary
    info->wcTarget = WC_STATUS_READY;
    if (whInitialized)
    {
        info->wcStatus = WC_STATUS_READY;
    }
    else
    {
        info->wcStatus = WC_STATUS_BUSY;
    }
    (void)WH_Initialize();
}

SDK_WEAK_SYMBOL void WcFinish(void)
{
    WHCompatibleInfoForWC  *info = infoForWC;
    {
        OSIntrMode  bak_cpsr = OS_DisableInterrupts();
        // Wait until WH is entirely finished.
        if (info->wcStatus == WC_STATUS_BUSY)
        {
            WH_Reset();
        }
        WcEnd();
        (void)WcWaitForState(WH_SYSSTATE_IDLE);
        info->wcStatus = WC_STATUS_BUSY;
        (void)WH_End();
        (void)WcWaitForState(WH_SYSSTATE_STOP);
        // revert the WH members that were extended for WC to their original values
        sStateCallback = NULL;
        sDataSharingDoubleMode = TRUE;
        sPortExtraCallback = NULL;
        sAutoConnectFlag = FALSE;
        (void)OS_RestoreInterrupts(bak_cpsr);
    }
}

SDK_WEAK_SYMBOL void WcStartParent(void)
{
    WcStateScheduler(WC_STATUS_PARENT);
}

SDK_WEAK_SYMBOL void WcStartChild(void)
{
    WcStateScheduler(WC_STATUS_CHILD);
}

SDK_WEAK_SYMBOL void WcEnd(void)
{
    WcStateScheduler(WC_STATUS_READY);
}

SDK_WEAK_SYMBOL WCStatus WcGetStatus(void)
{
    return infoForWC->wcStatus;
}

SDK_WEAK_SYMBOL u16 WcGetAid(void)
{
    return WH_GetCurrentAid();
}

SDK_WEAK_SYMBOL void WcSetMpReceiveCallback(WCCallbackFunc cb)
{
    WH_SetReceiver((WHReceiverFunc)cb);
}

SDK_WEAK_SYMBOL void WcSetBlockTransferCallback(WCBlockTransferCallbackFunc cb)
{
    sPortExtraCallback = (WMCallbackFunc)cb;
}

SDK_WEAK_SYMBOL void WcSetParentData(const void *buf, u16 size)
{
    (void)WH_SendData(buf, size, NULL);
}

SDK_WEAK_SYMBOL void WcSetChildData(const void *buf, u16 size)
{
    (void)WH_SendData(buf, size, NULL);
}

SDK_WEAK_SYMBOL BOOL WcStepDataSharing(const void *send, u16 *bitmap)
{
    BOOL    succeeded = WH_StepDS(send);
    if (succeeded && bitmap)
    {
        *bitmap = sDataSet.receivedBitmap;
    }
    return succeeded;
}

SDK_WEAK_SYMBOL u8 *WcGetSharedDataAddress(u16 aid)
{
    return (u8 *)WH_GetSharedDataAdr(aid);
}

SDK_WEAK_SYMBOL void WcTurnOnPictoCatch(WCPictoCatchCallbackFunc func)
{
    infoForWC->pictoCatchCallback = func;
    WH_TurnOnPictoCatch();
}

SDK_WEAK_SYMBOL void WcTurnOffPictoCatch(void)
{
    WH_TurnOffPictoCatch();
}

#endif // WM_DEMO_WIRELESS_SHARED_WC_WH_CONFIG_H_
