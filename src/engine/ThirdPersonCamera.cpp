#include <nitro.h>
#include <nnsys/g3d.h>
#include "core.h"
#include "vehicles/train.h"
#include "Camera.h"
#include "LookAtCamera.h"
#include "ThirdPersonCamera.h"

void ThirdPersonCamera::Apply()
{
	MtxFx43 rot2;
	{
		VecFx32 up = {0, FX32_ONE, 0};
		VecFx32 cam = {0,0,0};
		VecFx32 dir = mTrain->firstPart->direction;
		dir.z = -dir.z;

		MTX_LookAt(&cam, &up, &dir, &rot2);
	}
	if(mPhi > 89 * FX32_ONE) mPhi = 89 * FX32_ONE;
	fx32 sTheta = FX_SinIdx(FX_DEG_TO_IDX(mTheta));
	fx32 cTheta = FX_CosIdx(FX_DEG_TO_IDX(mTheta));
	fx32 sPhi = FX_SinIdx(FX_DEG_TO_IDX(mPhi));
	fx32 cPhi = FX_CosIdx(FX_DEG_TO_IDX(mPhi));
	MtxFx33 mtx;
	MTX_RotY33(&mtx, sTheta, cTheta);
	MtxFx33 mtx2;
	MTX_RotZ33(&mtx2, sPhi, cPhi);
	MTX_Concat33(&mtx2, &mtx, &mtx);
	VecFx32 tmp = {mRadius, 0, 0};
	//MTX_MultVec33(&tmp, &mtx2, &tmp);
	MTX_MultVec33(&tmp, &mtx, &tmp);
	MTX_MultVec43(&tmp, &rot2, &tmp);
	mPosition.x = tmp.x + mTrain->firstPart->position.x - 8 * FX32_ONE;
	mPosition.y = tmp.y + mTrain->firstPart->position.y + (FX32_ONE / 3);
	mPosition.z = tmp.z + mTrain->firstPart->position.z - 8 * FX32_ONE;
	mDestination.x = mTrain->firstPart->position.x - 8 * FX32_ONE + mTrain->firstPart->direction.x;
	mDestination.y = mTrain->firstPart->position.y + (FX32_ONE / 3) + mTrain->firstPart->direction.y;
	mDestination.z = mTrain->firstPart->position.z - 8 * FX32_ONE + mTrain->firstPart->direction.z;
	LookAtCamera::Apply();
}