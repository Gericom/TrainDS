#ifndef __SFX_H__
#define __SFX_H__

#include <nnsys/fnd.h>
#include "engine/LookAtCamera.h"
#include "engine/objects/WorldObject.h"

class SfxManager;

class Sfx
{
	friend class SfxManager;
private:
	NNSFndLink mLink;
protected:
	bool mHasStarted;
public:
	VecFx32 mPosition;//world position if mParent == NULL, relative position otherwise
	WorldObject* mParent;

	virtual ~Sfx() { }

	virtual void Start() = 0;
	virtual void Stop(int fade = 0) = 0;

	virtual void SetPan(u8 pan) = 0;
	virtual void SetVolume(u8 volume) = 0;

	void GetPosition(VecFx32* dst)
	{
		if (mParent != NULL)
		{
			VecFx32 tmp;
			mParent->GetPosition(&tmp);
			VEC_Add(&tmp, &mPosition, dst);
		}
		else
			*dst = mPosition;
	}

	virtual void Update(LookAtCamera* camera)
	{
		if (!IsPlaying())
			return;
		VecFx32 pos;
		GetPosition(&pos);

		VecFx32 traintocam;
		traintocam.x = (pos.x - 32 * FX32_ONE) - camera->mPosition.x;
		traintocam.y = pos.y - camera->mPosition.y;
		traintocam.z = (pos.z - 32 * FX32_ONE) - camera->mPosition.z;
		fx32 dist = VEC_Mag(&traintocam);
		//traintocam.y = 0;
		VEC_Normalize(&traintocam, &traintocam);

		VecFx32 lookdir;
		VEC_Subtract(&camera->mDestination, &camera->mPosition, &lookdir);
		VEC_Normalize(&lookdir, &lookdir);

		VecFx32 side;
		VEC_CrossProduct(&lookdir, &camera->mUp, &side);
		VEC_Normalize(&side, &side);

		//VecFx32 panvec = traintocam;
		//panvec.y = 0;
		//VEC_Normalize(&panvec, &panvec);
		//VecFx32 m_right = {FX32_ONE, 0, 0};
		fx32 dot = VEC_DotProduct(&side, &traintocam);
		s32 pan = (dot * 128) >> FX32_SHIFT;
		if (pan < -128) { pan = -128; }
		if (pan > 127) { pan = 127; }

		fx32 revdist = (FX_Mul((8 * FX32_ONE) - dist, 256) * 127) >> FX32_SHIFT;
		if (revdist < 0) revdist = 0;
		if (revdist > 127) revdist = 127;

		SetPan(pan);
		SetVolume(revdist);
	}

	bool HasStarted()
	{
		return mHasStarted;
	}
	virtual bool IsPlaying() = 0;
};

#endif