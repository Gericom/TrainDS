#include <nitro.h>
#include "core.h"
#include "TitleSequencePlayer.h"
#include "TitleSequence.h"

u32 gTitleSequence[] =
{
	TITLE_SEQUENCE_COMMAND_TRAIN_CAM(0, 0, 22 * FX32_ONE, 0),
	TITLE_SEQUENCE_COMMAND_END_OF_SEQUENCE()
};