#ifndef __TRACKBUILDUISLICE_H__
#define __TRACKBUILDUISLICE_H__
#include "../UISlice.h"

#define TRACKBUILD_BUTTON_INVALID			(-1)
#define TRACKBUILD_BUTTON_DIR_STRAIGHT		0
#define TRACKBUILD_BUTTON_DIR_LEFT_BIG		1
#define TRACKBUILD_BUTTON_DIR_RIGHT_BIG		2
#define TRACKBUILD_BUTTON_DIR_LEFT_SMALL	3
#define TRACKBUILD_BUTTON_DIR_RIGHT_SMALL	4
#define TRACKBUILD_BUTTON_SLOPE_NONE		5
#define TRACKBUILD_BUTTON_SLOPE_DOWN		6
#define TRACKBUILD_BUTTON_SLOPE_UP			7

class TrackBuildUISlice : public UISlice
{
private:
	int mDownButton;

	int mDirectionSelection;
	int mSlopeSelection;

	int FindButtonByPoint(int x, int y);
public:
	TrackBuildUISlice()
		: UISlice(), mDownButton(TRACKBUILD_BUTTON_INVALID), mDirectionSelection(TRACKBUILD_BUTTON_DIR_STRAIGHT), mSlopeSelection(TRACKBUILD_BUTTON_SLOPE_NONE)
	{ }

	BOOL OnPenDown(Menu* context, int x, int y);
	BOOL OnPenMove(Menu* context, int x, int y);
	BOOL OnPenUp(Menu* context, int x, int y);

	void Render(Menu* context, NNSG2dOamManagerInstance* oamManager);
};

#endif