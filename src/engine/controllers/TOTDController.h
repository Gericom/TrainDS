#pragma once

class GameController;

#define TOTD_THREAD_STACK_SIZE	1024
#define TOTD_THREAD_PRIORITY   (OS_THREAD_LAUNCHER_PRIORITY + 15)

//#define ENABLE_NEW_TOTD

//based on https://github.com/Pixelstudio/Skydome
class TOTDController
{
private:
	struct vec3
	{
		float x, y, z;
	};

	float mCurJulianDate;
	float mLongitude;
	float mLatitude;
	float mMeridian;
	float mCurTime;

	float mNewJulianDate;
	float mNewTime;

	float mLatitudeRad;
	float mLongitudeRad;
	float mStdMeridian;

	vec3 vBetaRayleigh;
	vec3 BetaRayTheta;
	vec3 vBetaMie;
	vec3 BetaMieTheta;
	VecFx16 mLightDir;
	VecFx32 mSunColor;
	fx32 mDirectionalityFactor;
	fx32 mSunColorIntensity;

	float mRayFactor;
	float mTurbidity;
	float mMieFactor;

	GameController* mGameController;

	OSThread mTOTDThread;
	u32 mTOTDThreadStack[TOTD_THREAD_STACK_SIZE / sizeof(u32)];

	void BetaR(float Theta, vec3* dst);
	void BetaM(float Theta, vec3* dst);
	void Lin(float Theta, float SR, float SM, VecFx32* dst);

	void calcAtmosphere();
	void calcRay();
	void CalculateMieCoeff();
	void initSunThetaPhi();
	void ComputeAttenuation(float m_fTheta);

	void InternalUpdate();
	void TOTDThreadMain();

#ifdef ENABLE_NEW_TOTD
	bool solveQuadratic(float a, float b, float c, float& x1, float& x2);
	bool raySphereIntersect(const vec3& orig, const vec3& dir, const float& radius, float& t0, float& t1);
	bool computeIncidentLight(const vec3& orig, const vec3& dir, float tmin, float tmax, vec3 &result);

	//vec3 sunDirection;     // The sun direction (normalized) 
	float earthRadius;      // In the paper this is usually Rg or Re (radius ground, eart) 
	float atmosphereRadius; // In the paper this is usually R or Ra (radius atmosphere) 
	float Hr;               // Thickness of the atmosphere if density was uniform (Hr) 
	float Hm;               // Same as above but for Mie scattering (Hm) 

	static const vec3 betaR;
	static const vec3 betaM;
#endif

public:
	TOTDController(GameController* gameController)
		: /*mG(FX32_CONST(0.8)),*/ mGameController(gameController)
	{
		mCurJulianDate = 117185; //150;
		mLongitude = 52.0f;
		mLatitude = 4.0f;
		mMeridian = 0.0f;
		mCurTime = 8.0f;

		mNewJulianDate = mCurJulianDate;
		mNewTime = mCurTime;

		mLatitudeRad = 3.141592653589793f * mLatitude / 180.f;
		mLongitudeRad = 3.141592653589793f * mLongitude / 180.f;
		mStdMeridian = mMeridian * 15.0f;

		mRayFactor = 1000;
		mTurbidity = 2;
		mMieFactor = 0.7f;
		mLightDir.x = FX16_CONST(-0.657);
		mLightDir.y = FX16_CONST(-0.024);
		mLightDir.z = FX16_CONST(0.7758);

		mSunColor.x = FX32_CONST(0.6878);
		mSunColor.y = FX32_CONST(0.5951);
		mSunColor.z = FX32_CONST(0.4217);
		mSunColorIntensity = FX32_CONST(0.7 * 3.f);
		mDirectionalityFactor = FX32_CONST(1.5);

#ifdef ENABLE_NEW_TOTD
		earthRadius = 6360e3;
		atmosphereRadius = 6420e3;
		Hr = 7994;
		Hm = 1200;
#endif

		InternalUpdate();

		OS_CreateThread(&mTOTDThread, TOTDThreadMain, this, mTOTDThreadStack + TOTD_THREAD_STACK_SIZE / sizeof(u32), TOTD_THREAD_STACK_SIZE, TOTD_THREAD_PRIORITY);
		OS_WakeupThreadDirect(&mTOTDThread);
	}

	~TOTDController()
	{
		OS_DestroyThread(&mTOTDThread);
	}

	void Update();

	void SetDateTime(float julianDate, float time)
	{
		OSIntrMode old = OS_DisableInterrupts();
		mNewJulianDate = julianDate;
		mNewTime = time;
		OS_RestoreInterrupts(old);
	}

	static void TOTDThreadMain(void* arg)
	{
		((TOTDController*)arg)->TOTDThreadMain();
	}
};