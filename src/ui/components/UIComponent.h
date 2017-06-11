#pragma once

#include <nitro.h>
#include <nnsys/g2d.h>
#include <nnsys/fnd.h>

class UIManager;

class UIComponent
{
	friend class UIManager;
public:
	typedef void(*OnClickHandler)(void* arg);

private:
	NNSFndLink mLink;

protected:
	OnClickHandler mOnClickHandler;
	void* mOnClickHandlerArg;
	int mX, mY, mWidth, mHeight;

public:
	UIComponent(int x, int y, int width, int height)
		: mOnClickHandler(NULL), mX(x), mY(y), mWidth(width), mHeight(height)
	{

	}

	void SetOnClickHandler(OnClickHandler handler, void* arg)
	{
		mOnClickHandler = handler;
		mOnClickHandlerArg = arg;
	}

	void SetPosition(int x, int y)
	{
		mX = x;
		mY = y;
	}

	void GetPosition(int &x, int &y) { x = mX; y = mY; }

	virtual bool IsInside(int x, int y)
	{
		return x >= mX && y >= mY && x < mX + mWidth && y < mY + mHeight;
	}

	virtual void Render(GXOamAttr* pOAMBuffer, int &oamCount) = 0;

	virtual void OnPenDown(int x, int y) = 0;
	virtual void OnPenMove(int x, int y) = 0;
	virtual void OnPenUp(int x, int y) = 0;
};