#include <nitro.h>
#include "core.h"
#include "tile.h"

void tile_render(tile_t* tile)
{
	G3_Color(GX_RGB(129 >> 3, 159 >> 3, 42 >> 3));
	G3_Begin(GX_BEGIN_QUADS);
	{
		G3_Vtx(0, 0, 0);
		G3_Vtx(0, 0,  FX32_ONE);
		G3_Vtx(FX32_ONE, 0,  FX32_ONE);
		G3_Vtx(FX32_ONE, 0, 0);
	}
	G3_End();
}