/*
  Project:  NitroSDK - wireless_shared - demos - wh
  File:     wh.h

  Copyright 2003-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: wh.h,v $
  Revision 1.25  2007/10/02 09:06:05  yosizaki
  Small fix specific to const-cast.

  Revision 1.24  2007/02/20 00:28:08  kitase_hirotake
  Indented source

  Revision 1.23  2006/10/26 06:56:01  kitase_hirotake
  Added WH_SetIndCallback

  Revision 1.22  2006/07/19 09:07:33  yosizaki
  Added WH_SetSsid.

  Revision 1.21  2006/04/10 13:19:13  yosizaki
  Added support for WH_MP_FREQUENCY.

  Revision 1.20  2006/01/18 02:12:39  kitase_hirotake
  do-indent

  Revision 1.19  2005/04/26 02:35:02  terui
  Fixed comment

  Revision 1.18  2005/03/25 03:05:28  seiki_masashi
  Added WH_Set{Parent,Child}WEPKeyGenerator to set WEPKey setting functions

  Revision 1.17  2005/03/04 09:18:56  takano_makoto
  Addition of WH_SetUserGameInfo

  Revision 1.16  2005/02/28 05:26:35  yosizaki
  do-indent.

  Revision 1.15  2004/12/22 02:49:08  terui
  Support for Pictochat Search

  Revision 1.14  2004/12/20 08:31:27  takano_makoto
  Made changes to shorten the initialization process by using WH_Initialize.

  Revision 1.13  2004/12/20 07:17:20  takano_makoto
  Changed WHReceiver to WHReceiverFunc, excluded the internal use of OS_Alloc, separated parameters to set for each application as wh_config.h.

  Revision 1.12  2004/11/16 03:01:11  takano_makoto
  Added the WH_GetCurrentAid function.

  Revision 1.11  2004/11/08 02:46:17  takano_makoto
  Prevented multiple calling of WM_EndScan

  Revision 1.10  2004/11/05 04:27:40  sasakis
  Added the lobby screen and parent selection screen, and made the necessary modifications (such as scan related).

  Revision 1.9  2004/11/02 19:36:19  takano_makoto
  Changed the return values of WH_StartMeasureChannel and WH_EndScan to BOOL

  Revision 1.8  2004/11/01 09:38:14  seiki_masashi
  Changed the WH_DS_DATA_SIZE from 14 to 12.
  Corrected comment

  Revision 1.7  2004/11/01 09:16:47  takano_makoto
  Changed the return value type of the WH functions, and added the WH_SYSSTATE_FATAL state.

  Revision 1.6  2004/10/29 02:16:34  takano_makoto
  Changed the WH_StartScan argument

  Revision 1.4  2004/10/28 07:11:20  takano_makoto
  Changed the WH_Connect function name

  Revision 1.3  2004/10/27 02:31:31  takano_makoto
  Integrated the multiple MeasureChannel-related functions
  Fixed a bug that was using WM_ERRCODE_SUCCESS to determine a section that should wait on WM_ERRCODE_OPERATING
  Fixed other bugs

  Revision 1.2  2004/10/25 02:29:54  seiki_masashi
  Applied WH_CHILD_MAX to the maximum send size between parent and child.
  Made the maximum send size for the parent and child into WH_{PARENT,CHILD}_MAX_SIZE
  Corrected comment

  Revision 1.1  2004/10/21 00:41:50  yosizaki
  Initial upload.

  $NoKeywords$
 */

/*
  Log : (from multiboot-model)
  Revision 1.6  2004/09/30 08:28:14  yosizaki
  Added SDK_MAKEGGID_SYSTEM macro.

  Revision 1.5  2004/09/30 03:58:40  takano_makoto
  Added WH_SYSSTATE_CONNECT_FAIL.

  Revision 1.4  2004/09/27 08:34:38  takano_makoto
  Changed interface for WH_ChildConnect and WH_Initialize.

  Revision 1.3  2004/09/17 14:12:45  takano_makoto
  Added a routine that determines connected children.

  Revision 1.2  2004/09/17 10:29:47  takano_makoto
  Defined WH_PARENT_MAX_SIZE, WH_CHILD_MAX_SIZE.

  Revision 1.1  2004/09/16 14:07:11  takano_makoto
  Initial Update
*/

/*
  Log : (from dataShare-model)
  Revision 1.15  2004/10/06 05:11:35  sasakis
  Addition of a graph display, etc.

  Revision 1.14  2004/10/05 09:21:57  seiki_masashi
  Made changes so that a callback is designated in WH_SendData.
  Added an error check to the function that converts the character strings of constants.

  Revision 1.13  2004/10/01 01:58:54  seiki_masashi
  Restored the normal MP communication-related functions.

  Revision 1.12  2004/09/30 08:46:48  seiki_masashi
  Configured the proper values allocated to the GGID.

  Revision 1.11  2004/09/30 06:26:49  seiki_masashi
  Temporarily deleted the normal MP communications-related functions.

  Revision 1.10  2004/09/30 06:20:03  seiki_masashi
  Changed an indent.

  Revision 1.9  2004/09/30 06:16:38  seiki_masashi
  Made changes to be compliant with the newest SDK specifications.

  Revision 1.8  2004/09/24 09:43:35  sasakis
  Made several small bug fixes.

  Revision 1.7  2004/09/24 06:12:35  sasakis
  Added a manual channel selection feature.

  Revision 1.6  2004/09/22 09:39:33  sasakis
  Added a debug screen.

  Revision 1.5  2004/09/15 10:23:40  sasakis
  Added a radio reception strength icon, modifications to make the data size adjustable, etc.

  Revision 1.4  2004/09/10 08:44:52  sasakis
  Added a guideline compliance process, made error-related reconsiderations, etc.

  Revision 1.3  2004/09/08 08:55:35  sasakis
  Slightly reinforced the disconnect process.

  Revision 1.2  2004/09/08 01:52:52  sasakis
  Error / disconnection related improvements.

  Revision 1.1  2004/09/07 04:59:42  sasakis
  Initial version registration.
*/


#ifndef __WMHIGH_H__
#define __WMHIGH_H__

#include "wh_config.h"

/* added from NITRO-SDK3.1RC (prior to that is always 1) */
#if !defined(WH_MP_FREQUENCY)
#define WH_MP_FREQUENCY   1
#endif

// OFF if initializing by using WM_Initialize
// Set to ON if there is a need to individually use and precisely control WM_Init, WM_Enable, and WM_PowerOn.
// #define WH_USE_DETAILED_INITIALIZE

enum
{
    WH_SYSSTATE_STOP,                  // Initial state
    WH_SYSSTATE_IDLE,                  // Waiting
    WH_SYSSTATE_SCANNING,              // Scanning
    WH_SYSSTATE_BUSY,                  // Connecting
    WH_SYSSTATE_CONNECTED,             // Completed connection (Communications are possible in this state)
    WH_SYSSTATE_DATASHARING,           // Completed connection, with data-sharing enabled
    WH_SYSSTATE_KEYSHARING,            // Completed connection, with key-sharing enabled
    WH_SYSSTATE_MEASURECHANNEL,        // Check the radio wave usage rate of the channel
    WH_SYSSTATE_CONNECT_FAIL,          // Connection to the parent device failed
    WH_SYSSTATE_ERROR,                 // Error occurred
    WH_SYSSTATE_FATAL,                 // A FATAL error has occurred
    WH_SYSSTATE_NUM
};

enum
{
    WH_CONNECTMODE_MP_PARENT,          // Parent device MP connection mode
    WH_CONNECTMODE_MP_CHILD,           // Child device MP connection mode
    WH_CONNECTMODE_KS_PARENT,          // Parent device key-sharing connection mode
    WH_CONNECTMODE_KS_CHILD,           // Child device key-sharing connection mode
    WH_CONNECTMODE_DS_PARENT,          // Parent device data-sharing connection mode
    WH_CONNECTMODE_DS_CHILD,           // Child device data-sharing connection mode
    WH_CONNECTMODE_NUM
};

enum
{
    // Separate error codes
    WH_ERRCODE_DISCONNECTED = WM_ERRCODE_MAX,   // Disconnected from parent
    WH_ERRCODE_PARENT_NOT_FOUND,       // No parent
    WH_ERRCODE_NO_RADIO,               // Wireless cannot be used
    WH_ERRCODE_LOST_PARENT,            // Parent lost
    WH_ERRCODE_NOMORE_CHANNEL,         // Scan has finished on all channels
    WH_ERRCODE_MAX
};

typedef void (*WHStartScanCallbackFunc) (WMBssDesc *bssDesc);

/* Parent device receive buffer size */
#define WH_PARENT_RECV_BUFFER_SIZE  WM_SIZE_MP_PARENT_RECEIVE_BUFFER( WH_CHILD_MAX_SIZE, WH_CHILD_MAX, FALSE )
/* Parent device send buffer size */
#define WH_PARENT_SEND_BUFFER_SIZE  WM_SIZE_MP_PARENT_SEND_BUFFER( WH_PARENT_MAX_SIZE, FALSE )

/* Child device receive buffer size */
#define WH_CHILD_RECV_BUFFER_SIZE   WM_SIZE_MP_CHILD_RECEIVE_BUFFER( WH_PARENT_MAX_SIZE, FALSE )
/* Child device send buffer size */
#define WH_CHILD_SEND_BUFFER_SIZE   WM_SIZE_MP_CHILD_SEND_BUFFER( WH_CHILD_MAX_SIZE, FALSE )

/* Macro that defines the GGID reserved for the SDK sample demo */
#define SDK_MAKEGGID_SYSTEM(num)    (0x003FFF00 | (num))

/* Bitmap value that shows a not connected or a parent device only state */
#define WH_BITMAP_EMPTY           1

// Send callback type (for data-sharing model)
typedef void (*WHSendCallbackFunc) (BOOL result);

// Function type for making connection permission determinations (for multiboot model)
typedef BOOL (*WHJudgeAcceptFunc) (WMStartParentCallback *);

// Receive callback type
typedef void (*WHReceiverFunc) (u16 aid, u16 *data, u16 size);

// Function for generating a WEP key
typedef u16 (*WHParentWEPKeyGeneratorFunc) (u16 *wepkey, const WMParentParam *parentParam);
typedef u16 (*WHChildWEPKeyGeneratorFunc) (u16 *wepkey, const WMBssDesc *bssDesc);



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
extern void WH_SetGgid(u32 ggid);

/*---------------------------------------------------------------------------*
  Name:         WH_SetSsid

  Description:  Sets the SSID specified during child connection.
                Call before making a connection from the child device.

  Arguments:    ssid    Buffer in which the SSID to be set has been stored.
                length  data length of the SSID to be set.
                        If less than WM_SIZE_CHILD_SSID (24 byte)
                        Fill the subsequent white space with 0 if less than WM_SIZE_CHILD_SSID (24 byte), and truncate if exceeding it.

  Returns:      None.
 *---------------------------------------------------------------------------*/
extern void WH_SetSsid(const void *ssid, u32 length);

/*---------------------------------------------------------------------------*
  Name:         WH_SetUserGameInfo

  Description:  Configures user-defined parent device information
                Call before making a connection to the parent device.

  Arguments:    userGameInfo   pointer to user-defined parent device information
                length         the size of the user-defined parent device information

  Returns:      None.
 *---------------------------------------------------------------------------*/
extern void WH_SetUserGameInfo(u16 *userGameInfo, u16 length);

/*---------------------------------------------------------------------------*
  Name:         WH_SetDebugOutput

  Description:  Configures the function for outputting the debug character string.

  Arguments:    func    Function for the set debug character string output.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    WH_SetDebugOutput(void (*func) (const char *, ...));

/*---------------------------------------------------------------------------*
  Name:         WH_SetParentWEPKeyGenerator

  Description:  Sets the function that generates WEP Key.
                If this function is called, WEP will be used for authentication during connection.
                Using a unique algorithm for each game application,
                it sets the parent and child to the same value before connection.
                This function is for parents.

  Arguments:    func    Pointer to the function that generates the WEP Key
                        If NULL is specified, WEP Key is not used

  Returns:      None.
 *---------------------------------------------------------------------------*/
extern void WH_SetParentWEPKeyGenerator(WHParentWEPKeyGeneratorFunc func);

/*---------------------------------------------------------------------------*
  Name:         WH_SetChildWEPKeyGenerator

  Description:  Sets the function that generates WEP Key.
                If this function is called, WEP will be used for authentication during connection.
                Using a unique algorithm for each game application,
                it sets the parent and child to the same value before connection.
                This function is for children.

  Arguments:    func    Pointer to the function that generates the WEP Key
                        If NULL is specified, WEP Key is not used

  Returns:      None.
 *---------------------------------------------------------------------------*/
extern void WH_SetChildWEPKeyGenerator(WHChildWEPKeyGeneratorFunc func);

/*---------------------------------------------------------------------------*
  Name:         WH_SetIndCallback

  Description:  Sets the callback function specified in the WM_SetIndCallback function called by WH_Initialize.
                Specifies the callback function.
                This function should be called before WH_Initialize is called.
                If a callback function is not specified by this function, the default WH_IndicateHandler will be set to callback.
                WH_IndicateHandler is set as the callback.

  Arguments:    callback    Call back for the indication notification specified by WM_SetIndCallback.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    WH_SetIndCallback(WMCallbackFunc callback);

/**************************************************************************
 * The wrapper functions below get the low layer WM library states.
 **************************************************************************/

/* ----------------------------------------------------------------------
  Name:        WH_GetLinkLevel
  Description: Gets the radio reception strength.
  Arguments:   None.
  Returns:     Returns the numeric value of WMLinkLevel.
  ---------------------------------------------------------------------- */
extern int WH_GetLinkLevel(void);

/* ----------------------------------------------------------------------
   Name:        WH_GetAllowedChannel
   Description: Gets the bit pattern of channels that can be used for connection.
   Arguments:   None.
   Returns:     channel pattern
   ---------------------------------------------------------------------- */
extern u16 WH_GetAllowedChannel(void);


/**************************************************************************
 * The functions below take the WH state.
 **************************************************************************/

/* ----------------------------------------------------------------------
   Name:        WH_GetBitmap
   Description: Gets the bit pattern for displaying the connection state.
   Arguments:   None.
   Returns:     bitmap pattern
   ---------------------------------------------------------------------- */
extern u16 WH_GetBitmap(void);

/* ----------------------------------------------------------------------
   Name:        WH_GetSystemState
   Description: Gets the WH internal state.
   Arguments:   None.
   Returns:     The internal state (WH_SYSSTATE_XXXX).
   ---------------------------------------------------------------------- */
extern int WH_GetSystemState(void);

/* ----------------------------------------------------------------------
   Name:        WH_GetConnectMode
   Description: Gets the connection information.
   Arguments:   None.
   Returns:     The connection information (WH_CONNECTMODE_XX_XXXX).
   ---------------------------------------------------------------------- */
extern int WH_GetConnectMode(void);

/* ----------------------------------------------------------------------
   Name:        WH_GetLastError
   Description: Gets the error code that most recently occurred.
   Arguments:   None.
   Returns:     The error code.
   ---------------------------------------------------------------------- */
extern int WH_GetLastError(void);

/*---------------------------------------------------------------------------*
  Name:         WH_PrintBssDesc

  Description:  Debug outputs the members of the WMBssDesc structure.

  Arguments:    info    The pointer to the BssDesc to be debug output.

  Returns:      None.
 *---------------------------------------------------------------------------*/
extern void WH_PrintBssDesc(WMBssDesc *info);


/**************************************************************************
 * The functions below perform channel-related processes.
 **************************************************************************/

/*---------------------------------------------------------------------------*
  Name:         WH_StartMeasureChannel

  Description:  Starts measuring the radio wave usage rate on every usable channel.
                When measurement is complete, internally calculates the channel with the lowest usage rate, and transitions it to the WH_SYSSTATE_MEASURECHANNEL state.
                

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
extern BOOL WH_StartMeasureChannel(void);

/*---------------------------------------------------------------------------*
  Name:         WH_GetMeasureChannel

  Description:  Returns the channel with the lowest usage rate from all the usable channels.
                The WH_MeasureChannel operations must be complete and it must be in a WH_SYSSTATE_MEASURECHANNEL state.
                
                When this function is called, the channel with the lowest use rate is returned
                and the WH_SYSSTATE_IDLE state is set.
                
  Arguments:    None.

  Returns:      The channel number of the usable channel with the lowest usage rate
 *---------------------------------------------------------------------------*/
extern u16 WH_GetMeasureChannel(void);


/**************************************************************************
 * Functions initializing wireless and switching to a communication enabled state are given below.
 **************************************************************************/

/* ----------------------------------------------------------------------
   Name:        WH_Initialize
   Description: Performs initialization operations, and starts the initialization sequence.
   Arguments:   None.
   Returns:     TRUE if the sequence start was a success.
   ---------------------------------------------------------------------- */
extern BOOL WH_Initialize(void);


/*---------------------------------------------------------------------------*
  Name:         WH_TurnOnPictoCatch

  Description:  Enables the Pictochat Search feature.
                This will cause the callback function to be invoked even if Pictochat was found while scanning with WH_StartScan.
                

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
extern void WH_TurnOnPictoCatch(void);

/*---------------------------------------------------------------------------*
  Name:         WH_TurnOffPictoCatch

  Description:  Disables the Pictochat Search feature.
                This makes it to be ignored, even if pictochat was found while scanning with WH_StartScan.
                

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
extern void WH_TurnOffPictoCatch(void);

/*---------------------------------------------------------------------------*
  Name:         WH_StartScan

  Description:  Function for fetching the parent beacon

  Arguments:    callback - Sets the callback returned when the parent device is found.
                
                macAddr  - Specifies the MAC address for the connected parent
                           If 0xFFFFFF, all parents are searched for.
                           
                channel - Designate the channels on which to search for the parent.
                           If 0, all channels are searched.

  Returns:      None.
 *---------------------------------------------------------------------------*/
extern BOOL WH_StartScan(WHStartScanCallbackFunc callback, const u8 *macAddr, u16 channel);

/*---------------------------------------------------------------------------*
  Name:         WH_EndScan

  Description:  Function for fetching the parent beacon

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
extern BOOL WH_EndScan(void);

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
extern BOOL WH_ParentConnect(int mode, u16 tgid, u16 channel);

/* ----------------------------------------------------------------------
  Name:        WH_ChildConnect
  Description: Starts the connection sequence.
  Arguments:   mode    - Start MP as the child if WH_CONNECTMODE_MP_CHILD.
                         Start DataSharing as the child if WH_CONNECTMODE_DS_CHILD.
                         Start KeySharing as the child if WH_CONNECTMODE_KS_CHILD.
               bssDesc - The bssDesc of the parent device to connect to.
               
  Returns:     TRUE if the connection sequence is successful
  ---------------------------------------------------------------------- */
extern BOOL WH_ChildConnect(int mode, WMBssDesc *bssDesc);

/* ----------------------------------------------------------------------
   Name:        WH_ChildConnectAuto
   Description: Starts the child device connection sequence.
                This is specified with WH_ParentConnect or WH_ChildConnect.
                The settings are left to automatic internal processing.
   Arguments:   mode    - Start MP as the child if WH_CONNECTMODE_MP_CHILD.
                          Start DataSharing as the child if WH_CONNECTMODE_DS_CHILD.
                          Start KeySharing as the child if WH_CONNECTMODE_KS_CHILD.

                macAddr  - Specifies the MAC address for the connected parent
                          If 0xFFFFFF, all parents are searched for.
                          
                channel - Designate the channels on which to search for the parent.
                          If 0, all channels are searched.

  Returns:     TRUE if the connection sequence is successful
   ---------------------------------------------------------------------- */
extern BOOL WH_ChildConnectAuto(int mode, const u8 *macAddr, u16 channel);

/*---------------------------------------------------------------------------*
  Name:         WH_SetJudgeAcceptFunc

  Description:  Sets the functions used to determine whether to accept the child device connection.

  Arguments:    Set the child device connection determination functions.

  Returns:      None.
 *---------------------------------------------------------------------------*/
extern void WH_SetJudgeAcceptFunc(WHJudgeAcceptFunc func);


/**************************************************************************
 * The functions below are for direct MP communications that use the WH_DATA_PORT port.
 **************************************************************************/

/* ----------------------------------------------------------------------
   Name:        WH_SetReceiver
   Description: Configures the data reception callback in the WH_DATA_PORT port.
   Arguments:   proc - The data reception callback
   Returns:     None.
   ---------------------------------------------------------------------- */
extern void WH_SetReceiver(WHReceiverFunc proc);

/* ----------------------------------------------------------------------
   Name:        WH_SendData
   Description: Starts sending data to the WH_DATA_PORT port.
               (For MP communications. There is no need to call this function while data sharing, etc.)
   Arguments:   size - The data size
   Returns:     Returns TRUE if starting the send is successful.
   ---------------------------------------------------------------------- */
extern BOOL WH_SendData(const void *data, u16 datasize, WHSendCallbackFunc callback);


/**************************************************************************
 * The functions below control data sharing communications.
 **************************************************************************/

/* ----------------------------------------------------------------------
   Name:        WH_GetKeySet
   Description: Reads the common key data.
   Arguments:   keyset - Designates the data storage destination
   Returns:     If it succeeds returns TRUE.
   ---------------------------------------------------------------------- */
extern BOOL WH_GetKeySet(WMKeySet *keyset);

/* ----------------------------------------------------------------------
   Name:        WH_GetSharedDataAdr
  Description: Calculates and gets the data address obtained from the machine with
                the designated aid from the shared data address.
   Arguments:   aid - The machine designation
   Returns:     NULL on failure.
   ---------------------------------------------------------------------- */
extern u16 *WH_GetSharedDataAdr(u16 aid);

/* ----------------------------------------------------------------------
   Name:        WH_StepDS
   Description: Proceeds to the next synchronized data sharing process.
                If communication is performed for each frame, this function must also be called for each frame.
                
   Arguments:   data - The data to send
   Returns:     If it succeeds returns TRUE.
   ---------------------------------------------------------------------- */
extern BOOL WH_StepDS(const void *data);


/**************************************************************************
 * The functions below end communications and transition to initialization states.
 **************************************************************************/

/* ----------------------------------------------------------------------
   Name:        WH_Reset
   Description: Starts the reset sequence.
                When this function is called, it resets regardless of the current state.
        It is used for forced recovery from errors.
   Arguments:   None.
   Returns:     TRUE if the process successfully starts
   ---------------------------------------------------------------------- */
extern void WH_Reset(void);

/* ----------------------------------------------------------------------
   Name:        WH_Finalize
   Description: Starts the post-processing / end sequence.
                When this function is called, the current state is referenced and
                an appropriate end sequence is executed.
                This function is used in the normal end process (not WH_Reset).
   Arguments:   None.
   Returns:     None.
   ---------------------------------------------------------------------- */
extern void WH_Finalize(void);

/*---------------------------------------------------------------------------*
  Name:         WH_End

  Description:  Ends wireless communications.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
extern BOOL WH_End(void);

/*---------------------------------------------------------------------------*
  Name:         WH_GetCurrentAid

  Description:  Acquires its own current AID.
                Children may change when they connect or disconnect.

  Arguments:    None.

  Returns:      AID value
 *---------------------------------------------------------------------------*/
extern u16 WH_GetCurrentAid(void);

#endif
