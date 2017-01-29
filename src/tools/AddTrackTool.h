#ifndef __ADDTRACKTOOL_H__
#define __ADDTRACKTOOL_H__

#include "DragTool.h"

class AddTrackTool : public DragTool
{
private:
	VecFx32 mStartPos;
	FlexTrack* mNewTrackPiece;
public:
	AddTrackTool(Game* context)
		: DragTool(context)
	{}

	void OnDragStart(VecFx32* position);
	void OnDragging(VecFx32* position);
	void OnDragEnd(VecFx32* position);
};

#endif