#include "common.h"

#include "util.h"
#include "Wagon.h"
#include "Train.h"

Train::Train(GameController* gameController)
	: Vehicle(gameController)
{
	NNS_FND_INIT_LIST(&mWagonList, Wagon, mLink);
}

void Train::Update()
{
	Wagon* wagon = NULL;
	while ((wagon = (Wagon*)NNS_FndGetNextListObject(&mWagonList, wagon)) != NULL)
		wagon->Update();
}

void Train::Render()
{
	Wagon* wagon = NULL;
	while ((wagon = (Wagon*)NNS_FndGetNextListObject(&mWagonList, wagon)) != NULL)
		wagon->Render();
}

void Train::GetOrientation(VecFx32* dst)
{
	VecFx32 result = { 0 };
	VecFx32 tmp;
	int count = 0;
	Wagon* wagon = NULL;
	while ((wagon = (Wagon*)NNS_FndGetNextListObject(&mWagonList, wagon)) != NULL)
	{
		wagon->GetOrientation(&tmp);
		VEC_Add(&result, &tmp, &result);
		count++;
	}
	dst->x = result.x / count;
	dst->y = result.y / count;
	dst->z = result.z / count;
}

void Train::GetPosition(VecFx32* dst)
{
	VecFx32 result = { 0 };
	VecFx32 tmp;
	int count = 0;
	Wagon* wagon = NULL;
	while ((wagon = (Wagon*)NNS_FndGetNextListObject(&mWagonList, wagon)) != NULL)
	{
		wagon->GetPosition(&tmp);
		VEC_Add(&result, &tmp, &result);
		count++;
	}
	dst->x = result.x / count;
	dst->y = result.y / count;
	dst->z = result.z / count;
}

void Train::GetBounds(box2d_t* box)
{
	box2d_t result = { 0 };
	box2d_t tmp;
	Wagon* wagon = NULL;
	while ((wagon = (Wagon*)NNS_FndGetNextListObject(&mWagonList, wagon)) != NULL)
	{
		wagon->GetBounds(&tmp);
		if (tmp.x1 < result.x1)
			result.x1 = tmp.x1;
		if (tmp.y1 < result.y1)
			result.y1 = tmp.y1;
		if (tmp.x2 > result.x2)
			result.x2 = tmp.x2;
		if (tmp.y2 > result.y2)
			result.y2 = tmp.y2;
	}
	*box = result;
}

void Train::DestroyClearWagons()
{
	Wagon* wagon = (Wagon*)NNS_FndGetNextListObject(&mWagonList, NULL);
	while (wagon)
	{
		Wagon* next = (Wagon*)NNS_FndGetNextListObject(&mWagonList, wagon);
		NNS_FndRemoveListObject(&mWagonList, wagon);
		delete wagon;
		wagon = next;
	}
}

void Train::SetDriving(bool driving)
{
	Wagon* wagon = NULL;
	while ((wagon = (Wagon*)NNS_FndGetNextListObject(&mWagonList, wagon)) != NULL)
	{
		wagon->mDriving = driving;
	}
}

void Train::PutOnTrack(TrackPieceEx * track, int inPoint, fx32 offset)
{
	Wagon* wagon = NULL;
	while ((wagon = (Wagon*)NNS_FndGetPrevListObject(&mWagonList, wagon)) != NULL)
	{
		wagon->PutOnTrack(track, inPoint, offset);
		offset += wagon->GetLength() + FX32_CONST(0.05f);
	}
}
