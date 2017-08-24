#pragma once
#include "Vehicle.h"

class Train : public Vehicle
{
private:
	NNSFndList mWagonList;

public:
	Train(GameController* gameController);

	virtual void Update();
	virtual void Render();

	virtual void GetOrientation(VecFx32* dst);

	virtual void GetPosition(VecFx32* dst);
	virtual void GetBounds(box2d_t* box);

	void AddWagon(Wagon* wagon)
	{
		NNS_FndAppendListObject(&mWagonList, wagon);
	}

	Wagon* GetFirstWagon()
	{
		return (Wagon*)NNS_FndGetNextListObject(&mWagonList, NULL);
	}

	void DestroyClearWagons();

	void SetDriving(bool driving);

	void PutOnTrack(TrackPieceEx* track, int inPoint, fx32 offset = 0);
};
