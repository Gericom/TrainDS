#ifndef __SFX_MANAGER_H__
#define __SFX_MANAGER_H__

#include <nnsys/fnd.h>
#include "Sfx.h"

class SfxManager
{
private:
	NNSFndList mSfxList;

public:

	SfxManager()
	{
		NNS_FND_INIT_LIST(&mSfxList, Sfx, mLink);
	}

	//I'm not sure if this should be changed to be an effect id and a lookup in some table
	//then this function should return an instance id
	void StartSfx(Sfx* pSfx)
	{
		NNS_FndAppendListObject(&mSfxList, pSfx);
		pSfx->Start();
	}

	void StopSfx(Sfx* pSfx)
	{
		pSfx->Stop();
	}

	void Update();

};

#endif