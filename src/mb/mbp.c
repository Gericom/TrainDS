/*---------------------------------------------------------------------------*
  Project:  NitroSDK - wireless_shared - demos - mbp
  File:     mbp.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: mbp.c,v $
  Revision 1.10  01/18/2006 02:11:30  kitase_hirotake
  do-indent

  Revision 1.9  11/22/2005 03:17:54  yosizaki
  fix about MBP_STATE_REBOOTING state.

  Revision 1.8  04/08/2005 05:47:55  yosizaki
  Corrected bitmap judgment

  Revision 1.7  2005/02/28 05:26:25  yosizaki
  do-indent.

  Revision 1.6  02/18/2005 10:38:30  yosizaki
  fix around hidden warnings.

  Revision 1.5  12/03/2004 08:13:32  takano_makoto
  Revised MPB_USING_MB_EX to MBP_USING_MB_EX; added the MBP_USING_PREVIOUS_DOWNLOAD switch

  Revision 1.4  11/24/2004 06:15:12  yosizaki
  rename MB_StartParentEx/MB_EndEx

  Revision 1.3  11/18/2004 07:57:41  takano_makoto
  Got rid of the argument for setting child transfer size in MB_SetParentCommSize, MB_SetParentCommParam

  Revision 1.2  11/12/2004 11:57:35  yosizaki
  add option for cloneboot.

  Revision 1.1  11/12/2004 09:02:39  yosizaki
  (none)

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include "common.h"
#include <nitro/wm.h>
#include <nitro/mb.h>



#include "mbp.h"




//============================================================================
//  Prototype Declaration
//============================================================================

static BOOL MBP_RegistFile(const MBGameRegistry *gameInfo);
static inline void MBP_AddBitmap(u16 *pBitmap, u16 aid);
static inline void MBP_RemoveBitmap(u16 *pBitmap, u16 aid);
static inline void MBP_DisconnectChild(u16 aid);
static inline void MBP_DisconnectChildFromBmp(u16 aid);
static void ParentStateCallback(u16 child_aid, u32 status, void *arg);
static void MBP_ChangeState(u16 state);


// Because the callback is executed by an IRQ, if multiple processes are executed in the callback, it is safer to set the IRQ stack size in the lcf file a bit on the large size.
//
//
// Because OS_Printf() in particular consumes a large amount of stack, try using the light-version, OS_TPrintf(), as much as possible in callbacks.
//

#define MBP_DEBUG
#if defined( MBP_DEBUG )
#define MBP_Printf          OS_TPrintf //
#else
#define MBP_Printf(...)     ((void)0)
#endif



//============================================================================
//  Variable definitions
//============================================================================

// Child connection information
static MBPState mbpState;
static MBPChildInfo childInfo[MBP_CHILD_MAX];

/* the work region to be allocated to the MB library */
static u32 *sCWork = NULL;

/* buffer for child device startup binary transmission */
static u8 *sFilebuf = NULL;

//============================================================================
//  function definitions
//============================================================================

/*---------------------------------------------------------------------------*
  Name:         MBP_Init

  Description:  Initializes the multiboot parent information.

  Arguments:    ggid  Specifies the parent's GGID when distributing a game
                tgid  Specifies the parent's TGID when distributing a game

  Returns:      None.
 *---------------------------------------------------------------------------*/
void MBP_Init(u32 ggid, u16 tgid)
{
    /* for the configuration of the parent device information the be displayed to the child device screen */
    MBUserInfo myUser;

    /* The type of value that must be set as parent information to displayed to the MB child screen is not strictly decided. *
     * Since the command was not completed, this is not a true error.                           *
     * Here, user information registered in IPL is set as MB parent information, but the established name may be set within the game.     *
     * However, this must be set as 2-byte code.            */
#if ( SDK_NVRAM_FORMAT >= 100 )
    OSOwnerInfo info;

    OS_GetOwnerInfo(&info);
    myUser.favoriteColor = info.favoriteColor;
    myUser.nameLength = (u8)info.nickNameLength;
    MI_CpuCopy8(info.nickName, myUser.name, (u32)(info.nickNameLength * 2));
#else  // For old IPL
    NVRAMConfig *info = (NVRAMConfig *)(OS_GetSystemWork()->nvramUserInfo);

#define DEFAULT_FAVORIT_COLOR    1
    myUser.favoriteColor = DEFAULT_FAVORIT_COLOR;
    myUser.nameLength = (u8)info->ncd.owner.nickname.length;
    MI_CpuCopy8(info->ncd.owner.nickname.name, myUser.name, NVRAM_CONFIG_NICKNAME_LENGTH * 2);
#endif

    myUser.playerNo = 0;               // The parent is number 0.

    // Initialize the status information
    mbpState = (const MBPState)
    {
    MBP_STATE_STOP, 0, 0, 0, 0, 0, 0};

    /* begin MB parent device control */
    // Allocate the MB work area.
#if !defined(MBP_USING_MB_EX)
    sCWork = mb_alloc(MB_SYSTEM_BUF_SIZE);
#else
    /* A small work size does not matter if WM initialization has been finished externally */
    sCWork = mb_alloc(MB_SYSTEM_BUF_SIZE - WM_SYSTEM_BUF_SIZE);
#endif

    if (MB_Init(sCWork, &myUser, ggid, tgid, MBP_DMA_NO) != MB_SUCCESS)
    {
        OS_Panic("ERROR in MB_Init\n");
    }

    // Set the maximum number of children to be connected (set a value that excludes the parent)
    (void)MB_SetParentCommParam(MB_COMM_PARENT_SEND_MIN, MBP_CHILD_MAX);

    MB_CommSetParentStateCallback(ParentStateCallback);

    MBP_ChangeState(MBP_STATE_IDLE);
}


/*---------------------------------------------------------------------------*
  Name:         MBP_Start

  Description:  Starts multiboot parent

  Arguments:    gameInfo Distribution binary information
                channel  Channel to be used

  Returns:      None.
 *---------------------------------------------------------------------------*/
void MBP_Start(const MBGameRegistry *gameInfo, u16 channel)
{
    SDK_ASSERT(MBP_GetState() == MBP_STATE_IDLE);

    MBP_ChangeState(MBP_STATE_ENTRY);
#if !defined(MBP_USING_MB_EX)
    if (MB_StartParent(channel) != MB_SUCCESS)
#else
    /* transfer the WM library to an IDLE state, and directly begin */
    if (MB_StartParentFromIdle(channel) != MB_SUCCESS)
#endif
    {
        MBP_Printf("MB_StartParent fail\n");
        MBP_ChangeState(MBP_STATE_ERROR);
        return;
    }

    /* --------------------------------------------------------------------- *
     * even if the binary was registered with MB_RegisterFile() before MB_StartParent().
     * Make sure to register MB_RegisterFile() after MB_StartParent().
     * --------------------------------------------------------------------- */

    /* register the download program file information */
    if (!MBP_RegistFile(gameInfo))
    {
        OS_Panic("Illegal multiboot gameInfo\n");
    }
}


/*---------------------------------------------------------------------------*
  Name:         MBP_RegistFile

  Description:  Registers the multiboot binary

  Arguments:    gameInfo Pointer to the multiboot binary information.
                         Of these members, if romFilePathp is NULL,
                         it operates the same as when clone boot is specified.

  Returns:      Returns TRUE if the file open was successful.
                If failed, returns FALSE.
 *---------------------------------------------------------------------------*/
static BOOL MBP_RegistFile(const MBGameRegistry *gameInfo)
{
    FSFile  file, *p_file;
    u32     bufferSize;
    BOOL    ret = FALSE;

    /* --------------------------------------------------------------------- *
     * Will be initialized when MB_StartParent() is called
     * even if the binary was registered with MB_RegisterFile() before MB_StartParent().
     * Make sure to register MB_RegisterFile() after MB_StartParent().
     * --------------------------------------------------------------------- */

    /*
     * As per specifications, this function operates as clone boot
     * if romFilePathp is NULL.
     * If not, it handles the specified file as a child program.
     */
    if (!gameInfo->romFilePathp)
    {
        p_file = NULL;
    }
    else
    {
        /* 
         * FS_ReadFile() must be able to read the program file.
         *  Although there is no problem because programs should usually be saved as files on the CARD-ROM,  if we consider the possibility of a special multi-boot system, it can be supported by building an original archive using FSArchive.
         * 
         *  
         */
        FS_InitFile(&file);
        if (!FS_OpenFile(&file, gameInfo->romFilePathp))
        {
            /* cannot open file */
            OS_Warning("Cannot Register file\n");
            return FALSE;
        }
        p_file = &file;
    }

    /*
     * Obtains the size of the segment information.
     * If it is not the valid download program,
     * 0 is returned for the size.
     */
    bufferSize = MB_GetSegmentLength(p_file);
    if (bufferSize == 0)
    {
        OS_Warning("specified file may be invalid format.\"%s\"\n",
                   gameInfo->romFilePathp ? gameInfo->romFilePathp : "(cloneboot)");
    }
    else
    {
        /*
         * Allocates memory that loads the segment information of the download program.
         * If the file registration succeeded, this area will be used until MB_End() is called.
         * This memory can be secured statically as long as the size is large enough. *
         */
        sFilebuf = mb_alloc(bufferSize);
        if (sFilebuf == NULL)
        {
            /* failed to allocate buffer that stores segment information */
            OS_Warning("can't allocate Segment buffer size.\n");
        }
        else
        {
            /* 
             * Extract the segment information from the file
             * The extracted segment information must stay resident on main memory
             * while the download program is being distributed.
             */
            if (!MB_ReadSegment(p_file, sFilebuf, bufferSize))
            {
                /* 
                 * Segment extraction failed due to invalid file.
                 * In the application-side bug, "The request contents differ even though synchronization was specified",
                 * This happens when a some kind of change has been made to the file handle.
                 * (File was closed, seek was performed for the location, ...)
                 */
                OS_Warning(" Can't Read Segment\n");
            }
            else
            {
                /*
                 * Register the download program with
                 * the extracted segment information and MBGameRegistry.
                 */
                if (!MB_RegisterFile(gameInfo, sFilebuf))
                {
                    /* registration failed because of incorrect program information */
                    OS_Warning(" Illegal program info\n");
                }
                else
                {
                    /* process completed correctly */
                    ret = TRUE;
                }
            }
            if (!ret)
				NNS_FndFreeToExpHeap(gHeapHandle, sFilebuf);
        }
    }

    /* close file if not clone boot */
    if (p_file == &file)
    {
        (void)FS_CloseFile(&file);
    }

    return ret;
}


/*---------------------------------------------------------------------------*
  Name:         MBP_AcceptChild

  Description:  Accepts a request from a child.

  Arguments:    child_aid       aid of a connected child.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void MBP_AcceptChild(u16 child_aid)
{
    if (!MB_CommResponseRequest(child_aid, MB_COMM_RESPONSE_REQUEST_ACCEPT))
    {
        // If the request failed, disconnect that child.
        MBP_DisconnectChild(child_aid);
        return;
    }

    MBP_Printf("accept child %d\n", child_aid);
}


/*---------------------------------------------------------------------------*
  Name:         MBP_KickChild

  Description:  Kicks the connected child.

  Arguments:    child_aid       aid of a connected child.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void MBP_KickChild(u16 child_aid)
{
    if (!MB_CommResponseRequest(child_aid, MB_COMM_RESPONSE_REQUEST_KICK))
    {
        // If the request failed, disconnect that child.
        MBP_DisconnectChild(child_aid);
        return;
    }

    {
        OSIntrMode enabled = OS_DisableInterrupts();

        mbpState.requestChildBmp &= ~(1 << child_aid);
        mbpState.connectChildBmp &= ~(1 << child_aid);

        (void)OS_RestoreInterrupts(enabled);
    }
}


/*---------------------------------------------------------------------------*
  Name:         MBP_StartDownload

  Description:  Sends the download start signals to the child.

  Arguments:    child_aid      aid of a connected child.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void MBP_StartDownload(u16 child_aid)
{
    if (!MB_CommStartSending(child_aid))
    {
        // If the request failed, disconnect that child.
        MBP_DisconnectChild(child_aid);
        return;
    }

    {
        OSIntrMode enabled = OS_DisableInterrupts();

        mbpState.entryChildBmp &= ~(1 << child_aid);
        mbpState.downloadChildBmp |= 1 << child_aid;

        (void)OS_RestoreInterrupts(enabled);
    }
}

/*---------------------------------------------------------------------------*
  Name:         MBP_StartDownloadAll

  Description:  Starts the data transfer to all of the children currently being entered.
                After this, it does not accept entries from children.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void MBP_StartDownloadAll(void)
{
    u16     i;

    // Stop accepting entries.
    MBP_ChangeState(MBP_STATE_DATASENDING);

    for (i = 1; i < 16; i++)
    {
        if (!(mbpState.connectChildBmp & (1 << i)))
        {
            continue;
        }

#ifdef MBP_USING_PREVIOUS_DOWNLOAD
        /* when starting to send data immediately after entry */
        // Disconnect children that are not being entered.
        if (MBP_GetChildState(i) == MBP_CHILDSTATE_CONNECTING)
        {
            MBP_DisconnectChild(i);
            continue;
        }
#else
        /* when starting to send data to all child devices at once */
        if (mbpState.requestChildBmp & (1 << i))
        {
            // Processed after the child currently being entered are ready
            continue;
        }

        // Disconnect children that are not being entered.
        if (!(mbpState.entryChildBmp & (1 << i)))
        {
            MBP_DisconnectChild(i);
            continue;
        }

        // Children that are being entered start downloading
        MBP_StartDownload(i);
#endif
    }
}

/*---------------------------------------------------------------------------*
  Name:         MBP_IsBootableAll

  Description:  Checks to see if all the connected child devices
                are in a bootable state.

  Arguments:    None.

  Returns:      TRUE if all of the children are bootable.
                FALSE if there is at least one child that is not bootable.
 *---------------------------------------------------------------------------*/
BOOL MBP_IsBootableAll(void)
{
    u16     i;

    if (mbpState.connectChildBmp == 0)
    {
        return FALSE;
    }

    for (i = 1; i < 16; i++)
    {
        if (!(mbpState.connectChildBmp & (1 << i)))
        {
            continue;
        }

        if (!MB_CommIsBootable(i))
        {
            return FALSE;
        }
    }
    return TRUE;
}


/*---------------------------------------------------------------------------*
  Name:         MBP_StartRebootAll

  Description:  Boots all of the connected children that are bootable.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void MBP_StartRebootAll(void)
{
    u16     i;
    u16     sentChild = 0;

    for (i = 1; i < 16; i++)
    {
        if (!(mbpState.bootableChildBmp & (1 << i)))
        {
            continue;
        }
        if (!MB_CommBootRequest(i))
        {
            // If the request failed, disconnect that child.
            MBP_DisconnectChild(i);
            continue;
        }
        sentChild |= (1 << i);
    }

    // If the connected child became 0, it generates an error and stops.
    if (sentChild == 0)
    {
        MBP_ChangeState(MBP_STATE_ERROR);
        return;
    }

    // Changes the state and stops accepting data transfer.
    MBP_ChangeState(MBP_STATE_REBOOTING);
}


/*---------------------------------------------------------------------------*
  Name:         MBP_Cancel

  Description:  Cancels multiboot.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void MBP_Cancel(void)
{
    MBP_ChangeState(MBP_STATE_CANCEL);
#if !defined(MBP_USING_MB_EX)
    MB_End();
#else
    MB_EndToIdle();
#endif
}

/*---------------------------------------------------------------------------*
  Name:         MBPi_CheckAllReboot

  Description:  Determines whether all connected children are in a MB_COMM_PSTATE_BOOT_STARTABLE state,
                and if they are, it starts the MB library end operation.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void MBPi_CheckAllReboot(void)
{
    if ((mbpState.state == MBP_STATE_REBOOTING) &&
        (mbpState.connectChildBmp == mbpState.rebootChildBmp))
    {
        MBP_Printf("call MB_End()\n");
#if !defined(MBP_USING_MB_EX)
        MB_End();
#else
        MB_EndToIdle();
#endif
    }
}

/*---------------------------------------------------------------------------*
  Name:         ParentStateCallback

  Description:  Parent state notification callback

  Arguments:    child_aid       aid of the child
                status          Callback status
                arg             Option argument
  Returns:      None.
 *---------------------------------------------------------------------------*/
static void ParentStateCallback(u16 child_aid, u32 status, void *arg)
{
#if defined( MBP_DEBUG )
    // For debug output
    static const char *MB_CALLBACK_TABLE[] = {
        "MB_COMM_PSTATE_NONE",
        "MB_COMM_PSTATE_INIT_COMPLETE",
        "MB_COMM_PSTATE_CONNECTED",
        "MB_COMM_PSTATE_DISCONNECTED",
        "MB_COMM_PSTATE_KICKED",
        "MB_COMM_PSTATE_REQ_ACCEPTED",
        "MB_COMM_PSTATE_SEND_PROCEED",
        "MB_COMM_PSTATE_SEND_COMPLETE",
        "MB_COMM_PSTATE_BOOT_REQUEST",
        "MB_COMM_PSTATE_BOOT_STARTABLE",
        "MB_COMM_PSTATE_REQUESTED",
        "MB_COMM_PSTATE_MEMBER_FULL",
        "MB_COMM_PSTATE_END",
        "MB_COMM_PSTATE_ERROR",
        "MB_COMM_PSTATE_WAIT_TO_SEND",
    };
#endif

    MBP_Printf("get callback %s %d\n", MB_CALLBACK_TABLE[status], child_aid);

    switch (status)
    {
        //-----------------------------------------------------------
        // Notification that the initialization process completed with the parent
    case MB_COMM_PSTATE_INIT_COMPLETE:
        // None.
        break;

        //-----------------------------------------------------------
        // Notification of the moment the child came for connection.
    case MB_COMM_PSTATE_CONNECTED:
        {
            // Connection is not accepted if the parent is not in entry accepting state.
            if (MBP_GetState() != MBP_STATE_ENTRY)
            {
                break;
            }

            MBP_AddBitmap(&mbpState.connectChildBmp, child_aid);
            // Saves a child's MacAddress
            WM_CopyBssid(((WMStartParentCallback *)arg)->macAddress,
                         childInfo[child_aid - 1].macAddress);

            childInfo[child_aid - 1].playerNo = child_aid;
        }
        break;

        //-----------------------------------------------------------
        // Notification that the child device has terminated connection
    case MB_COMM_PSTATE_DISCONNECTED:
        {
            // Deletes the entry if the it was disconnected by the condition other than rebooting of a child.
            if (MBP_GetChildState(child_aid) != MBP_CHILDSTATE_REBOOT)
            {
                MBP_DisconnectChildFromBmp(child_aid);
                // Based on that, it determines whether everyone has booted.
                MBPi_CheckAllReboot();
            }
        }
        break;

        //-----------------------------------------------------------
        // Notification of the moment that the entry request came from a child
    case MB_COMM_PSTATE_REQUESTED:
        {
            const MBUserInfo *userInfo;

            userInfo = (MBUserInfo *)arg;

            OS_TPrintf("callback playerNo = %d\n", userInfo->playerNo);

            // The child that requested entry is kicked without checking
            // if the parent was not in entry accepting state.
            if (MBP_GetState() != MBP_STATE_ENTRY)
            {
                MBP_KickChild(child_aid);
                break;
            }

            // Accepts child's entry.
            mbpState.requestChildBmp |= 1 << child_aid;

            MBP_AcceptChild(child_aid);

            // UserInfo is not set at the timing of MB_COMM_PSTATE_CONNECTED,
            // and it does not mean anything unless MB_CommGetChildUser was called in state after REQUESTED.
            userInfo = MB_CommGetChildUser(child_aid);
            if (userInfo != NULL)
            {
                MI_CpuCopy8(userInfo, &childInfo[child_aid - 1].user, sizeof(MBUserInfo));
            }
            MBP_Printf("playerNo = %d\n", userInfo->playerNo);
        }
        break;

        //-----------------------------------------------------------
        // Notify ACK for ACCEPT to a child.
    case MB_COMM_PSTATE_REQ_ACCEPTED:
        // No processing needed.
        break;
        //-----------------------------------------------------------
        // Notify when received the download request from a child.
    case MB_COMM_PSTATE_WAIT_TO_SEND:
        {
            // Changes the status of a child from entered to download standby.
            // Since this is the process in the interrupt, so change it without prohibiting interrupt.
            mbpState.requestChildBmp &= ~(1 << child_aid);
            mbpState.entryChildBmp |= 1 << child_aid;

            // Starts the data transmission when MBP_StartDownload() is called from main routine.
            // If it is already in data transmission state, it starts the data transmission to the child.
#ifdef MBP_USING_PREVIOUS_DOWNLOAD
            /* when starting to send data immediately after entry */
            MBP_StartDownload(child_aid);
#else
            /* when starting to send data to all child devices at once */
            if (MBP_GetState() == MBP_STATE_DATASENDING)
            {
                MBP_StartDownload(child_aid);
            }
#endif
        }
        break;

        //-----------------------------------------------------------
        // Notify when kicking a child
    case MB_COMM_PSTATE_KICKED:
        // None.
        break;

        //-----------------------------------------------------------
        // Notify when starting the binary transmission to a child.
    case MB_COMM_PSTATE_SEND_PROCEED:
        // None.
        break;

        //-----------------------------------------------------------
        // Notify when the binary transmission to the child completed.
    case MB_COMM_PSTATE_SEND_COMPLETE:
        {
            // Since this is the process in the interrupt, so change it without prohibiting interrupt.
            mbpState.downloadChildBmp &= ~(1 << child_aid);
            mbpState.bootableChildBmp |= 1 << child_aid;
        }
        break;

        //-----------------------------------------------------------
        // Notify of a child when boot had completed properly.
    case MB_COMM_PSTATE_BOOT_STARTABLE:
        {
            // Since this is the process in the interrupt, so change it without prohibiting interrupt.
            mbpState.bootableChildBmp &= ~(1 << child_aid);
            mbpState.rebootChildBmp |= 1 << child_aid;

            // When all of the children completed booting, starts the reconnection process with the parent.
            MBPi_CheckAllReboot();
        }
        break;

        //-----------------------------------------------------------
        // Notify when the boot request transmission to the target child has started.
        // It does not return as a callback.
    case MB_COMM_PSTATE_BOOT_REQUEST:
        // None.
        break;

        //-----------------------------------------------------------
        // Notify when the member became full.
    case MB_COMM_PSTATE_MEMBER_FULL:
        // None.
        break;

        //-----------------------------------------------------------
        // Notify when ending multiboot.
    case MB_COMM_PSTATE_END:
        {
            if (MBP_GetState() == MBP_STATE_REBOOTING)
                // When reboot process ends, it ends MB and reconnects with a child.
            {
                MBP_ChangeState(MBP_STATE_COMPLETE);
            }
            else
                // Shut down completed, and restore the STOP state.
            {
                MBP_ChangeState(MBP_STATE_STOP);
            }

            // Clears the buffer used for distributing games.
            // At the point when the callback of MB_COMM_PSTATE_END returns, work will be released, so it's fine to execute Free.
            // 
            if (sFilebuf)
            {
				NNS_FndFreeToExpHeap(gHeapHandle, sFilebuf);
                sFilebuf = NULL;
            }
            if (sCWork)
            {
				NNS_FndFreeToExpHeap(gHeapHandle, sCWork);
                sCWork = NULL;
            }

            /* MB_UnregisterFile can be omitted because it is initialized by calling MB_End and releasing work memory. **/

        }
        break;

        //-----------------------------------------------------------
        // Notify that the error occurred.
    case MB_COMM_PSTATE_ERROR:
        {
            MBErrorStatus *cb = (MBErrorStatus *)arg;

            switch (cb->errcode)
            {
                //------------------------------
                // Notification level error. No process is necessary.
            case MB_ERRCODE_WM_FAILURE:
                // None.
                break;
                //------------------------------
                // Error at the level that requires wireless to be reset.
            case MB_ERRCODE_INVALID_PARAM:
            case MB_ERRCODE_INVALID_STATE:
            case MB_ERRCODE_FATAL:
                MBP_ChangeState(MBP_STATE_ERROR);
                break;
            }
        }
        break;

        //-----------------------------------------------------------
    default:
        OS_Panic("Get illegal parent state.\n");
    }
}


/*---------------------------------------------------------------------------*
  Name:         MBP_ChangeState

  Description:  Changes the parent state.

  Arguments:    state       State to change

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void MBP_ChangeState(u16 state)
{
#if defined( MBP_DEBUG )
    // For debug output
    static const char *STATE_NAME[] = {
        "MBP_STATE_STOP",
        "MBP_STATE_IDLE",
        "MBP_STATE_ENTRY",
        "MBP_STATE_DATASENDING",
        "MBP_STATE_REBOOTING",
        "MBP_STATE_COMPLETE",
        "MBP_STATE_CANCEL",
        "MBP_STATE_ERROR"
    };
#endif

    SDK_ASSERT(state < MBP_STATE_NUM);

    MBP_Printf("%s -> %s\n", STATE_NAME[mbpState.state], STATE_NAME[state]);
    mbpState.state = state;
}


/*---------------------------------------------------------------------------*
  Name:         MBP_GetState

  Description:  Obtains the parent state

  Arguments:    None.

  Returns:      Parent state
 *---------------------------------------------------------------------------*/
u16 MBP_GetState(void)
{
    return mbpState.state;
}

/*---------------------------------------------------------------------------*
  Name:         MBP_GetConnectState

  Description:  Obtains the connection information.

  Arguments:    Buffer area for obtaining the current connection state.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void MBP_GetConnectState(MBPState * state)
{
    MI_CpuCopy8(&mbpState, state, sizeof(MBPState));
}

/*---------------------------------------------------------------------------*
  Name:         MBP_GetChildBmp

  Description:  Obtains the connection bit map of a child.

  Arguments:    Type of the connection bit map to obtain.

  Returns:      Child state.
 *---------------------------------------------------------------------------*/
u16 MBP_GetChildBmp(MBPBmpType bmpType)
{
    u16     tmpBitmap;
    static const u16 *BITMAP_TABLE[MBP_BMPTYPE_NUM] = {
        &mbpState.connectChildBmp,
        &mbpState.requestChildBmp,
        &mbpState.entryChildBmp,
        &mbpState.downloadChildBmp,
        &mbpState.bootableChildBmp,
        &mbpState.rebootChildBmp,
    };

    SDK_ASSERT(bmpType < MBP_BMPTYPE_NUM);

    tmpBitmap = *(BITMAP_TABLE[bmpType]);

    return tmpBitmap;
}


/*---------------------------------------------------------------------------*
  Name:         MBP_GetChildState

  Description:  Obtains the connection state of a child.

  Arguments:    aid     Number for the connected child.

  Returns:      Child state.
 *---------------------------------------------------------------------------*/
MBPChildState MBP_GetChildState(u16 aid)
{
    MBPState tmpState;
    u16     bitmap = (u16)(1 << aid);

    // Because the connection state may change unexpectedly by an interrupt,
    // prohibit interrupts and obtain the snapshot of the state of that time.
    OSIntrMode enabled = OS_DisableInterrupts();
    if ((mbpState.connectChildBmp & bitmap) == 0)
    {
        (void)OS_RestoreInterrupts(enabled);
        return MBP_CHILDSTATE_NONE;    // Unconnected
    }
    MI_CpuCopy8(&mbpState, &tmpState, sizeof(MBPState));
    (void)OS_RestoreInterrupts(enabled);

    // Determining the connection status
    if (tmpState.requestChildBmp & bitmap)
    {
        return MBP_CHILDSTATE_REQUEST; // Requesting connection
    }
    if (tmpState.entryChildBmp & bitmap)
    {
        return MBP_CHILDSTATE_ENTRY;   // Undergoing entry
    }
    if (tmpState.downloadChildBmp & bitmap)
    {
        return MBP_CHILDSTATE_DOWNLOADING;      // Downloading
    }
    if (tmpState.bootableChildBmp & bitmap)
    {
        return MBP_CHILDSTATE_BOOTABLE; // Boot preparation completed
    }
    if (tmpState.rebootChildBmp & bitmap)
    {
        return MBP_CHILDSTATE_REBOOT;  // Rebooted
    }

    return MBP_CHILDSTATE_CONNECTING;  // Connecting
}


/*---------------------------------------------------------------------------*
  Name:         MBP_AddBitmap

  Description:  Adds a child to the connection state bit map.

  Arguments:    pBitmap     Pointer to the bit map to add.
                aid         aid of the child to be added.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void MBP_AddBitmap(u16 *pBitmap, u16 aid)
{
    // Prohibit interrupts so that the state is not broken when by a sudden interrupt when the values are being changed.
    //
    OSIntrMode enabled = OS_DisableInterrupts();
    *pBitmap |= (1 << aid);
    (void)OS_RestoreInterrupts(enabled);
}


/*---------------------------------------------------------------------------*
  Name:         MBP_RemoveBitmap

  Description:  Delete a child from the connection state bit map.

  Arguments:    pBitmap     Pointer to the bit map to delete.
                aid         aid of the child to be deleted.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void MBP_RemoveBitmap(u16 *pBitmap, u16 aid)
{
    // Prohibit interrupts so the state is not broken by a sudden interrupt while values are changing.
    //
    OSIntrMode enabled = OS_DisableInterrupts();
    *pBitmap &= ~(1 << aid);
    (void)OS_RestoreInterrupts(enabled);
}

/*---------------------------------------------------------------------------*
  Name:         MBP_DisconnectChildFromBmp

  Description:  delete the child device flag from all bitmaps when a child device is disconnected
                on the production prototype of the machine.

  Arguments:    aid     aid of the child that was disconnected.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void MBP_DisconnectChildFromBmp(u16 aid)
{
    u16     aidMask = (u16)~(1 << aid);

    OSIntrMode enabled = OS_DisableInterrupts();

    mbpState.connectChildBmp &= aidMask;
    mbpState.requestChildBmp &= aidMask;
    mbpState.entryChildBmp &= aidMask;
    mbpState.downloadChildBmp &= aidMask;
    mbpState.bootableChildBmp &= aidMask;
    mbpState.rebootChildBmp &= aidMask;

    (void)OS_RestoreInterrupts(enabled);
}

/*---------------------------------------------------------------------------*
  Name:         MBP_DisconnectChild

  Description:  Disconnects a child.

  Arguments:    aid     aid of the child to be disconnected.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void MBP_DisconnectChild(u16 aid)
{
    // MB_Disconnect may be provided in the future.
    MBP_Printf(" WM_Disconnect %d\n", aid);
    MBP_DisconnectChildFromBmp(aid);
    MB_DisconnectChild(aid);
}

/*---------------------------------------------------------------------------*
  Name:         MBP_GetChildInfo

  Description:  Obtains a pointer to the user information on a child.

  Arguments:    child_aid   aid of the child.

  Returns:      Pointer to the user information.
 *---------------------------------------------------------------------------*/
const MBPChildInfo *MBP_GetChildInfo(u16 child_aid)
{
    if (!(mbpState.connectChildBmp & (1 << child_aid)))
    {
        return NULL;
    }
    return &childInfo[child_aid - 1];
}


/*---------------------------------------------------------------------------*
  Name:         MBP_GetPlayerNo

  Description:  This allows you to search and obtain the player number (aid) of the time when it was a multiboot child,
                based on MAC address, after reconnecting a child by rebooting.

  Arguments:    macAddress      MAC address of a child.

  Returns:      child aid at the time of multiboot connection
 *---------------------------------------------------------------------------*/
u16 MBP_GetPlayerNo(const u8 *macAddress)
{
    u16     i;

    for (i = 1; i < MBP_CHILD_MAX + 1; i++)
    {
        if ((mbpState.connectChildBmp & (1 << i)) == 0)
        {
            continue;
        }
        if (WM_IsBssidEqual(macAddress, childInfo[i - 1].macAddress))
        {
            return childInfo[i - 1].playerNo;
        }
    }

    return 0;
}


/*---------------------------------------------------------------------------*
  Name:         MBP_GetChildMacAddress

  Description:  Obtains MAC address of the multiboot child

  Arguments:    aid     aid of the child

  Returns:      Pointer to MAC address
 *---------------------------------------------------------------------------*/
const u8 *MBP_GetChildMacAddress(u16 aid)
{
    if (!(mbpState.connectChildBmp & (1 << aid)))
    {
        return NULL;
    }
    return childInfo[aid - 1].macAddress;
}
