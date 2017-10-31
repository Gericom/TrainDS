#include <nitro.h>
#include "core.h"
#include "util.h"
#include "PicturePane.h"

void PicturePane::RenderContent()
{
	if (mCell)
		NNS_G2dDrawCell(mCell);
}
