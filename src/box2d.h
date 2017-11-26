#pragma once

struct box2d_t
{
	fx32 x1, y1;
	fx32 x2, y2;

	fx32 GetWidth() const
	{
		return x2 - x1;
	}

	fx32 GetHeight() const
	{
		return y2 - y1;
	}

	bool Intersects(box2d_t* b) const
	{
		return (x1 <= b->x2) && (y1 <= b->y2) &&
			(x2 >= b->x1) && (y2 >=b->y1);
	}

	bool Inside(fx32 x, fx32 y) const
	{
		return x >= x1 && x < x2 && y >= y1 && y < y2;
	}
};