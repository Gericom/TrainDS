#pragma once

#include "LookAtCamera.h"
class Vehicle;

class VehicleCamera : public LookAtCamera
{
private:
	VecFx32 mRotation;
	fx32 mRotXSin;
	fx32 mRotXCos;
	fx32 mRotYSin;
	fx32 mRotYCos;

	Vehicle* mVehicle;
public:
	fx32 mCamDistance;

	VehicleCamera(Vehicle* vehicle)
		: mCamDistance(2 * FX32_ONE), mVehicle(vehicle)
	{}

	void Apply();

	void GetRotation(VecFx32* rot)
	{
		*rot = mRotation;
	}

	void SetRotation(VecFx32* rot)
	{
		mRotation = *rot;
		UpdateSinCos();
	}

private:
	void UpdateSinCos()
	{
		mRotXSin = FX_SinIdx(FX_DEG_TO_IDX(mRotation.x));
		mRotXCos = FX_CosIdx(FX_DEG_TO_IDX(mRotation.x));
		mRotYSin = FX_SinIdx(FX_DEG_TO_IDX(mRotation.y));
		mRotYCos = FX_CosIdx(FX_DEG_TO_IDX(mRotation.y));
	}
};