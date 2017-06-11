#pragma once

#include "Tool.h"

class PointerTool : public Tool
{
public:
	PointerTool(Game* context)
		: Tool(context)
	{ }

	void OnPenDown(int x, int y);
	void OnPenMove(int x, int y);
	void OnPenUp(int x, int y);
};