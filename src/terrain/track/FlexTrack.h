#ifndef __FLEXTRACK_H__
#define __FLEXTRACK_H__

#include "TrackPiece.h"

class FlexTrack : public TrackPiece
{
public:
	mapcoord_t mEndPosition;
public:
	FlexTrack(fx32 x1, fx32 y1, fx32 z1, fx32 x2, fx32 y2, fx32 z2)
		: TrackPiece(x1, y1, z1, 0)
	{ 
		mEndPosition.x = x2;
		mEndPosition.y = y2;
		mEndPosition.z = z2;
	}

	void Render(TerrainManager* terrainManager);
	fx32 GetNextDistance(fx32 linDist);
	void CalculatePoint(VecFx32* pStartPos, VecFx32* pEndPos, VecFx32* pNextDir, fx32 progress, VecFx32* pPos, VecFx32* pDir);
};

#endif