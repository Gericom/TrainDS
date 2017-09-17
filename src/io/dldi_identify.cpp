#include <nitro.h>
#include "core.h"
#include "util.h"
#include "dldi.h"
#include "dldi_identify.h"

/* Card control register */
#define REG_CARD_MASTER_CNT (HW_REG_BASE + 0x1A1)
#define REG_CARDCNT         (HW_REG_BASE + 0x1A4)
#define REG_CARD_CMD        (HW_REG_BASE + 0x1A8)
#define REG_CARD_DATA       (HW_REG_BASE + 0x100010)

/* REG_CARD_MASTER_CNT setting bit */
#define CARDMST_SEL_ROM     0x00
#define CARDMST_IF_ENABLE   0x40
#define CARDMST_ENABLE      0x80

/* Card command format (Hardware does not support anything other than 1 page) */
#define CARD_DATA_READY     0x00800000
#define CARD_COMMAND_PAGE   0x01000000
#define CARD_COMMAND_ID     0x07000000
#define CARD_COMMAND_MASK   0x07000000
#define CARD_RESET_HI       0x20000000
#define CARD_RESET_LO       0x00000000
#define CARD_ACCESS_MODE    0x40000000
#define CARD_READ_MODE      0x00000000
#define CARD_WRITE_MODE     0x40000000
#define CARD_START          0x80000000
#define CARD_LATENCY1_MASK  0x00001FFF

#define MROMOP_G_READ_ID    0xB8000000
#define MROMOP_G_READ_PAGE  0xB7000000

static void DLDIi_SetRomOp(u32 cmd1, u32 cmd2)
{
	/* Wait for CARD ROM completion (just to be safe) */
	while ((*(vu32 *)REG_CARDCNT & CARD_START) != 0);
	/* Master enable */
	*(vu8 *)REG_CARD_MASTER_CNT = CARDMST_SEL_ROM | CARDMST_ENABLE | CARDMST_IF_ENABLE;
	{                                  /* Command settings */
		vu8    *const p_cmd = (vu8 *)REG_CARD_CMD;
		p_cmd[0] = (u8)(cmd1 >> (8 * 3));
		p_cmd[1] = (u8)(cmd1 >> (8 * 2));
		p_cmd[2] = (u8)(cmd1 >> (8 * 1));
		p_cmd[3] = (u8)(cmd1 >> (8 * 0));
		p_cmd[4] = (u8)(cmd2 >> (8 * 3));
		p_cmd[5] = (u8)(cmd2 >> (8 * 2));
		p_cmd[6] = (u8)(cmd2 >> (8 * 1));
		p_cmd[7] = (u8)(cmd2 >> (8 * 0));
	}
}

static void DLDIi_CardTransfer(u32 control, u32 cmd1, u32 cmd2, u32* dst, u32 len)
{
	DLDIi_SetRomOp(cmd1, cmd2);
	{                              /* CPU transfer (repeated loop) */
		u32     pos = 0;
		*(vu32 *)REG_CARDCNT = control;// 0xa7586000;
		for (;;)
		{
			const u32 ctrl = *(vu32 *)REG_CARDCNT;
			if ((ctrl & CARD_DATA_READY) != 0)
			{
				/* Store as much as the specified size */
				u32     data = *(vu32 *)REG_CARD_DATA;
				if (pos * sizeof(u32) < len)
					dst[pos++] = data;
			}
			if (!(ctrl & CARD_START))
				break;
		}
	}
}

static bool DLDIi_IsR4DSInserted()
{
	u32 val;
	DLDIi_CardTransfer(0xa7586000, 0xB0000000, 0, &val, 4);
	return (val & 7) == 4;
}

DSFlashCard DLDI_IdentifyCard()
{
	if (DLDIi_IsR4DSInserted())
		return DS_FLASH_CARD_R4DS;
	return DS_FLASH_CARD_UNKNOWN;
}