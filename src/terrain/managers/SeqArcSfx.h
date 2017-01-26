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

public:
	SeqArcSfx(int seqArcNo, int index)
		: mSeqArcNo(seqArcNo), mIndex(index)
	{ 
		NNS_SndHandleInit(&mSndHandle);
	}

	void Start()
	{
		if (IsPlaying())
			return;
		NNS_SndArcPlayerStartSeqArc(&mSndHandle, mSeqArcNo, mIndex);
		mHasStarted = true;
	}

	void Stop(int fade = 0)
	{
		NNS_SndPlayerStopSeq(&mSndHandle, fade);
	}

	bool IsPlaying()
	{
		if (!mHasStarted)
			return false;
		SNDPlayerInfo info;
		NNS_SndPlayerReadDriverPlayerInfo(&mSndHandle, &info);
		return info.activeFlag;
	}
};

#endif