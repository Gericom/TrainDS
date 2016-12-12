#ifndef __TRACKPIECE_H__
#define __TRACKPIECE_H__

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
	uint16_t x;
	uint16_t y;
	uint16_t z;
	union
	{
		uint16_t flags;
		struct
		{
			uint16_t rot : 2;
			uint16_t reserved_flags : 14;
		};
	};

	TrackPiece* prev[4];
	TrackPiece* next[4];

public:
	TrackPiece(uint16_t x, uint16_t y, uint16_t z, int rot)
		: x(x), y(y), z(z), rot(rot)
	{ }

	virtual void Render(TerrainManager* terrainManager) = 0;
	virtual fx32 GetNextDistance(fx32 linDist) = 0;
	virtual void CalculatePoint(VecFx32* pStartPos, VecFx32* pEndPos, VecFx32* pNextDir, fx32 progress, VecFx32* pPos, VecFx32* pDir) = 0;
};

#endif