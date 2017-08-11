#include <nitro.h>
#include <nnsys/g3d.h>
#include "core.h"
#include "util.h"
#include "engine/objects/WorldObject.h"
#include "io/ObjectData.h"
#include "terrain/Map.h"
#include "SimpleSceneryObject.h"

SimpleSceneryObject::SimpleSceneryObject(Map* map, u32 type, fx32 x, fx32 z, fx32 rotY, bool billboard)
	: mMap(map), mObjectType(type), mRotY(FX_DEG_TO_IDX(rotY + 90 * FX32_ONE)), mBillboard(billboard)
{
	mPosition.x = x;
	mPosition.z = z;
	NNS_G3dRenderObjInit(&mRenderObj, NNS_G3dGetMdlByIdx(NNS_G3dGetMdlSet(mMap->mObjectData->mLoadedObjects[type].modelData), 0));
	mUse1Mat1ShpRendering = /*mBillboard || */mRenderObj.resMdl->info.numShp == 1;
	mBillboard = mBillboard && mRenderObj.resMdl->info.numShp == 1;
}

void SimpleSceneryObject::Render()
{
	G3_PushMtx();
	{
		G3_Translate(mPosition.x, mPosition.y, mPosition.z);
		G3_PositionTest(0, 0, 0);
		VecFx32 vec;
		fx32 w;
		while (G3X_GetPositionTestResult(&vec, &w));
		fx32 neww = FX_Mul(FX32_CONST(1.2), w);
		if (vec.x >= -neww && vec.y >= -neww && vec.x <= neww && vec.y <= neww)
		{
			if (!mBillboard)
			{
				MtxFx33 rotmtx;
				MTX_RotY33(&rotmtx, FX_SinIdx(mRotY), FX_CosIdx(mRotY));
				G3_MultMtx33(&rotmtx);
			}
			else
				Util_SetupBillboardYMatrix();
			G3_Scale(FX32_CONST(0.1f), FX32_CONST(0.1f), FX32_CONST(0.1f));
			if (mUse1Mat1ShpRendering)
				NNS_G3dDraw1Mat1Shp(mRenderObj.resMdl, 0, 0, true);
			else
				NNS_G3dDraw(&mRenderObj);
			NNS_G3dGeFlushBuffer();
		}
	}
	G3_PopMtx(1);
}

void SimpleSceneryObject::Invalidate()
{
	mPosition.y = mMap->GetYOnMap(mPosition.x, mPosition.z);
}