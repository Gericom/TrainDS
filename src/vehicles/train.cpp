#include <nitro.h>
#include <nnsys/g3d.h>
#include "core.h"
#include "engine/PathWorker.h"
#include "engine/Camera.h"
#include "engine/LookAtCamera.h"
#include "train.h"

void Train_UpdatePos(train_t* train)
{
	//Just assume one train part for now
	VecFx32 tpos1;
	VecFx32 dir1;
	VecFx32 tpos2;
	VecFx32 dir2;
	if (train->isDriving && !train->isDrivingBackwards)
	{
		train->firstPart->pathWorker1->Proceed(FX32_ONE / 60/*24*/, &tpos1, &dir1);
		train->firstPart->pathWorker2->Proceed(FX32_ONE / 60, &tpos2, &dir2);
	}
	else if (train->isDriving && train->isDrivingBackwards)
	{
		train->firstPart->pathWorker1->Proceed(-FX32_ONE / 60, &tpos1, &dir1);
		train->firstPart->pathWorker2->Proceed(-FX32_ONE / 60, &tpos2, &dir2);
	}
	else
	{
		train->firstPart->pathWorker1->Proceed(0, &tpos1, &dir1);
		train->firstPart->pathWorker2->Proceed(0, &tpos2, &dir2);
	}
	VecFx32 tpos;
	VecFx32 dir;
	VEC_Add(&tpos1, &tpos2, &tpos);
	VEC_Add(&dir1, &dir2, &dir);
	dir.y = 0;
	VEC_Normalize(&dir, &dir);
	train->firstPart->position.x = (tpos.x + 1) >> 1;
	train->firstPart->position.y = (tpos.y + 1) >> 1;
	train->firstPart->position.z = (tpos.z + 1) >> 1;
	train->firstPart->direction = dir;
	//train->firstPart->direction.x = (dir.x + 1) >> 1;
	//train->firstPart->direction.y = (dir.y + 1) >> 1;
	//train->firstPart->direction.z = (dir.z + 1) >> 1;



	//ydirection
}

void Train_UpdateSound(train_t* train, LookAtCamera* camera)//, VecFx32* CamPos, VecFx32* CamDst, VecFx32* CamUp)
{
	if(train->isDriving)
	{
		VecFx32 traintocam;
		traintocam.x = (train->firstPart->position.x - 32 * FX32_ONE) - camera->mPosition.x;
		traintocam.y = train->firstPart->position.y - camera->mPosition.y;
		traintocam.z = (train->firstPart->position.z - 32 * FX32_ONE) - camera->mPosition.z;
		fx32 dist = VEC_Mag(&traintocam);
		//traintocam.y = 0;
		VEC_Normalize(&traintocam, &traintocam);

		VecFx32 lookdir;
		VEC_Subtract(&camera->mDestination, &camera->mPosition, &lookdir);
		VEC_Normalize(&lookdir, &lookdir);

		VecFx32 side;
		VEC_CrossProduct(&lookdir, &camera->mUp, &side);
		VEC_Normalize(&side, &side);

		//VecFx32 panvec = traintocam;
		//panvec.y = 0;
		//VEC_Normalize(&panvec, &panvec);
		//VecFx32 m_right = {FX32_ONE, 0, 0};
		fx32 dot = VEC_DotProduct(&side, &traintocam);
		s32 pan = (dot * 128) >> FX32_SHIFT;
		if(pan < -128) { pan = -128; }
		if(pan > 127) { pan = 127; }

		fx32 revdist = (FX_Mul((16 * FX32_ONE) - dist, 256) * 127) >> FX32_SHIFT;
		if(revdist < 0) revdist = 0;
		if(revdist > 127) revdist = 127;


		NNS_SndPlayerSetTrackPan(&train->trackSoundHandle, 0xFFFF, pan);
		NNS_SndPlayerSetVolume(&train->trackSoundHandle, revdist);
	}
	else NNS_SndPlayerSetVolume(&train->trackSoundHandle, 0);
}