#ifndef __DRAGTOOL_H__
#define __DRAGTOOL_H__

#include "Tool.h"

class DragTool : public Tool
{
private:
	void OnPenDownPickingCallback(picking_result_t result);
	void OnPenMovePickingCallback(picking_result_t result);
	void OnPenUpPickingCallback(picking_result_t result);

	int mPickingX;
	int mPickingY;

	bool mDragStarted;
public:
	DragTool(Game* context)
		: Tool(context), mDragStarted(FALSE)
	{ }

	void OnPenDown(int x, int y);
	void OnPenMove(int x, int y);
	void OnPenUp(int x, int y);

	static void OnPenDownPickingCallback(void* arg, picking_result_t result)
	{
		((DragTool*)arg)->OnPenDownPickingCallback(result);
	}

	static void OnPenMovePickingCallback(void* arg, picking_result_t result)
	{
		((DragTool*)arg)->OnPenMovePickingCallback(result);
	}

	static void OnPenUpPickingCallback(void* arg, picking_result_t result)
	{
		((DragTool*)arg)->OnPenUpPickingCallback(result);
	}

	virtual void OnDragStart(VecFx32* position) = 0;
	virtual void OnDragging(VecFx32* position) = 0;
	virtual void OnDragEnd(VecFx32* position) = 0;
};

#endif