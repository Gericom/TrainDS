#include <nitro.h>
#include <nnsys/g3d.h>
#include "core.h"
#include "Camera.h"
#include "LookAtCamera.h"
#include "FreeRoamCamera.h"
#include "../Quaternion.h"

void FreeRoamCamera::Apply()
{
	mPosition.x = FX_Mul(FX_Mul(mCamDistance, mRotXCos), mRotYCos) + mDestination.x;
	mPosition.y = FX_Mul(mCamDistance, mRotYSin) + mDestination.y;
	mPosition.z = FX_Mul(FX_Mul(mCamDistance, mRotXSin), mRotYCos) + mDestination.z;
	LookAtCamera::Apply();
}