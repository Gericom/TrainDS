/*---------------------------------------------------------------------------*
  Project:  NitroSDK - wireless_shared - demos - mbp
  File:     mbp.h


  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: mbp.h,v $
  Revision 1.5  01/18/2006 02:11:30  kitase_hirotake
  do-indent

  Revision 1.4  2005/02/28 05:26:25  yosizaki
  do-indent.

  Revision 1.3  12/03/2004 08:13:32  takano_makoto
  Revised MPB_USING_MB_EX to MBP_USING_MB_EX; added the MBP_USING_PREVIOUS_DOWNLOAD switch

  Revision 1.2  11/24/2004 06:15:12  yosizaki
  rename MB_StartParentEx/MB_EndEx

  Revision 1.1  11/12/2004 09:02:39  yosizaki
  (none)

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef NITROSDK_DEMO_WIRELESSSHARED_MBP_H_
#define NITROSDK_DEMO_WIRELESSSHARED_MBP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <nitro/mb.h>


/******************************************************************************/
/* constant */

/*
 * Game group ID
 *
 * Because specification of GGID has been enabled for individual files
 * in the MBGameRegistry structure, specification with MB_Init()
 intsance  xite
 */

/*
 * Channel which the parent distributes
 *
 * The multi-boot child cycles through all possible channels, so any of the values permitted by the WM library (currently 3, 8, and 13) can be used.
 *
 * Make this variable to avoid communication congestion in the user application.
 * The timing of channel changes, will be left to the application's discretion. However, it is also possible, for example, to create an opportunity for the user to select a message such as "Restart"in response to poor responsiveness.
 */

/* maximum number of connectable child devices */
#define MBP_CHILD_MAX       (15)

/* DMA number to allocate to the MB library */
#define MBP_DMA_NO          (2)

/*
 * Switch that uses MB_StartParentFromIdle() / MB_EndToIdle() in MB library.
 * It is possible to go through the multiboot process in IDLE state.
 */
#define MBP_USING_MB_EX

/*
 * Switch to start data transmission in advance for the child which has entered.
 * Because it starts the data transmission immediately after the entry, so
 * the wait time for download can be shortened when starting before the number of children reaches the MAX number.
 */
#define MBP_USING_PREVIOUS_DOWNLOAD


/* MB parent device state */
typedef struct
{
    u16     state;                     // Parent state
    u16     connectChildBmp;           // Flag for all children that are connected
    u16     requestChildBmp;           // Flag for children that are requesting entry
    u16     entryChildBmp;             // Flag for children that are waiting for data
    u16     downloadChildBmp;          // Flag for children that are downloading data
    u16     bootableChildBmp;          // Flag for children that has completed downloading
    u16     rebootChildBmp;            // Flag for children that sent boot
}
MBPState;


/* connected child device state */
typedef struct
{
    MBUserInfo user;
    u8      macAddress[6];
    u16     playerNo;
}
MBPChildInfo;


/* the game sequence state value of this demo */
enum
{
    MBP_STATE_STOP,                    // Stop state
    MBP_STATE_IDLE,                    // Idle state (Init done)
    MBP_STATE_ENTRY,                   // Child entry accepting
    MBP_STATE_DATASENDING,             // MP data transmitting
    MBP_STATE_REBOOTING,               // Child rebooting
    MBP_STATE_COMPLETE,                // Child reboot completed
    MBP_STATE_CANCEL,                  // Multiboot cancellation processing
    MBP_STATE_ERROR,                   // Error occurred
    MBP_STATE_NUM
};

/* type of connected state bitmap */
typedef enum
{
    MBP_BMPTYPE_CONNECT,               // Connected child information
    MBP_BMPTYPE_REQUEST,               // Information of the child requesting connection
    MBP_BMPTYPE_ENTRY,                 // Information of the child waiting for downloading after the entry
    MBP_BMPTYPE_DOWNLOADING,           // Information on the child that is downloading
    MBP_BMPTYPE_BOOTABLE,              // Bootable child
    MBP_BMPTYPE_REBOOT,                // Reboot completed child
    MBP_BMPTYPE_NUM
}
MBPBmpType;

/* multiboot  child device state */
typedef enum
{
    MBP_CHILDSTATE_NONE,               // No connection
    MBP_CHILDSTATE_CONNECTING,         // connecting
    MBP_CHILDSTATE_REQUEST,            // Requesting connection
    MBP_CHILDSTATE_ENTRY,              // Undergoing entry
    MBP_CHILDSTATE_DOWNLOADING,        // Downloading
    MBP_CHILDSTATE_BOOTABLE,           // Boot waiting
    MBP_CHILDSTATE_REBOOT,             // Rebooting
    MBP_CHILDSTATE_NUM
}
MBPChildState;


/******************************************************************************/
/* variable */

/* parent device initialization */
void    MBP_Init(u32 ggid, u16 tgid);
void    MBP_Start(const MBGameRegistry *gameInfo, u16 channel);

/* parent device main process in each single frame */
const MBPState *MBP_Main(void);

void    MBP_KickChild(u16 child_aid);
void    MBP_AcceptChild(u16 child_aid);
void    MBP_StartRebootAll(void);
void    MBP_StartDownload(u16 child_aid);
void    MBP_StartDownloadAll(void);
BOOL    MBP_IsBootableAll(void);

void    MBP_Cancel(void);

u16     MBP_GetState(void);
u16     MBP_GetChildBmp(MBPBmpType bmpType);
void    MBP_GetConnectState(MBPState * state);
const u8 *MBP_GetChildMacAddress(u16 aid);
MBPChildState MBP_GetChildState(u16 aid);
u16     MBP_GetPlayerNo(const u8 *macAddress);
const MBPChildInfo *MBP_GetChildInfo(u16 child_aid);


#ifdef __cplusplus
}/* extern "C" */
#endif

#endif // NITROSDK_DEMO_WIRELESSSHARED_MBP_H_
