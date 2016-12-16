#include <nitro.h>
#include <nnsys/fnd.h>
#include "core.h"
#include "terrain.h"
#include "TerrainManager.h"
#include "terrain/track/TrackPiece.h"
#include "terrain/scenery/SceneryObject.h"
#include "Map.h"

Map::Map()
	: mGridEnabled(FALSE)
{
	NNS_FND_INIT_LIST(&mTrackList, TrackPiece, mLink);
	NNS_FND_INIT_LIST(&mSceneryList, SceneryObject, mLink);
	mTerrainManager = new TerrainManager();
}

Map::~Map()
{
	delete mTerrainManager;
}

void Map::Render(int xstart, int xend, int zstart, int zend, BOOL picking, int selectedMapX, int selectedMapZ)
{
	G3_Translate(-32 * FX32_ONE, 0, -32 * FX32_ONE);
	G3_PushMtx();
	{
		int i = 0;
		for (int y = zstart; y < zend; y++)
		{
			for (int x = xstart; x < xend; x++)
			{
				G3_PushMtx();
				{
					if (picking) G3_MaterialColorSpecEmi(0, PICKING_COLOR(PICKING_TYPE_MAP, i + 1), FALSE);
					else if (selectedMapX == x && selectedMapZ == y)
					{
						G3_MaterialColorDiffAmb(GX_RGB(0, 0, 0), GX_RGB(0, 0, 0), FALSE);
						G3_MaterialColorSpecEmi(GX_RGB(0, 0, 0), GX_RGB(31, 31, 31), FALSE);
						G3_PolygonAttr(GX_LIGHTMASK_0, GX_POLYGONMODE_TOON, GX_CULL_NONE, 1, 31, GX_POLYGON_ATTR_MISC_FOG);
					}
					else if (mGridEnabled)
						G3_PolygonAttr(GX_LIGHTMASK_0, GX_POLYGONMODE_MODULATE, GX_CULL_BACK, ((x & 1) ^ (y & 1)) << 1, 31, GX_POLYGON_ATTR_MISC_FOG);
					G3_Translate(x * FX32_ONE, 0, y * FX32_ONE);
					tile_render(&mTiles[y][x], mTerrainManager);
					if (!picking && selectedMapX == x && selectedMapZ == y)
					{
						G3_MaterialColorDiffAmb(GX_RGB(21, 21, 21), GX_RGB(15, 15, 15), FALSE);
						G3_MaterialColorSpecEmi(GX_RGB(0, 0, 0), GX_RGB(0, 0, 0), FALSE);
						G3_PolygonAttr(GX_LIGHTMASK_0, GX_POLYGONMODE_MODULATE, GX_CULL_BACK, 0, 31, GX_POLYGON_ATTR_MISC_FOG);
					}
				}
				G3_PopMtx(1);
				i++;
			}
		}
		//NOCASH_Printf("Total quads: %d", i);
	}
	G3_PopMtx(1);
	TrackPiece* trackPiece = NULL;
	while ((trackPiece = (TrackPiece*)NNS_FndGetNextListObject(&mTrackList, trackPiece)) != NULL)
	{
		if (trackPiece->mPosition.x >= xstart && trackPiece->mPosition.x < xend &&
			trackPiece->mPosition.z >= zstart && trackPiece->mPosition.z < zend)
		{
			if (picking) G3_MaterialColorSpecEmi(0, 0, FALSE);
			trackPiece->Render(mTerrainManager);
		}
	}
	SceneryObject* sceneryObject = NULL;
	while ((sceneryObject = (SceneryObject*)NNS_FndGetNextListObject(&mSceneryList, sceneryObject)) != NULL)
	{
		if (sceneryObject->mPosition.x >= xstart && sceneryObject->mPosition.x < xend &&
			sceneryObject->mPosition.z >= zstart && sceneryObject->mPosition.z < zend)
		{
			if (picking) G3_MaterialColorSpecEmi(0, 0, FALSE);
			sceneryObject->Render(mTerrainManager);
		}
	}
}