#include <nitro.h>
#include <nnsys/g3d.h>
#include "core.h"
#include "Camera.h"
#include "LookAtCamera.h"

void LookAtCamera::Apply()
{
	NNS_G3dGlbLookAt(&mPosition, &mUp, &mDestination);
}