#include <nitro.h>
#include "core.h"
#include "terrain/GameController.h"
#include "TitleSequencePlayer.h"

void TitleSequencePlayer::Update()
{
	if (mFinished)
		return;
	if (mWaitCounter)
	{
		mWaitCounter--;
		return;
	}
	//process commands until end of sequence or wait command
	while (!mWaitCounter && !mFinished)
	{
		u32 command = *mCurSequencePos++;
		switch (command)
		{
		case TITLE_SEQUENCE_COMMAND_ID_WAIT:
			mWaitCounter = *mCurSequencePos++;
			break;
		case TITLE_SEQUENCE_COMMAND_ID_TRAIN_CAM:
		{
			u32 train = *mCurSequencePos++;
			VecFx32 rot;
			rot.x = *mCurSequencePos++;
			rot.y = *mCurSequencePos++;
			rot.z = *mCurSequencePos++;
			break;
		}
		case TITLE_SEQUENCE_COMMAND_ID_END_OF_SEQUENCE:
			mFinished = true;
			break;
		default:
			OS_Panic("Invalid Title Sequence Command! (%x)", command);
			break;
		}
	}
}