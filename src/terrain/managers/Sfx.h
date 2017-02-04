#ifndef __SFX_H__
#define __SFX_H__

#include <nnsys/fnd.h>
class SfxManager;

class Sfx
{
	friend class SfxManager;
private:
	NNSFndLink mLink;
protected:
	bool mHasStarted;
public:
	VecFx32 mPosition;
	//TODO: connect to object; these should have some base type MapObject with a position

	virtual ~Sfx() { }

	virtual void Start() = 0;
	virtual void Stop(int fade = 0) = 0;

	bool HasStarted()
	{
		return mHasStarted;
	}
	virtual bool IsPlaying() = 0;
};

#endif