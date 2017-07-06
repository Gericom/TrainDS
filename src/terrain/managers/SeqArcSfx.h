#ifndef __SEQ_ARC_SFX_H__
#define __SEQ_ARC_SFX_H__

#include <nnsys/snd.h>

#include "Sfx.h"

class SeqArcSfx : public Sfx
{
private:
	NNSSndHandle mSndHandle;
	int mSeqArcNo;
	int mIndex;

	bool mStarting;

public:
	SeqArcSfx(int seqArcNo, int index)
		: mSeqArcNo(seqArcNo), mIndex(index), mStarting(false)
	{ 
		NNS_SndHandleInit(&mSndHandle);
	}

	void Start()
	{
		if (IsPlaying())
			return;
		NNS_SndArcPlayerStartSeqArc(&mSndHandle, mSeqArcNo, mIndex);
		mHasStarted = true;
		mStarting = true;
	}

	void Stop(int fade = 0)
	{
		if (!IsPlaying())
			return;
		if (fade == 0)
			fade = 1;
		NNS_SndPlayerStopSeq(&mSndHandle, fade);
	}

	void SetPan(u8 pan)
	{
		NNS_SndPlayerSetTrackPan(&mSndHandle, 0xFFFF, pan);
	}

	void SetVolume(u8 volume)
	{
		NNS_SndPlayerSetVolume(&mSndHandle, volume);
	}

	bool IsPlaying()
	{
		if (!mHasStarted)
			return false;
		SNDPlayerInfo info;
		NNS_SndPlayerReadDriverPlayerInfo(&mSndHandle, &info);
		if (mStarting)
			if (info.activeFlag)
				mStarting = false;
			else
				return true;
		return info.activeFlag;
	}
};

#endif