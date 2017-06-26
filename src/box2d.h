#pragma once

struct box2d_t
{
	fx32 x1, y1;
	fx32 x2, y2;

	inline fx32 GetWidth()
	{
		return x2 - x1;
	}

	inline fx32 GetHeight()
	{
		return y2 - y1;
	}

	inline bool Intersects(box2d_t* b)
	{
		return (x1 <= b->x2) && (y1 <= b->y2) &&
			(x2 >= b->x1) && (y2 >=b->y1);
	}

	inline bool Inside(fx32 x, fx32 y)
	{
		return x >= x1 && x < x2 && y >= y1 && y < y2;
	}
};