#ifndef __TRACKPIECE_H__
#define __TRACKPIECE_H__

#include <nnsys/fnd.h>
#include "terrain\Map.h"

#define TRACKPIECE_ROT_0						0	// ->
#define TRACKPIECE_ROT_90						1	// ^
#define TRACKPIECE_ROT_180						2	// <-
#define TRACKPIECE_ROT_270						3	// V

class TrackPiece
{
	friend class Map;
private:
	NNSFndLink mLink;
public:
	mapcoord_t mPosition;
	union
	{
		uint16_t mFlags;
		struct
		{
			uint16_t mRot : 2;
			uint16_t : 14;
		};
	};

	TrackPiece* mPrev[4];
	TrackPiece* mNext[4];

public:
	TrackPiece(fx32 x, fx32 y, fx32 z, int rot)
		: mRot(rot)
	{ 
		mPosition.x = x;
		mPosition.y = y;
		mPosition.z = z;
	}

	virtual void Render(TerrainManager* terrainManager) = 0;
	virtual fx32 GetNextDistance(fx32 linDist) = 0;
	virtual void CalculatePoint(VecFx32* pStartPos, VecFx32* pEndPos, VecFx32* pNextDir, fx32 progress, VecFx32* pPos, VecFx32* pDir) = 0;
};

#endif