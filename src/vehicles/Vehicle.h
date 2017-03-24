#ifndef __VEHICLE_H__
#define __VEHICLE_H__

class Map;

class Vehicle
{
protected:
	Map* mMap;

public:
	Vehicle(Map* map)
		: mMap(map) { }
	virtual void Render() = 0;
	virtual void Update() = 0;

	virtual void GetCenterPos(VecFx32* dst) = 0;
};

#endif