#ifndef __HEMISPHERE_H__
#define __HEMISPHERE_H__

#define HEMISPHERE_NR_RINGS		16
#define HEMISPHERE_NR_SECTORS	16

class Hemisphere
{
	friend class TOTDController;
private:
	VecFx16 mVtx[HEMISPHERE_NR_RINGS * HEMISPHERE_NR_SECTORS];
	GXRgb mColors[HEMISPHERE_NR_RINGS * HEMISPHERE_NR_SECTORS];
public:
	Hemisphere();

	void Render();
};

#endif