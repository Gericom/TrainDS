#pragma once

typedef u32 sec_t;

#define FEATURE_MEDIUM_CANREAD		0x00000001
#define FEATURE_MEDIUM_CANWRITE		0x00000002
#define FEATURE_SLOT_GBA			0x00000010
#define FEATURE_SLOT_NDS			0x00000020

#define DEVICE_TYPE_DSI_SD ('i') | ('_' << 8) | ('S' << 16) | ('D' << 24)

typedef bool(*FN_MEDIUM_STARTUP)(void);
typedef bool(*FN_MEDIUM_ISINSERTED)(void);
typedef bool(*FN_MEDIUM_READSECTORS)(sec_t sector, sec_t numSectors, void* buffer);
typedef bool(*FN_MEDIUM_WRITESECTORS)(sec_t sector, sec_t numSectors, const void* buffer);
typedef bool(*FN_MEDIUM_CLEARSTATUS)(void);
typedef bool(*FN_MEDIUM_SHUTDOWN)(void);

struct DISC_INTERFACE
{
	unsigned long			ioType;
	unsigned long			features;
	FN_MEDIUM_STARTUP		startup;
	FN_MEDIUM_ISINSERTED	isInserted;
	FN_MEDIUM_READSECTORS	readSectors;
	FN_MEDIUM_WRITESECTORS	writeSectors;
	FN_MEDIUM_CLEARSTATUS	clearStatus;
	FN_MEDIUM_SHUTDOWN		shutdown;
};

#define FIX_ALL						0x01
#define FIX_GLUE					0x02
#define FIX_GOT						0x04
#define FIX_BSS						0x08

#define DLDI_SIZE_32KB	0x0f
#define DLDI_SIZE_16KB	0x0e
#define DLDI_SIZE_8KB	0x0d
#define DLDI_SIZE_4KB	0x0c
#define DLDI_SIZE_1KB	0x0a

#define DLDI_MAGIC_NUMBER 0xBF8DA5ED

#define DLDI_MAGIC_STRING_LEN 		8
#define DLDI_FRIENDLY_NAME_LEN 		48

struct DLDI_INTERFACE
{
	u32 	magicNumber;
	char	magicString[DLDI_MAGIC_STRING_LEN];
	u8		versionNumber;
	u8		driverSize;			// log-2 of driver size in bytes
	u8		fixSectionsFlags;
	u8		allocatedSize;		// log-2 of the allocated space in bytes

	char	friendlyName[DLDI_FRIENDLY_NAME_LEN];

	// Pointers to sections that need address fixing
	void*	dldiStart;
	void*	dldiEnd;
	void*	interworkStart;
	void*	interworkEnd;
	void*	gotStart;
	void*	gotEnd;
	void*	bssStart;
	void*	bssEnd;

	// Original I/O interface data
	DISC_INTERFACE ioInterface;
};

enum DSFlashCard
{
	DS_FLASH_CARD_UNKNOWN,
	DS_FLASH_CARD_R4DS,
	DS_FLASH_CARD_DSTWO,
	DS_FLASH_CARD_MPCF,
	DS_FLASH_CARD_MAX
};

//basic initialisation
DSFlashCard DLDI_IdentifyCard();
bool DLDI_Init(DSFlashCard card);

//device access
bool DLDI_DeviceStartup();
bool DLDI_DeviceIsInserted();
bool DLDI_DeviceReadSectors(sec_t sector, sec_t numSectors, void* buffer);
bool DLDI_DeviceWriteSectors(sec_t sector, sec_t numSectors, const void* buffer);
bool DLDI_DeviceClearStatus();
bool DLDI_DeviceShutdown();