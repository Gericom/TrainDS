#include <nitro.h>
#include "core.h"
#include "util.h"
extern "C"
{
	#include "fat/fat_filelib.h"
}
#include "dldi.h"
#include "dldi_archive.h"

static FSArchive sDldiArchive;

int media_read(unsigned long sector, unsigned char *buffer, unsigned long sector_count)
{
	return DLDI_DeviceReadSectors(sector, sector_count, buffer);
}

int media_write(unsigned long sector, unsigned char *buffer, unsigned long sector_count)
{
	return DLDI_DeviceWriteSectors(sector, sector_count, buffer);
}

static FSResult DLDIi_ArchiveProc(FSFile *p_file, FSCommandType cmd)
{
	NOCASH_Printf("cmd: %d\n", cmd);
	switch (cmd)
	{
	case FS_COMMAND_SEEKDIR:
		return FS_RESULT_UNSUPPORTED;

	case FS_COMMAND_READDIR:
		return FS_RESULT_UNSUPPORTED;

	case FS_COMMAND_OPENFILEFAST:
	{
		char* path = (char*)p_file->arg.openfilefast.id.file_id;
		if(!path)
			return FS_RESULT_FAILURE;

		void* file = fl_fopen(path, "rb");

		NNS_FndFreeToExpHeap(gHeapHandle, path);

		if(!file)
			return FS_RESULT_FAILURE;

		fl_fseek(file, 0, SEEK_END);

		u32 len = fl_ftell(file);

		fl_fseek(file, 0, SEEK_SET);

		p_file->prop.file.own_id = (u32)file;
		p_file->prop.file.top = 0;
		p_file->prop.file.pos = 0;
		p_file->prop.file.bottom = len;

		return FS_RESULT_SUCCESS;
	}

	case FS_COMMAND_OPENFILEDIRECT:
		return FS_RESULT_UNSUPPORTED;

	case FS_COMMAND_FINDPATH:
	{
		if (p_file->arg.findpath.find_directory)
			return FS_RESULT_UNSUPPORTED;
		char* path = (char*)NNS_FndAllocFromExpHeapEx(gHeapHandle, STD_StrLen(p_file->arg.findpath.path) + 2, -4);
		path[0] = '/';
		STD_StrCpy(path + 1, p_file->arg.findpath.path);
		p_file->arg.findpath.result.file->arc = &sDldiArchive;
		p_file->arg.findpath.result.file->file_id = (u32)path;
		return FS_RESULT_SUCCESS;
	}

	case FS_COMMAND_CLOSEFILE:
	{
		if (!p_file->prop.file.own_id)
			return FS_RESULT_FAILURE;
		fl_fclose((void*)p_file->prop.file.own_id);
		return FS_RESULT_SUCCESS;
	}

	case FS_COMMAND_READFILE:
	{
		void* file = (void*)p_file->prop.file.own_id;
		u32 curpos = fl_ftell(file);
		if (curpos != p_file->prop.file.pos)
			fl_fseek(file, p_file->prop.file.pos, SEEK_SET);
		fl_fread(p_file->arg.readfile.dst, 1, p_file->arg.readfile.len, file);
		return FS_RESULT_SUCCESS;
	}

	case FS_COMMAND_WRITEFILE:
		return FS_RESULT_UNSUPPORTED;

	/*
	* upper-layer commands or optional message commands.
	*
	* as returning "unknown",
	* some are substituted by standard routine
	* which uses basic commands (we implemented!),
	* others are simply ignored.
	*/
	default:
	case FS_COMMAND_GETPATH:
	case FS_COMMAND_ACTIVATE:
	case FS_COMMAND_IDLE:
		return FS_RESULT_PROC_UNKNOWN;

	}
}

static FSResult CustomRom_ReadCallback(FSArchive *p_arc, void *dst, u32 pos, u32 size)
{
	//(void)p_arc;
	//MI_CpuCopy8((const void *)pos, dst, size);
	return FS_RESULT_SUCCESS;
}
static FSResult CustomRom_WriteCallback(FSArchive *p_arc, const void *src, u32 pos, u32 size)
{
	//(void)p_arc;
	//MI_CpuCopy8(src, (void *)pos, size);
	return FS_RESULT_SUCCESS;
}

bool DLDI_Mount()
{
	fl_init();
	if (fl_attach_media(media_read, media_write) != FAT_INIT_OK)
	{
		OS_Panic("ERROR: Media attach failed\n");
		return false;
	}
	FS_InitArchive(&sDldiArchive);
	if (!FS_RegisterArchiveName(&sDldiArchive, "sdc", 3))
	{
		OS_Panic("error! FS_RegisterArchiveName(sdc) failed.\n");
		return false;
	}
	FS_SetArchiveProc(&sDldiArchive, DLDIi_ArchiveProc, FS_ARCHIVE_PROC_ALL);
	if (!FS_LoadArchive(&sDldiArchive, 0, NULL, 0, NULL, 0, CustomRom_ReadCallback, CustomRom_WriteCallback))
	{
		OS_Panic("error! FS_LoadArchive() failed.\n");
		return false;
	}
	return true;
}