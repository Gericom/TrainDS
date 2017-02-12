#ifndef __TRACKPIECEEX_H__
#define __TRACKPIECEEX_H__

class TrackPieceEx
{
	friend class Map;
private:
	NNSFndLink mLink;
public:
	virtual int GetNrConnectionPoints() = 0;
	virtual void GetConnectionPoint(int id, VecFx32* dst) = 0;
	virtual int GetOutPointId(int inPoint) = 0;
	virtual void GetConnnectedTrack(int id, TrackPieceEx* &track, int &inPoint) = 0;
	virtual void Connect(int id, TrackPieceEx* track, int inPoint, bool updatePos) = 0;
	virtual void Disconnect(int id) = 0;

	virtual void Render(Map* map, TerrainManager* terrainManager) = 0;
	virtual void RenderMarkers(Map* map, TerrainManager* terrainManager) = 0;
	virtual fx32 GetTrackLength(int inPoint) = 0;
	virtual void CalculatePoint(int inPoint, fx32 progress, VecFx32* pPos, VecFx32* pDir, Map* map) = 0;
};

#endif