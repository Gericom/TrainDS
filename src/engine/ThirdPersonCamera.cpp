#include <nitro.h>
#include <nnsys/g3d.h>
#include "core.h"
#include "Camera.h"
#include "ThirdPersonCamera.h"

void ThirdPersonCamera::Apply()
{
	//NNS_G3dGlbLookAt(&mPosition, &mUp, &mDestination);
	//fx32 x = r sin(mTheta) cos(mPhi)
	//fx32 y = r sin(mTheta) sin(mPhi)
	//fx32 z = r cos(mTheta)
}