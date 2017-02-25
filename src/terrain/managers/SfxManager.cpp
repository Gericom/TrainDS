#include <nitro.h>
#include <nnsys/fnd.h>
#include "core.h"
#include "Sfx.h"
#include "SfxManager.h"

void SfxManager::Update()
{
	Sfx* sfx = (Sfx*)NNS_FndGetNextListObject(&mSfxList, NULL);
	while(sfx)
	{
		Sfx* next = (Sfx*)NNS_FndGetNextListObject(&mSfxList, sfx);
		if (sfx->HasStarted() && !sfx->IsPlaying())
		{
			NNS_FndRemoveListObject(&mSfxList, sfx);
			delete sfx;
		}
		sfx = next;
	}
}