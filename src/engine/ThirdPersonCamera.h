#ifndef __THIRDPERSONCAMERA_H__
#define __THIRDPERSONCAMERA_H__

typedef struct train_t;

//A camera that looks at the locomotive of a train
//The camera can be seen as a point on a sphere with radius r
//that looks at the center of the sphere
//x = r sin(mTheta) cos(mPhi)
//y = r sin(mTheta) sin(mPhi)
//z = r cos(mTheta)
class ThirdPersonCamera : public Camera
{
public:
	train_t* mTrain;
	fx32 mRadius;
	fx32 mTheta;//inclination
	fx32 mPhi;//azimuth

	void Apply();
};

#endif