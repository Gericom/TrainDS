#ifndef __TOOL_H__
#define __TOOL_H__

class Game;

class Tool
{
protected:
	Game* mContext;

public:
	Tool(Game* context)
		: mContext(context)
	{}

	virtual void OnPenDown(int x, int y) = 0;
	virtual void OnPenMove(int x, int y) = 0;
	virtual void OnPenUp(int x, int y) = 0;
};

#endif