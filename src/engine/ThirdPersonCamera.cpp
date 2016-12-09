#include <nitro.h>
#include <nnsys/g3d.h>
#include "core.h"
#include "vehicles/train.h"
#include "Camera.h"
#include "LookAtCamera.h"
#include "ThirdPersonCamera.h"
#include "Quaternion.h"

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
	//if(mPhi > 89 * FX32_ONE) mPhi = 89 * FX32_ONE;

	/*mTheta = mTheta % (360 * FX32_ONE);
	mPhi = mPhi % (360 * FX32_ONE);

	fx32 camx = FX_Mul(FX_Mul(FX_CosIdx(FX_DEG_TO_IDX(mTheta)), FX_CosIdx(FX_DEG_TO_IDX(mPhi))), mRadius) + mTrain->firstPart->position.x - 8 * FX32_ONE;
	fx32 camy = FX_Mul(FX_SinIdx(FX_DEG_TO_IDX(mPhi)), mRadius) + mTrain->firstPart->position.y + (FX32_ONE / 3);
	fx32 camz = FX_Mul(FX_Mul(FX_SinIdx(FX_DEG_TO_IDX(mTheta)), FX_CosIdx(FX_DEG_TO_IDX(mPhi))), mRadius) + mTrain->firstPart->position.z - 8 * FX32_ONE;
	//fx32 xy = FX_Mul(FX_CosIdx(FX_DEG_TO_IDX(mTheta)), mRadius);
	//fx32 zy = FX_Mul(FX_SinIdx(FX_DEG_TO_IDX(mTheta)), mRadius);
	//fx32 zy2 = FX_Mul(FX_CosIdx(FX_DEG_TO_IDX(mPhi)), mRadius);
	//fx32 yy = FX_Mul(FX_SinIdx(FX_DEG_TO_IDX(mPhi)), mRadius);*/
	VecFx32 tmp = {mRadius, 0, 0};
	//VecFx32 tmp2 = {0, 0, FX32_ONE};
	Quaternion q(0, FX32_ONE, 0, mTheta);
	//Quaternion q2(FX32_ONE, 0, 0, mPhi);
	//q.MultiplyByQuaternion(&q2, &q);
	q.MultiplyByVector(&tmp, &tmp);
	//q.MultiplyByVector(&tmp2, &tmp2);
	//Quaternion q2(&tmp2, mPhi);
	//q2.MultiplyByVector(&tmp, &tmp);
	//q.MultiplyByVector(&tmp, &tmp);
	MTX_MultVec43(&tmp, &rot2, &tmp);//*/
	//VecFx32 tmp = {0, 0, mRadius};
	//fx16 sinx = FX_SinIdx(FX_DEG_TO_IDX(mTheta));
	//fx16 cosx = FX_CosIdx(FX_DEG_TO_IDX(mTheta));
	//fx16 siny = FX_SinIdx(FX_DEG_TO_IDX(mPhi));
	//fx16 cosy = FX_CosIdx(FX_DEG_TO_IDX(mPhi));
	//VecFx32 tmp;
	//tmp.x = FX_Mul(FX_Mul(siny, cosx), mRadius);
	//tmp.y = FX_Mul(cosy, mRadius);
	//tmp.z = FX_Mul(FX_Mul(siny, sinx), mRadius);
	//mRotation.MultiplyByVector(&tmp, &tmp);
	//MTX_MultVec43(&tmp, &rot2, &tmp);
	tmp.y += mPhi;
	mPosition.x = tmp.x + mTrain->firstPart->position.x - 32 * FX32_ONE;
	mPosition.y = tmp.y + mTrain->firstPart->position.y + (FX32_ONE / 3);
	mPosition.z = tmp.z + mTrain->firstPart->position.z - 32 * FX32_ONE;
	mDestination.x = mTrain->firstPart->position.x - 32 * FX32_ONE;// +mTrain->firstPart->direction.x;
	mDestination.y = mTrain->firstPart->position.y + (FX32_ONE / 3);// +mTrain->firstPart->direction.y;
	mDestination.z = mTrain->firstPart->position.z - 32 * FX32_ONE;// +mTrain->firstPart->direction.z;
	//mUp.x = 0;
	//mUp.y = (FX_CosIdx(FX_DEG_TO_IDX(mPhi)) < 0) ? -FX32_ONE : FX32_ONE;
	//mUp.z = 0;
	/*fx16 sinx = FX_SinIdx(FX_DEG_TO_IDX(mPhi));
	fx16 cosx = FX_CosIdx(FX_DEG_TO_IDX(mPhi));
	fx16 siny = FX_SinIdx(FX_DEG_TO_IDX(mTheta));
	fx16 cosy = FX_CosIdx(FX_DEG_TO_IDX(mTheta));
	fx16 sinz = FX_SinIdx(0);//(u16)camera->angleZ);
	fx16 cosz = FX_CosIdx(0);//(u16)camera->angleZ);

	mPosition.x = cosx * mRadius >> FX32_SHIFT;
	mPosition.y = sinx * mRadius >> FX32_SHIFT;
	mPosition.z = cosx * mRadius >> FX32_SHIFT;

	mPosition.x = mDestination.x + mPosition.x * siny >> FX32_SHIFT;
	mPosition.y = mDestination.y + mPosition.y;
	mPosition.z = mDestination.z + mPosition.z * cosy >> FX32_SHIFT;

    mUp.x =  FX_Mul( sinz, cosy );
    mUp.y =  cosz;
    mUp.z = -FX_Mul( sinz, siny );*/
	LookAtCamera::Apply();
}