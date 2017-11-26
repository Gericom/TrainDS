#pragma once

#define HEMISPHERE_NR_RINGS		16
#define HEMISPHERE_NR_SECTORS	16

class Hemisphere
{
	friend class TOTDController;
private:
	VecFx16 mVtx[HEMISPHERE_NR_RINGS * HEMISPHERE_NR_SECTORS];
	GXRgb mColors[2][HEMISPHERE_NR_RINGS * HEMISPHERE_NR_SECTORS];
	volatile int mColorBuf;
public:
	Hemisphere();

	void Render();
};