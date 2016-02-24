#ifndef __TRAIN_H__
#define __TRAIN_H__
#include <nnsys/g3d.h>

class PathWorker;

struct train_part_t
{
	train_part_t* next;
	NNSG3dRenderObj renderObj;
	PathWorker* pathWorker1;
	PathWorker* pathWorker2;
	VecFx32 position;
	VecFx32 direction;
};

typedef struct
{
	train_part_t* firstPart;
	BOOL isDriving;
	NNSSndHandle trackSoundHandle;
} train_t;

void Train_UpdatePos(train_t* train);
void Train_UpdateSound(train_t* train, VecFx32* CamPos, VecFx32* CamDst, VecFx32* CamUp);

#endif