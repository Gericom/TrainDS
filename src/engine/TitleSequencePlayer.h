#pragma once

#define TITLE_SEQUENCE_COMMAND_ID_WAIT				0x81
#define TITLE_SEQUENCE_COMMAND_ID_TRAIN_CAM			0x82
#define TITLE_SEQUENCE_COMMAND_ID_FIXED_CAM			0x83
#define TITLE_SEQUENCE_COMMAND_ID_FIXED_TRAIN_CAM	0x84
#define TITLE_SEQUENCE_COMMAND_ID_ANIM_CAM			0x85
#define TITLE_SEQUENCE_COMMAND_ID_END_OF_SEQUENCE	0xFF

#define TITLE_SEQUENCE_COMMAND_WAIT(frames) \
	TITLE_SEQUENCE_COMMAND_ID_WAIT, (frames)
#define TITLE_SEQUENCE_COMMAND_TRAIN_CAM(train, xrot, yrot, zrot) \
	TITLE_SEQUENCE_COMMAND_ID_TRAIN_CAM, (train), (xrot), (yrot), (zrot)
#define TITLE_SEQUENCE_COMMAND_FIXED_CAM(x, y, z, xrot, yrot, zrot) \
	TITLE_SEQUENCE_COMMAND_ID_FIXED_CAM, (x), (y), (z), (xrot), (yrot), (zrot)
#define TITLE_SEQUENCE_COMMAND_FIXED_TRAIN_CAM(train, x, y, z, xrot, yrot, zrot) \
	TITLE_SEQUENCE_COMMAND_ID_FIXED_CAM, (train), (x), (y), (z), (xrot), (yrot), (zrot)
#define TITLE_SEQUENCE_COMMAND_ANIM_CAM(x1, y1, z1, xrot1, yrot1, zrot1, x2, y2, z2, xrot2, yrot2, zrot2) \
	TITLE_SEQUENCE_COMMAND_ID_ANIM_CAM, (x1), (y1), (z1), (xrot1), (yrot1), (zrot1), (x2), (y2), (z2), (xrot2), (yrot2), (zrot2)
#define TITLE_SEQUENCE_COMMAND_END_OF_SEQUENCE() \
	TITLE_SEQUENCE_COMMAND_ID_END_OF_SEQUENCE

class GameController;

class TitleSequencePlayer
{
private:
	GameController* mGameController;
	u32* mSequence;
	u32* mCurSequencePos;
	bool mFinished;
	int mWaitCounter;
	int mUpdateDstToTrainId;
	void UpdateSingleFrame();
public:
	TitleSequencePlayer(GameController* gameController, u32* sequence)
		: mGameController(gameController), mSequence(sequence), mCurSequencePos(sequence), mFinished(false), mWaitCounter(0), mUpdateDstToTrainId(-1)
	{ }

	void Update(int nrFrames)
	{
		for (int i = 0; i < nrFrames; i++)
			UpdateSingleFrame();
	}
};