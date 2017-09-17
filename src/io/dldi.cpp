#include <nitro.h>
#include "core.h"
#include "util.h"
#include "dldi.h"

// Stored backwards to prevent it being picked up by DLDI patchers
static const char DLDI_MAGIC_STRING_BACKWARDS[DLDI_MAGIC_STRING_LEN] = { '\0', 'm', 'h', 's', 'i', 'h', 'C', ' ' };

static const char* sDldiFileNames[DS_FLASH_CARD_MAX] =
{
	NULL,
	"r4_sd",
	"dstwo",
	"mpcf"
};

static DLDI_INTERFACE* sDldiInterface = NULL;
static u16 sDldiLockId;



static bool dldiIsValid(const DLDI_INTERFACE* io) 
{
	if (io->magicNumber != DLDI_MAGIC_NUMBER)
		return false;

	for (int i = 0; i < DLDI_MAGIC_STRING_LEN; i++)
		if (io->magicString[i] != DLDI_MAGIC_STRING_BACKWARDS[DLDI_MAGIC_STRING_LEN - 1 - i])
			return false;

	return true;
}

static void dldiFixDriverAddresses(DLDI_INTERFACE* io) 
{
	u8** address;

	u32 offset = (char*)io - (char*)(io->dldiStart);

	u8* oldStart = (u8*)io->dldiStart;
	u8* oldEnd = (u8*)io->dldiEnd;

	// Correct all pointers to the offsets from the location of this interface
	io->dldiStart = (char*)io->dldiStart + offset;
	io->dldiEnd = (char*)io->dldiEnd + offset;
	io->interworkStart = (char*)io->interworkStart + offset;
	io->interworkEnd = (char*)io->interworkEnd + offset;
	io->gotStart = (char*)io->gotStart + offset;
	io->gotEnd = (char*)io->gotEnd + offset;
	io->bssStart = (char*)io->bssStart + offset;
	io->bssEnd = (char*)io->bssEnd + offset;

	io->ioInterface.startup = (FN_MEDIUM_STARTUP)((u8*)io->ioInterface.startup + offset);
	io->ioInterface.isInserted = (FN_MEDIUM_ISINSERTED)((u8*)io->ioInterface.isInserted + offset);
	io->ioInterface.readSectors = (FN_MEDIUM_READSECTORS)((u8*)io->ioInterface.readSectors + offset);
	io->ioInterface.writeSectors = (FN_MEDIUM_WRITESECTORS)((u8*)io->ioInterface.writeSectors + offset);
	io->ioInterface.clearStatus = (FN_MEDIUM_CLEARSTATUS)((u8*)io->ioInterface.clearStatus + offset);
	io->ioInterface.shutdown = (FN_MEDIUM_SHUTDOWN)((u8*)io->ioInterface.shutdown + offset);

	// Fix all addresses with in the DLDI
	if (io->fixSectionsFlags & FIX_ALL) {
		for (address = (u8**)io->dldiStart; address < (u8**)io->dldiEnd; address++) {
			if (oldStart <= *address && *address < oldEnd) {
				*address += offset;
			}
		}
	}

	// Fix the interworking glue section
	if (io->fixSectionsFlags & FIX_GLUE) {
		for (address = (u8**)io->interworkStart; address < (u8**)io->interworkEnd; address++) {
			if (oldStart <= *address && *address < oldEnd) {
				*address += offset;
			}
		}
	}

	// Fix the global offset table section
	if (io->fixSectionsFlags & FIX_GOT) {
		for (address = (u8**)io->gotStart; address < (u8**)io->gotEnd; address++) {
			if (oldStart <= *address && *address < oldEnd) {
				*address += offset;
			}
		}
	}

	// Initialise the BSS to 0
	if (io->fixSectionsFlags & FIX_BSS)
		MI_CpuClear8(io->bssStart, (u8*)io->bssEnd - (u8*)io->bssStart);
}

bool DLDI_Init(DSFlashCard card)
{
	if (sDldiInterface)
		return true;//already initialized

	if (card <= DS_FLASH_CARD_UNKNOWN || card >= DS_FLASH_CARD_MAX)
		return false;

	char dldiPath[35];
	OS_SPrintf(dldiPath, "/data/dldi/%s.dldi", sDldiFileNames[card]);
	FSFile dldiFile;
	FS_InitFile(&dldiFile);
	FS_OpenFile(&dldiFile, dldiPath);
	DLDI_INTERFACE header;
	FS_ReadFile(&dldiFile, &header, sizeof(DLDI_INTERFACE));
	if (!dldiIsValid(&header))
	{
		OS_Printf("DLDI Invalid!\n");
		FS_CloseFile(&dldiFile);
		return false;
	}
	u32 dldiSize;
	if (header.dldiEnd > header.bssEnd)
		dldiSize = (char*)header.dldiEnd - (char*)header.dldiStart;
	else
		dldiSize = (char*)header.bssEnd - (char*)header.dldiStart;
	dldiSize = (dldiSize + 0x03) & ~0x03;
	sDldiInterface = (DLDI_INTERFACE*)NNS_FndAllocFromExpHeapEx(gHeapHandle, dldiSize, 32);
	FS_SeekFileToBegin(&dldiFile);
	FS_ReadFile(&dldiFile, sDldiInterface, dldiSize);
	FS_CloseFile(&dldiFile);
	dldiFixDriverAddresses(sDldiInterface);
	if (header.ioInterface.features & FEATURE_SLOT_GBA)
	{
		OS_SetDPermissionsForProtectionRegion(OS_PR3_ACCESS_MASK, OS_PR3_ACCESS_RW);
		MIi_SetCartridgeProcessor(MI_PROCESSOR_ARM9);
	}
	else if(header.ioInterface.features & FEATURE_SLOT_NDS)
		sDldiLockId = OS_GetLockID();
	return true;
}

bool DLDI_DeviceStartup()
{
	if (!sDldiInterface)
		return false;
	if (sDldiInterface->ioInterface.features & FEATURE_SLOT_NDS)
		CARD_LockRom(sDldiLockId);
	bool result = sDldiInterface->ioInterface.startup();
	if (sDldiInterface->ioInterface.features & FEATURE_SLOT_NDS)
		CARD_UnlockRom(sDldiLockId);
	return result;
}

bool DLDI_DeviceIsInserted()
{
	if (!sDldiInterface)
		return false;
	if (sDldiInterface->ioInterface.features & FEATURE_SLOT_NDS)
		CARD_LockRom(sDldiLockId);
	bool result = sDldiInterface->ioInterface.isInserted();
	if (sDldiInterface->ioInterface.features & FEATURE_SLOT_NDS)
		CARD_UnlockRom(sDldiLockId);
	return result;
}

bool DLDI_DeviceReadSectors(sec_t sector, sec_t numSectors, void* buffer)
{
	if (!sDldiInterface)
		return false;
	if (sDldiInterface->ioInterface.features & FEATURE_SLOT_NDS)
		CARD_LockRom(sDldiLockId);
	bool result = sDldiInterface->ioInterface.readSectors(sector, numSectors, buffer);
	if (sDldiInterface->ioInterface.features & FEATURE_SLOT_NDS)
		CARD_UnlockRom(sDldiLockId);
	return result;
}

bool DLDI_DeviceWriteSectors(sec_t sector, sec_t numSectors, const void* buffer)
{
	if (!sDldiInterface)
		return false;
	if (sDldiInterface->ioInterface.features & FEATURE_SLOT_NDS)
		CARD_LockRom(sDldiLockId);
	bool result = sDldiInterface->ioInterface.writeSectors(sector, numSectors, buffer);
	if (sDldiInterface->ioInterface.features & FEATURE_SLOT_NDS)
		CARD_UnlockRom(sDldiLockId);
	return result;
}

bool DLDI_DeviceClearStatus()
{
	if (!sDldiInterface)
		return false;
	if (sDldiInterface->ioInterface.features & FEATURE_SLOT_NDS)
		CARD_LockRom(sDldiLockId);
	bool result = sDldiInterface->ioInterface.clearStatus();
	if (sDldiInterface->ioInterface.features & FEATURE_SLOT_NDS)
		CARD_UnlockRom(sDldiLockId);
	return result;
}

bool DLDI_DeviceShutdown()
{
	if (!sDldiInterface)
		return false;
	if (sDldiInterface->ioInterface.features & FEATURE_SLOT_NDS)
		CARD_LockRom(sDldiLockId);
	bool result = sDldiInterface->ioInterface.shutdown();
	if (sDldiInterface->ioInterface.features & FEATURE_SLOT_NDS)
		CARD_UnlockRom(sDldiLockId);
	return result;
}