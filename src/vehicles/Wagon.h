#ifndef __WAGON_H__
#define __WAGON_H__
#include <nnsys/g3d.h>

#include "Vehicle.h"

class Map;
class TrackPieceEx;
class PathWorker;
class Sfx;

class Wagon : public Vehicle
{
public:
	//I'm almost certain I'm not gonna keep it like this
	//eventually it should work with acceleration and velocity
	bool mDriving;
private:
	struct bogey_t
	{
		VecFx32 position;
		NNSG3dUtilResName nodeName;//for rotation
		PathWorker* pathWorker;
		VecFx32 direction;
		int jointId;
	};

	char mDirName[20];
	char* mName;
	VecFx32 mFront;
	VecFx32 mBack;
	int mNrBogeys;
	bogey_t mBogeys[10];

	NNSG3dResFileHeader* mModelData;
	NNSG3dRenderObj mRenderObj;

	bool mOnTrack;

	Sfx* mSfx;

	fx16 mCurRot;

	fx64c mAcceleration;
	fx64c mDeceleration;
	fx64c mSpeed;
	fx64c mMaxSpeed;
public:
	Wagon(GameController* gameController, char* name);
	void PutOnTrack(TrackPieceEx* track, int inPoint, fx32 offset = 0);
	void Render();
	void Update();
	void GetPosition(VecFx32* dst);

	static void StaticModifyBogeyRotation(NNSG3dRS* rs)
	{
		((Wagon*)rs->pRenderObj->ptrUser)->ModifyBogeyRotation(rs);
	}

	void ModifyBogeyRotation(NNSG3dRS* rs);
};

#endif