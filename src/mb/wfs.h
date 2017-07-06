/*---------------------------------------------------------------------------*
  Project:  NitroSDK - wireless_shared - demos - wfs
  File:     wfs.h

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: wfs.h,v $
  Revision 1.5  05/19/2006 06:42:06  yosizaki
  fix source indent.

  Revision 1.4  01/18/2006 02:12:39  kitase_hirotake
  do-indent

  Revision 1.3  05/09/2005 04:30:58  yosizaki
  add comment of WFS_End().

  Revision 1.2  2005/02/28 05:26:34  yosizaki
  do-indent.

  Revision 1.1  11/16/2004 01:13:04  yosizaki
  moved from /demos/wbt/wbt-fs.


 -[old log at /demos/wbt/wbt-fs/wfs]-

  Revision 1.9  10/25/2004 04:20:14  yosizaki
  add WFS_InitParent and WFS_InitChild.
  add some getter functions.

  Revision 1.8  09/29/2004 07:28:03  yosizaki
  change WFS_Init.

  Revision 1.7  09/29/2004 07:10:11  yosizaki
  merge from branch.

  Revision 1.6  09/15/2004 01:11:12  yosizaki
  add WFS_SetPacketSize().

  Revision 1.5  09/11/2004 04:13:35  yosizaki
  add comments.
  unify MP-size definition.

  Revision 1.4  09/04/2004 13:25:40  yosizaki
  add WFS_GetCurrentDownloadProgress.

  Revision 1.3  09/04/2004 01:45:14  yosizaki
  delete obsolete codes and waste parameters.

  Revision 1.2  09/03/2004 10:10:04  yosizaki
  enable WFS_OBSOLETE.

  Revision 1.1  09/03/2004 01:12:10  yosizaki
  (none)

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef	NITRO_BUILD_DEMOS_WIRELESSSHARED_WFS_INCLUDE_WFS_H_
#define	NITRO_BUILD_DEMOS_WIRELESSSHARED_WFS_INCLUDE_WFS_H_


#include <nitro.h>

#ifdef __cplusplus
extern  "C" {
#endif


/*****************************************************************************/
/* constant */


/*
 * Enumerator type showing internal state of WFS.
 */
typedef enum
{
    WFS_STATE_STOP,
    WFS_STATE_IDLE,
    WFS_STATE_READY,
    WFS_STATE_ERROR
}
WFSState;


/*****************************************************************************/
/* prototype */

/*
 * The callback called every time the internal state of the WFS changes.
 * The FS automatically becomes available when WFS_STATE_READY is returned.
 */
typedef void (*WFSStateCallback) (void *);


/*
 * The dynamic memory allocation callback internal to WFS.
 * arg is  user defined argument.
 * If ptr is NULL, return memory of size byte.
 * If ptr is not NULL, release ptr.
 */
typedef void *(*WFSAllocator) (void *arg, u32 size, void *ptr);


/*****************************************************************************/
/* function */

/*---------------------------------------------------------------------------*
  Name:         WFS_InitParent

  Description:  Initializes wireless file system (WFS) as parent.
                Since this configures and automatically activates the port callback to the WM library,
                this must be called once WM is in a READY state.

  Arguments:    port             Port number used for internal MP communication.
                callback         Callback that receives various state notifications.
                allocator        Callback used for internal memory allocation.
                                 The amount of memory the WFS uses is decided each time
                                 application runs.
                allocator_arg    User-defined argument given to allocator.
                parent_packet    Default parent send size.
                                 This value must be WBT_PACKET_SIZE_MIN or greater, and
                                 less than parent maximum send size.
                                 A child device will be simply ignored.
                p_rom            The file pointer indicating the program that includes
                                 the FAT/FNT/OVT  provided to the child device.
                                 Normally, this designates the child device program
                                 sent via wireless download.
                                 Seek position maintains position before the call.
                                 When this argument is NULL, provides parent's own
                                 file system.
                use_parent_fs    When TRUE, provides parent's own FAT/FNT, not
                                 program specified with p-rom.
                                 With these configurations, an independent parent/child program
                                 can share a single file system.
                                 If p_rom is NULL, this setting is ignored, and sharing always
                                 occurs.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    WFS_InitParent(int port, WFSStateCallback callback,
                       WFSAllocator allocator, void *allocator_arg, int parent_packet,
                       FSFile *p_rom, BOOL use_parent_fs);

/*---------------------------------------------------------------------------*
  Name:         WFS_InitChild

  Description:  Initializes wireless file system (WFS) as child.
                Since this configures and automatically activates the port callback to the WM library,
                this must be called once WM is in a READY state.

  Arguments:    port             Port number used for internal MP communication.
                callback         Callback that receives various state notifications.
                allocator        Callback used for internal memory allocation.
                                 The amount of memory the WFS uses is decided each time
                                 application runs.
                allocator_arg    User-defined argument given to allocator.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    WFS_InitChild(int port, WFSStateCallback callback,
                      WFSAllocator allocator, void *allocator_arg);

/*---------------------------------------------------------------------------*
  Name:         WFS_Init

  Description:  Initializes wireless file system (WFS).
                After initializing internal state as parent or child,
                Since this configures and automatically activates the port callback to the WM library,
                this must be called once WM is in a READY state.

  Arguments:    is_parent        If wireless parent, TRUE. If child, FALSE.
                                 For actual multiboot, this argument
                                 is given !MB_IsMultiBootChild().
                port             Port number used for internal MP communication.
                parent_packet    Default parent send size.
                                 This value must be WBT_PACKET_SIZE_MIN or greater, and
                                 less than parent maximum send size.
                                 A child device will be simply ignored.
                callback         Callback that receives various state notifications.
                allocator        Callback used for internal memory allocation.
                                 The amount of memory the WFS uses is decided each time
                                 application runs.
                allocator_arg    User-defined argument given to allocator.

  Returns:      None.
 *---------------------------------------------------------------------------*/
SDK_INLINE void WFS_Init(BOOL is_parent, int port, int parent_packet,
                         WFSStateCallback callback, WFSAllocator allocator, void *allocator_arg)
{
    if (is_parent)
        WFS_InitParent(port, callback, allocator, allocator_arg, parent_packet, NULL, TRUE);
    else
        WFS_InitChild(port, callback, allocator, allocator_arg);
}

/*---------------------------------------------------------------------------*
  Name:         WFS_Start

  Description:  For WFS, sends notification that wireless has entered MP state.
                After this notification, WFS uses WM_SetMPDataToPort() and runs
                automatically with "Priority: low (WM_PRIORITY_LOW)".
                For this reason, it must be called in MP_PARENT or MP_CHILD
                state.
                Normally, this is called in the WM_StartMP() callback after
                WM_STATECODE_MP_START is passed to the parameter types.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    WFS_Start(void);

/*---------------------------------------------------------------------------*
  Name:         WFS_End

  Description:  Called when WFS is not needed.
                Releases all internally allocated memory and returns to state before initialization.
                Normally, this is called when all wireless communication has been disconnected.
                This function cannot be called from the interrupt handler.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    WFS_End(void);

/*---------------------------------------------------------------------------*
  Name:         WFS_GetStatus

  Description:  Gets current internal state of WFS in WFSState type.

  Arguments:    None.

  Returns:      One of WFSState-type enumerated values.
 *---------------------------------------------------------------------------*/
WFSState WFS_GetStatus(void);

/*---------------------------------------------------------------------------*
  Name:         WFS_GetCurrentBitmap

  Description:  Gets the current child device group bitmap that WBT is aware of.
                This function can only be called by parent.

  Arguments:    None.

  Returns:      Currently recognized child devices.
 *---------------------------------------------------------------------------*/
int     WFS_GetCurrentBitmap(void);

/*---------------------------------------------------------------------------*
  Name:         WFS_GetSyncBitmap

  Description:  Gets bitmap of child group with accessd sync specified.
                This function can only be called by parent.

  Arguments:    None.

  Returns:      Child devices that are specified to synchronize access
 *---------------------------------------------------------------------------*/
int     WFS_GetSyncBitmap(void);

/*---------------------------------------------------------------------------*
  Name:         WFS_GetBusyBitmap

  Description:  Gets bitmap of child devices that are currently accessing parent.
                This function can only be called by parent.

  Arguments:    None.

  Returns:      Child devices that are currently accessing parent.
 *---------------------------------------------------------------------------*/
int     WFS_GetBusyBitmap(void);

/*---------------------------------------------------------------------------*
  Name:         WFS_IsBusy

  Description:  Determines whether or not the child device of a designated aid is being accessed.
                This function can only be called by parent.

  Arguments:    aid              aid of child to check.

  Returns:      If the designated child device is being accessed, TRUE. Otherwise, FALSE.
 *---------------------------------------------------------------------------*/
BOOL    WFS_IsBusy(int aid);

/*---------------------------------------------------------------------------*
  Name:         WFS_GetCurrentDownloadProgress

  Description:  Gets the progress status of the file that child is currently performing ReadFile with.
                The unit will be the WBT internal sequence number.
                This function can only be called by children.

  Arguments:    current_count    Stores the number of currently received sequences
                                 Variable address
                total_count      Stores the total number of sequences that should be received
                                 Variable address

  Returns:      If this is the current ReadFile state and the correct progress can be obtained, TRUE.
                Otherwise, returns FALSE.
 *---------------------------------------------------------------------------*/
BOOL    WFS_GetCurrentDownloadProgress(int *current_count, int *total_count);

/*---------------------------------------------------------------------------*
  Name:         WFS_GetPacketSize

  Description:  Gets MP communication packet size of parent set by WFS.

  Arguments:    None.

  Returns:      MP communication packet size of parent set by WFS.
 *---------------------------------------------------------------------------*/
int     WFS_GetPacketSize(void);

/*---------------------------------------------------------------------------*
  Name:         WFS_SetPacketSize

  Description:  Sets parent MP communication packet size.
                If this value is increased, it will improve transmission efficiency. If it is decreased,
                it will be bundled with separate port communications such as data sharing on single MP network communications,
                and one-sided slowdowns can be avoided via the port priority.
                This function can only be called by parent.

  Arguments:    size             Parent send size to set.
                                 This value must be WBT_PACKET_SIZE_MIN or greater, and
                                 less than parent maximum send size.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    WFS_SetPacketSize(int size);

/*---------------------------------------------------------------------------*
  Name:         WFS_EnableSync

  Description:  Configures the settings of the child device group that takes access synchronization on the parent device side.
                By synchronizing the response towards child device groups for which
                the access of identical file groups is clearly guaranteed, this function can
                make use of the WBT library attributes to achieve efficient transmissions.
                However, if the synchronization start timing is not logically safe,
                you will need to watch for the response to the child device group being off and deadlocks.
                This function can only be called by parent.

  Arguments:    sync_bitmap      The aid bitmap of the child device group that takes the access synchronization.
                                 The lowest bit 1 indicating the parent device itself is ignored.
                                 By assigning 0 for this value, synchronicity does not occur.
                                 This state by default.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    WFS_EnableSync(int sync_bitmap);

/*---------------------------------------------------------------------------*
  Name:         WFS_DisableSync

  Description:  The parent device releases access synchronization among child devices.
                This function is equivalent to specifying WFS_EnableSync() 0.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
SDK_INLINE void WFS_DisableSync(void)
{
    WFS_EnableSync(0);
}

/*---------------------------------------------------------------------------*
  Name:         WFS_SetDebugMode

  Description:  Enables/Disables WFS internal debug output.
                The default setting is FALSE.

  Arguments:    enable_debug     TRUE if debug output is enabled. FALSE if disabled.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    WFS_SetDebugMode(BOOL enable_debug);


/*****************************************************************************/


#ifdef __cplusplus
} /* extern "C" */
#endif


#endif /* NITRO_BUILD_DEMOS_WIRELESSSHARED_WFS_INCLUDE_WFS_H_ */
