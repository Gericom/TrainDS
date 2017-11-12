#include "common.h"




extern "C"
{
	#include "mb/wh.h"
	#include "mb/mbp.h"
	#include "mb/wfs.h"
}
#include "util.h"
#include "Menu.h"
#include "Game.h"
#include "MultibootMenu.h"

static const MBGameRegistry sMbGameList =
{
	NULL,
	(u16 *)L"Rail Simulator DS",           // Game name
	(u16 *)L"TODO",      // Game content description
	"/data/mb/icon.char",                 // Icon character data
	"/data/mb/icon.plt",                  // Icon palette data
	WH_GGID,                           // GGID
	2,                 // Max. number of players, including parents
};

static u16 sChannel = 0;

static void WaitWHState(int state)
{
	while (WH_GetSystemState() != state)
		OS_WaitVBlankIntr();
}

static u16 MeasureChannel()
{
	//SDK_ASSERT(WH_GetSystemState() == WH_SYSSTATE_IDLE);
	/* channel measurement begins */
	(void)WH_StartMeasureChannel();
	WaitWHState(WH_SYSSTATE_MEASURECHANNEL);
	/* All channels measurement complete */
	return WH_GetMeasureChannel();
}

static bool ConnectMain()
{
	for (;;)
	{
		/* frame standby */
		OS_WaitVBlankIntr();

		//PrintString(4, 22, PLTT_YELLOW, " MB Parent ");

		switch (MBP_GetState())
		{
			//-----------------------------------------
			// Accepting entry from the child
		case MBP_STATE_ENTRY:
		{
			//PrintString(4, 22, PLTT_WHITE, " Now Accepting            ");

			//if (IS_PAD_TRIGGER(PAD_BUTTON_B))
			//{
				// Cancel multiboot with B Button
			//	MBP_Cancel();
			//	break;
			//}

			// If there is at least one child in entry, start is possible.
			if (MBP_GetChildBmp(MBP_BMPTYPE_ENTRY) ||
				MBP_GetChildBmp(MBP_BMPTYPE_DOWNLOADING) ||
				MBP_GetChildBmp(MBP_BMPTYPE_BOOTABLE))
			{
				//PrintString(4, 22, PLTT_WHITE, " Push START Button to start   ");

				//if (IS_PAD_TRIGGER(PAD_BUTTON_START))
				{
					// Start download
					MBP_StartDownloadAll();
				}
			}
		}
		break;

		//-----------------------------------------
		// Program distribution process
		case MBP_STATE_DATASENDING:
		{

			// If everyone has completed download, start is possible.
			if (MBP_IsBootableAll())
			{
				// Start boot
				MBP_StartRebootAll();
			}
		}
		break;

		//-----------------------------------------
		// Reboot process
		case MBP_STATE_REBOOTING:
		{
			//PrintString(4, 22, PLTT_WHITE, " Rebooting now                ");
		}
		break;

		//-----------------------------------------
		// Reconnect process
		case MBP_STATE_COMPLETE:
		{
			// Once all members have successfully connected, the multi-boot processing ends,
			// and restarts the wireless as an normal parent.
			//PrintString(4, 22, PLTT_WHITE, " Reconnecting now             ");

			OS_WaitVBlankIntr();
			return TRUE;
		}
		break;

		//-----------------------------------------
		// Error occurred
		case MBP_STATE_ERROR:
		{
			// Cancel communications.
			MBP_Cancel();
		}
		break;

		//-----------------------------------------
		// Communication cancellation processsing
		case MBP_STATE_CANCEL:
			// None
			break;

			//-----------------------------------------
			// Communications abnormal end
		case MBP_STATE_STOP:
			OS_WaitVBlankIntr();
			return FALSE;
		}
	}
}

static void* AllocatorForWFS(void *arg, u32 size, void *ptr)
{
	if (!ptr)
		return mb_alloc(size);
	else
	{
		NNS_FndFreeToExpHeap(gHeapHandle, ptr);
		return NULL;
	}
}

static BOOL JudgeConnectableChild(WMStartParentCallback *cb)
{
	/*  Search the MAC address for the aid of when multibooting the child of cb->aid */
	u16     playerNo = MBP_GetPlayerNo(cb->macAddress);
	//OS_TPrintf("MB child(%d) -> DS child(%d)\n", playerNo, cb->aid);
	return (playerNo != 0);
}

void MultibootMenu::Initialize(int arg)
{
	WH_SetGgid(WH_GGID);

	for (;;)
	{
		/* wireless initialization */
		(void)WH_Initialize();
		WaitWHState(WH_SYSSTATE_IDLE);
		/* Search for a channel with little traffic */
		sChannel = MeasureChannel();

		/* DS Download Play begins */
#if !defined(MBP_USING_MB_EX)
		(void)WH_End();
		WaitWHState(WH_SYSSTATE_STOP);
#endif
		MBP_Init(WH_GGID, WM_GetNextTgid());
		MBP_Start(&sMbGameList, sChannel);

		if (ConnectMain())
		{
			/* Multiboot child startup is successful */
			break;
		}
		else
		{
			/* Exit the WH module and repeat */
			WH_Finalize();
			(void)WH_End();
		}
	}

#if !defined(MBP_USING_MB_EX)
	(void)WH_Initialize();
#endif
	WaitWHState(WH_SYSSTATE_IDLE);
	/* begins WFS initialization and connection processes */
	WFS_InitParent(PORT_WFS, NULL, AllocatorForWFS,
		NULL, WH_PARENT_MAX_SIZE, NULL, true);
	WH_SetJudgeAcceptFunc(JudgeConnectableChild);
	(void)WH_ParentConnect(WH_CONNECTMODE_DS_PARENT, WM_GetNextTgid(), sChannel);

	/* standby until data sharing preparations are complete, and launch WFS */
	WaitWHState(WH_SYSSTATE_DATASHARING);
	WFS_Start();

	/* frame processing for as long as there are communications */
	/*while (WH_GetSystemState() == WH_SYSSTATE_DATASHARING)
	{
		OS_WaitVBlankIntr();
	}

	/* as MP communications are ended, end WFS /
	WFS_End();
	WaitWHState(WH_SYSSTATE_IDLE);*/
	Game::GotoMenu();
}

void MultibootMenu::Render()
{

}

void MultibootMenu::VBlank()
{

}

void MultibootMenu::Finalize()
{

}