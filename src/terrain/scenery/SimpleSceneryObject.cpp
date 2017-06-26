#include <nitro.h>
#include <nnsys/g3d.h>
#include "core.h"
#include "util.h"
#include "engine/objects/WorldObject.h"
#include "io/ObjectData.h"
#include "terrain/Map.h"
#include "SimpleSceneryObject.h"

SimpleSceneryObject::SimpleSceneryObject(Map* map, u32 type, fx32 x, fx32 z, fx32 rotY)
	: mMap(map), mObjectType(type), mRotY(FX_DEG_TO_IDX(rotY + 90 * FX32_ONE))
{
	mPosition.x = x;
	mPosition.z = z;
	NNS_G3dRenderObjInit(&mRenderObj, NNS_G3dGetMdlByIdx(NNS_G3dGetMdlSet(mMap->mObjectData->mLoadedObjects[type].modelData), 0));
	mUse1Mat1ShpRendering = mRenderObj.resMdl->info.numShp == 1;
}

void SimpleSceneryObject::Render()
{
	NNS_G3dGePushMtx();
	{
		NNS_G3dGeTranslateVec(&mPosition);
		MtxFx33 rotmtx;
		MTX_RotY33(&rotmtx, FX_SinIdx(mRotY), FX_CosIdx(mRotY));
		NNS_G3dGeMultMtx33(&rotmtx);
		NNS_G3dGeScale(FX32_CONST(0.1f), FX32_CONST(0.1f), FX32_CONST(0.1f));
		if (mUse1Mat1ShpRendering)
			NNS_G3dDraw1Mat1Shp(mRenderObj.resMdl, 0, 0, true);
		else
			NNS_G3dDraw(&mRenderObj);
	}
	NNS_G3dGePopMtx(1);
	NNS_G3dGeFlushBuffer();
}

void SimpleSceneryObject::Invalidate()
{
	mPosition.y = mMap->GetYOnMap(mPosition.x, mPosition.z);
}