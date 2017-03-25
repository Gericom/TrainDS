#ifndef __VEHICLE_H__
#define __VEHICLE_H__

#include "engine/WorldObject.h"

class GameController;

class Vehicle : public WorldObject
{
protected:
	GameController* mGameController;

public:
	Vehicle(GameController* gameController)
		: mGameController(gameController) { }
	virtual void Render() = 0;
	virtual void Update() = 0;

	virtual void GetPosition(VecFx32* dst) = 0;
};

#endif