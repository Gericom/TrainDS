#include <nitro.h>
#include "core.h"
#include "TitleSequencePlayer.h"
#include "TitleSequence.h"

u32 gTitleSequence[] =
{
	TITLE_SEQUENCE_COMMAND_FIXED_CAM(55122, 128, 40054, -167936, -4096, 0),
	TITLE_SEQUENCE_COMMAND_WAIT(4 * 60 + 36),
	TITLE_SEQUENCE_COMMAND_TRAIN_CAM(0, 0, 12 * FX32_ONE, 0),
	TITLE_SEQUENCE_COMMAND_END_OF_SEQUENCE()
};