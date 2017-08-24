#ifndef __TRACKPIECEEX_H__
#define __TRACKPIECEEX_H__

#include "box2d.h"
#include "engine/objects/WorldObject.h"
#include "TrackVertex.h"

class TerrainManager;
class Map;

class TrackPieceEx : public WorldObject
{
	friend class Map;
private:
	NNSFndLink mLink;
protected:
	Map* mMap;
public:
	TrackPieceEx(Map* map)
		: mMap(map)
	{ }

	virtual int GetNrConnectionPoints() = 0;
	//virtual void GetConnectionPoint(int id, VecFx32* dst) = 0;
	virtual int GetOutPointId(int inPoint) = 0;
	virtual TrackVertex* GetVertex(int id) = 0;
	virtual void ConnectVertex(int id, TrackVertex* vtx) = 0;
	virtual void DisconnectVertex(int id) = 0;
	//virtual void GetConnnectedTrack(int id, TrackPieceEx* &track, int &inPoint) = 0;
	//virtual void Connect(int id, TrackPieceEx* track, int inPoint, bool updatePos) = 0;
	//virtual void Disconnect(int id) = 0;

	virtual void Render() = 0;
	virtual void RenderMarkers() = 0;
	virtual fx32 GetTrackLength(int inPoint) = 0;
	virtual void CalculatePoint(int inPoint, fx32 progress, VecFx32* pPos, VecFx32* pDir) = 0;

	virtual void Invalidate() = 0;

	virtual void GetPosition(VecFx32* dst) = 0;
	virtual void GetBounds(box2d_t* box) = 0;
};

#endif