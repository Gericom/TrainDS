#pragma once

class GameController;

//based on https://github.com/Pixelstudio/Skydome
class TOTDController
{
private:
	struct vec3
	{
		float x, y, z;
	};

	//VecFx32 mBr; //rayleigh
	//VecFx32 mBm; //mie
	//fx32 mG; //something with particle size for mie

	//VecFx32 mEsun;

	float JULIANDATE;
	float LONGITUDE;
	float LATITUDE;
	float MERIDIAN;
	float TIME;

	float LATITUDE_RADIANS;
	float LONGITUDE_RADIANS;
	float STD_MERIDIAN;

	//VecFx32 g_vEyePt;
	vec3 vBetaRayleigh;
	vec3 BetaRayTheta;
	vec3 vBetaMie;
	vec3 BetaMieTheta;
	VecFx16 LightDir;
	VecFx32 g_vSunColor;
	fx32 DirectionalityFactor;
	fx32 SunColorIntensity;
	fx32 tint;

	float m_fRayFactor;
	float m_fTurbidity;
	float m_fMieFactor;

	GameController* mGameController;

	void BetaR(float Theta, vec3* dst);
	void BetaM(float Theta, vec3* dst);
	void Lin(float Theta, float SR, float SM, VecFx32* dst);

	void calcAtmosphere();
	void calcRay();
	void CalculateMieCoeff();
	void initSunThetaPhi();
	void ComputeAttenuation(float m_fTheta);

	void CalcLightIn(fx32 s, fx32 cosAng, VecFx32* dst);

public:
	TOTDController(GameController* gameController)
		: /*mG(FX32_CONST(0.8)),*/ mGameController(gameController)
	{
		JULIANDATE = 117185; //150;
		LONGITUDE = 52.0f;
		LATITUDE = 4.0f;
		MERIDIAN = 0.0f;
		TIME = 8.0f;

		tint = FX32_CONST(1.9f);

		LATITUDE_RADIANS = 3.141592653589793f * LATITUDE / 180.f;
		LONGITUDE_RADIANS = 3.141592653589793f * LONGITUDE / 180.f;
		STD_MERIDIAN = MERIDIAN * 15.0f;

		m_fRayFactor = 1000;
		m_fTurbidity = 2;
		m_fMieFactor = 0.7f;
		LightDir.x = FX16_CONST(-0.657);
		LightDir.y = FX16_CONST(-0.024);
		LightDir.z = FX16_CONST(0.7758);
		/*vBetaRayleigh.x = FX32_CONST(0.0008);
		vBetaRayleigh.y = FX32_CONST(0.0014);
		vBetaRayleigh.z = FX32_CONST(0.0029);
		BetaRayTheta.x = FX32_CONST(0.0001);
		BetaRayTheta.y = FX32_CONST(0.0002);
		BetaRayTheta.z = FX32_CONST(0.0005);
		vBetaMie.x = FX32_CONST(0.0012);
		vBetaMie.y = FX32_CONST(0.0016);
		vBetaMie.z = FX32_CONST(0.0023);
		BetaMieTheta.x = FX32_CONST(0.0009);
		BetaMieTheta.y = FX32_CONST(0.0012);
		BetaMieTheta.z = FX32_CONST(0.0017);*/
		g_vSunColor.x = FX32_CONST(0.6878);
		g_vSunColor.y = FX32_CONST(0.5951);
		g_vSunColor.z = FX32_CONST(0.4217);
		SunColorIntensity = FX32_CONST(0.7 * 3.f);
		DirectionalityFactor = FX32_CONST(1.5);

		//mBr.x = 33;//24;//5602 / 100 / 2;
		//mBr.y = 57;// 55;
		//mBr.z = 119;//136;

		//mBm.x = 49;//16;// 5602 / 100 / 2;
		//mBm.y = 66;// 16;//10928 / 100 / 2;
		//mBm.z = 94;// 16;//24386 / 2;

		//mEsun.x = FX32_CONST(0.6878 * 75);
		//mEsun.y = FX32_CONST(0.5951 * 75);
		//mEsun.z = FX32_CONST(0.4217 * 75);
	}

	void Update();
};