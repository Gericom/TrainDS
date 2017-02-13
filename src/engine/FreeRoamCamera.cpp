#include <nitro.h>
#include <nnsys/g3d.h>
#include "core.h"
#include "Camera.h"
#include "LookAtCamera.h"
#include "FreeRoamCamera.h"
#include "Quaternion.h"

void FreeRoamCamera::Apply()
{
	mPosition.x = FX_Mul(2 * mRotXCos, mRotYCos) + mDestination.x;
	mPosition.y = (2 * mRotYSin) + mDestination.y;
	mPosition.z = FX_Mul(2 * mRotXSin, mRotYCos) + mDestination.z;
	LookAtCamera::Apply();
}