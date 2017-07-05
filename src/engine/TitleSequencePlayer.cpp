#include <nitro.h>
#include "core.h"
#include "engine/cameras/VehicleCamera.h"
#include "terrain/GameController.h"
#include "TitleSequencePlayer.h"

void TitleSequencePlayer::Update()
{	
	if (mUpdateDstToTrainId >= 0)
	{
		//mGameController->mWagon->GetPosition(&mGameController->mCamera->mDestination);
		//mGameController->mCamera->mDestination.x -= 32 * FX32_ONE;
		//mGameController->mCamera->mDestination.z -= 32 * FX32_ONE;
		//mGameController->mCamera->mDestination.y += FX32_CONST(0.2f);
	}

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
			mUpdateDstToTrainId = *mCurSequencePos++;
			VecFx32 rot;
			rot.x = *mCurSequencePos++;
			rot.y = *mCurSequencePos++;
			rot.z = *mCurSequencePos++;
			mGameController->mWagon->GetPosition(&mGameController->mCamera->mDestination);
			//mGameController->mCamera->mDestination.x -= 32 * FX32_ONE;
			//mGameController->mCamera->mDestination.z -= 32 * FX32_ONE;
			//mGameController->mCamera->mDestination.y += FX32_CONST(0.2f);
			if (mGameController->mCamera)
				delete mGameController->mCamera;
			mGameController->mCamera = new VehicleCamera(mGameController->mWagon);

			((VehicleCamera*)mGameController->mCamera)->mCamDistance = FX32_CONST(1.25f);
			((VehicleCamera*)mGameController->mCamera)->SetRotation(&rot);
			break;
		}
		case TITLE_SEQUENCE_COMMAND_ID_FIXED_CAM:
		{
			mUpdateDstToTrainId = -1;
			VecFx32 dst;
			dst.x = *mCurSequencePos++;
			dst.y = *mCurSequencePos++;
			dst.z = *mCurSequencePos++;
			VecFx32 rot;
			rot.x = *mCurSequencePos++;
			rot.y = *mCurSequencePos++;
			rot.z = *mCurSequencePos++;
			if (mGameController->mCamera)
				delete mGameController->mCamera;
			mGameController->mCamera = new FreeRoamCamera();
			mGameController->mCamera->mDestination = dst;
			((FreeRoamCamera*)mGameController->mCamera)->mCamDistance = 2 * FX32_ONE;
			((FreeRoamCamera*)mGameController->mCamera)->SetRotation(&rot);
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