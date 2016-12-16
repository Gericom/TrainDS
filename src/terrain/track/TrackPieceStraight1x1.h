#ifndef __TRACKPIECESTRAIGHT1X1_H__
#define __TRACKPIECESTRAIGHT1X1_H__

#include "TrackPiece.h"

class TerrainManager;

class TrackPieceStraight1x1 : public TrackPiece
{
public:
	TrackPieceStraight1x1(uint16_t x, uint8_t y, uint16_t z, int rot)
		: TrackPiece(x, y, z, rot)
	{ }

	virtual void Render(TerrainManager* terrainManager);
	virtual fx32 GetNextDistance(fx32 linDist);
	virtual void CalculatePoint(VecFx32* pStartPos, VecFx32* pEndPos, VecFx32* pNextDir, fx32 progress, VecFx32* pPos, VecFx32* pDir);
};

#endif