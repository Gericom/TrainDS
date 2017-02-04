#ifndef __FREEROAMCAMERA_H__
#define __FREEROAMCAMERA_H__

#include "LookAtCamera.h"
#include "Quaternion.h"

class FreeRoamCamera : public LookAtCamera
{
private:
	VecFx32 mRotation;
	fx32 mRotXSin;
	fx32 mRotXCos;
	fx32 mRotYSin;
	fx32 mRotYCos;
public:
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

	void MoveInOut(fx32 amount)
	{
		mDestination.x += FX_Mul(FX_Mul(-amount, mRotXCos), mRotYCos);
		mDestination.y += FX_Mul(-amount, mRotYSin);
		mDestination.z += FX_Mul(FX_Mul(-amount, mRotXSin), mRotYCos);
	}

	void MoveX(fx32 amount)
	{
		mDestination.x += FX_Mul(amount, mRotXSin);
		mDestination.z -= FX_Mul(amount, mRotXCos);
	}

	void MoveY(fx32 amount)
	{
		//mDestination.x -= FX_Mul(FX_Mul(amount, mRotXCos), mRotYSin);
		//mDestination.y += FX_Mul(amount, mRotYCos);
		//mDestination.z -= FX_Mul(FX_Mul(amount, mRotXSin), mRotYSin);
		mDestination.y += amount;
	}

	void MoveZ(fx32 amount)
	{
		//mDestination.x += FX_Mul(FX_Mul(-amount, mRotXCos), mRotYCos);
		//mDestination.z += FX_Mul(FX_Mul(-amount, mRotXSin), mRotYCos);
		mDestination.x -= FX_Mul(amount, mRotXCos);
		mDestination.z -= FX_Mul(amount, mRotXSin);
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

#endif
