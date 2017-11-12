#include "common.h"


#include "../Quaternion.h"
#include "Camera.h"
#include "vehicles\Vehicle.h"
#include "LookAtCamera.h"
#include "VehicleCamera.h"

void VehicleCamera::Apply()
{
	/*Quaternion a(0, FX32_ONE, 0, mRotation.x);
	a.MultiplyByQuaternion(&Quaternion(FX32_ONE, 0, 0, mRotation.y), &a);
	Quaternion b;
	mVehicle->GetOrientation(&b);
	a.MultiplyByQuaternion(&b, &a);
	VecFx32 c = { 0, 0, mCamDistance };
	a.MultiplyByVector(&c, &c);*/
	VecFx32 vehcRot;
	mVehicle->GetOrientation(&vehcRot);

	fx32 yrot = FX_Atan2(vehcRot.z, vehcRot.x);
	yrot = FX_RAD_TO_DEG(yrot);
	yrot += mRotation.x;

	VecFx32 vehiclePos;
	mVehicle->GetPosition(&vehiclePos);
	vehiclePos.y += FX32_CONST(0.2f);
	mDestination = vehiclePos;

	

	mPosition.x = FX_Mul(FX_Mul(mCamDistance, FX_CosIdx(FX_DEG_TO_IDX(yrot))), mRotYCos) + mDestination.x;
	mPosition.y = FX_Mul(mCamDistance, mRotYSin) + mDestination.y;
	mPosition.z = FX_Mul(FX_Mul(mCamDistance, FX_SinIdx(FX_DEG_TO_IDX(yrot))), mRotYCos) + mDestination.z;

	LookAtCamera::Apply();
}