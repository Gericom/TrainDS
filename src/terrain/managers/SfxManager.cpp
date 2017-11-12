#include "common.h"


#include "Sfx.h"
#include "SfxManager.h"

void SfxManager::Update(LookAtCamera* camera)
{
	Sfx* sfx = (Sfx*)NNS_FndGetNextListObject(&mSfxList, NULL);
	while(sfx)
	{
		sfx->Update(camera);
		Sfx* next = (Sfx*)NNS_FndGetNextListObject(&mSfxList, sfx);
		if (sfx->HasStarted() && !sfx->IsPlaying())
		{
			NNS_FndRemoveListObject(&mSfxList, sfx);
			delete sfx;
		}
		sfx = next;
	}
}