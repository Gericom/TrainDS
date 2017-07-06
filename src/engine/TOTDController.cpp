#include <nitro.h>
#include "core.h"
#include <math.h>
#include <float.h>
#include "Hemisphere.h"
#include "terrain/GameController.h"
#include "TOTDController.h"

/*void TOTDController::CalcLightIn(fx32 s, fx32 cosAng, VecFx32* dst)
{
	//r
	{
		fx32 eexp = -FX_Mul(mBm.x + mBr.x, s);

		fx32 e2 = FX_F32_TO_FX32(exp(FX_FX32_TO_F32(eexp)));

		fx32 brang = FX_Mul(FX_Mul(mBr.x, FX32_ONE + FX_Mul(cosAng, cosAng)), 244);

		fx32 gbottom = FX32_ONE + FX_Mul(mG, mG) - FX_Mul(2 * mG, cosAng);
		gbottom = FX_Mul(FX_Mul(gbottom, FX_Sqrt(gbottom)), 51472);

		fx32 gtop = FX_Mul(FX32_ONE - mG, FX32_ONE - mG);
		fx32 bmang = FX_Div(FX_Mul(mBm.x, gtop), gbottom);

		fx32 resultcoef = FX_Div(FX_Mul(brang + bmang, FX32_ONE - e2), mBr.x + mBm.x);
		dst->x = /*e2 + /FX_Mul(mEsun.x, resultcoef);
	}
	//g
	{
		fx32 eexp = -FX_Mul(mBm.y + mBr.y, s);

		fx32 e2 = FX_F32_TO_FX32(exp(FX_FX32_TO_F32(eexp)));

		fx32 brang = FX_Mul(FX_Mul(mBr.y, FX32_ONE + FX_Mul(cosAng, cosAng)), 244);

		fx32 gbottom = FX32_ONE + FX_Mul(mG, mG) - FX_Mul(2 * mG, cosAng);
		gbottom = FX_Mul(FX_Mul(gbottom, FX_Sqrt(gbottom)), 51472);

		fx32 gtop = FX_Mul(FX32_ONE - mG, FX32_ONE - mG);
		fx32 bmang = FX_Div(FX_Mul(mBm.y, gtop), gbottom);

		fx32 resultcoef = FX_Div(FX_Mul(brang + bmang, FX32_ONE - e2), mBr.y + mBm.y);
		dst->y =/*e2 + /FX_Mul(mEsun.y, resultcoef);
	}
	//b
	{
		fx32 eexp = -FX_Mul(mBm.z + mBr.z, s);

		fx32 e2 = FX_F32_TO_FX32(exp(FX_FX32_TO_F32(eexp)));

		fx32 brang = FX_Mul(FX_Mul(mBr.z, FX32_ONE + FX_Mul(cosAng, cosAng)), 244);

		fx32 gbottom = FX32_ONE + FX_Mul(mG, mG) - FX_Mul(2 * mG, cosAng);
		gbottom = FX_Mul(FX_Mul(gbottom, FX_Sqrt(gbottom)), 51472);

		fx32 gtop = FX_Mul(FX32_ONE - mG, FX32_ONE - mG);
		fx32 bmang = FX_Div(FX_Mul(mBm.z, gtop), gbottom);

		fx32 resultcoef = FX_Div(FX_Mul(brang + bmang, FX32_ONE - e2), mBr.z + mBm.z);
		dst->z = /*e2 + /FX_Mul(mEsun.z, resultcoef);
	}
}*/

void TOTDController::BetaR(float Theta, vec3* dst) 
{
	float t2 = 3.f + 0.5f * Theta * Theta;
	dst->x = BetaRayTheta.x * t2;
	dst->y = BetaRayTheta.y * t2;
	dst->z = BetaRayTheta.z * t2;
}

void TOTDController::BetaM(float Theta, vec3* dst)
{
	float g = FX_FX32_TO_F32(DirectionalityFactor);
	float bottom = 1.f + g * g - 2.f * g * Theta;
	bottom *= sqrtf(bottom);
	float factor = (1.f - g) * (1.f - g);
	dst->x = BetaMieTheta.x * factor / bottom;
	dst->y = BetaMieTheta.y * factor / bottom;
	dst->z = BetaMieTheta.z * factor / bottom;
}

void TOTDController::Lin(float Theta, float SR, float SM, VecFx32* dst)
{
	vec3 betaR, betaM;
	BetaR(Theta, &betaR);
	BetaM(Theta, &betaM);

	vec3 bottom =
	{
		vBetaRayleigh.x + vBetaMie.x,
		vBetaRayleigh.y + vBetaMie.y,
		vBetaRayleigh.z + vBetaMie.z
	};

	vec3 e =
	{
		1.f - expf(-(vBetaRayleigh.x * SR + vBetaMie.x * SM)),
		1.f - expf(-(vBetaRayleigh.y * SR + vBetaMie.y * SM)),
		1.f - expf(-(vBetaRayleigh.z * SR + vBetaMie.z * SM))
	};

	dst->x = FX_F32_TO_FX32((betaR.x + betaM.x) * e.x / bottom.x);
	dst->y = FX_F32_TO_FX32((betaR.y + betaM.y) * e.y / bottom.y);
	dst->z = FX_F32_TO_FX32((betaR.z + betaM.z) * e.z / bottom.z);
}

void TOTDController::calcAtmosphere()
{
	calcRay();
	CalculateMieCoeff();
}

void TOTDController::calcRay()
{
	const float n = 1.00029f;		//Refraction index for air
	const float N = 2.545e25f;		//Molecules per unit volume
	const float pn = 0.035f;		//Depolarization factor

	float fRayleighFactor = m_fRayFactor * (powf(3.141592653589793f, 2.0f) * powf(n * n - 1.0f, 2.0f) * (6 + 3 * pn)) / (N * (6 - 7 * pn));

	BetaRayTheta.x = fRayleighFactor / (2.0f * powf(650.0e-9f, 4.0f));
	BetaRayTheta.y = fRayleighFactor / (2.0f * powf(570.0e-9f, 4.0f));
	BetaRayTheta.z = fRayleighFactor / (2.0f * powf(475.0e-9f, 4.0f));

	vBetaRayleigh.x = 8.0f * fRayleighFactor / (3.0f * powf(650.0e-9f, 4.0f));
	vBetaRayleigh.y = 8.0f * fRayleighFactor / (3.0f * powf(570.0e-9f, 4.0f));
	vBetaRayleigh.z = 8.0f * fRayleighFactor / (3.0f * powf(475.0e-9f, 4.0f));
}

void TOTDController::CalculateMieCoeff()
{
	float K[3];
	K[0] = 0.685f;
	K[1] = 0.682f;
	K[2] = 0.670f;

	float c = (0.6544f * m_fTurbidity - 0.6510f) * 1e-16f;	//Concentration factor

	float fMieFactor = m_fMieFactor * 0.434f * c * 4.0f * 3.141592653589793f * 3.141592653589793f;

	BetaMieTheta.x = fMieFactor / (2.0f * powf(650e-9f, 2.0f));
	BetaMieTheta.y = fMieFactor / (2.0f * powf(570e-9f, 2.0f));
	BetaMieTheta.z = fMieFactor / (2.0f * powf(475e-9f, 2.0f));

	vBetaMie.x = K[0] * fMieFactor / powf(650e-9f, 2.0f);
	vBetaMie.y = K[1] * fMieFactor / powf(570e-9f, 2.0f);
	vBetaMie.z = K[2] * fMieFactor / powf(475e-9f, 2.0f);
}

void TOTDController::ComputeAttenuation(float m_fTheta)
{
	float fBeta = 0.04608365822050f * m_fTurbidity - 0.04586025928522f;
	float fTauR, fTauA;
	float fTau[3];
	//float tmp = 93.885f - (m_fTheta / Mathf.PI * 180.0f);

	float m;

	/*if(!(TIME > 5 && TIME < 18))
	{
	m = (float)(1.0f / (Mathf.Cos (m_fTheta) + 0.15f * tmp));
	}
	else
	{*/
	// Relative Optical Mass
	if (TIME > 5.82 && TIME < 18.1)
	{
		//cloudTint = 1;
		m = (float)(1.0f / (cosf(m_fTheta) + 0.15f * powf(93.885f - m_fTheta / 3.141592653589793f * 180.0f, -1.253f)));
	}
	else
	{
		//cloudTint = 0;
		m = 20;
	}
	if (m < 0)
	{
		m = 20;
	}
	float fLambda[3];
	fLambda[0] = 0.65f;	// red (in um.)
	fLambda[1] = 0.57f;	// green (in um.)
	fLambda[2] = 0.475f;// blue (in um.)
	for (int i = 0; i < 3; i++) {
		// Rayleigh Scattering
		// Results agree with the graph (pg 115, MI) 
		// lambda in um.
		fTauR = expf(-m * 0.008735f * powf(fLambda[i], -4.08f));

		// Aerosal (water + dust) attenuation
		// beta - amount of aerosols present 
		// alpha - ratio of small to large particle sizes. (0:4,usually 1.3)
		// Results agree with the graph (pg 121, MI) 
		const float fAlpha = 1.3f;
		fTauA = expf(-m * fBeta * powf(fLambda[i], -fAlpha));
		// lambda should be in um
		fTau[i] = fTauR * fTauA;
	}

	VEC_Set(&g_vSunColor, FX_F32_TO_FX32(fTau[0]), FX_F32_TO_FX32(fTau[1]), FX_F32_TO_FX32(fTau[2]));
	int r = (g_vSunColor.x * 31) >> 12;
	r = MATH_CLAMP(r, 0, 31);
	int g = (g_vSunColor.y * 31) >> 12;
	g = MATH_CLAMP(g, 0, 31);
	int b = (g_vSunColor.z * 31) >> 12;
	b = MATH_CLAMP(b, 0, 31);
	mGameController->mFogColor = GX_RGB(r, g, b);
	mGameController->mLightColor = GX_RGB(r, g, b);
}

static void calcDirection(float thetaSun, float phiSun, VecFx32* dst)
{
	dst->x = FX_F32_TO_FX32(cosf(0.5f * 3.141592653589793f - thetaSun) * cosf(phiSun));
	dst->y = FX_F32_TO_FX32(sinf(0.5f * 3.141592653589793f - thetaSun));
	dst->z = FX_F32_TO_FX32(cosf(0.5f * 3.141592653589793f - thetaSun) * sinf(phiSun));
	VEC_Normalize(dst, dst);
}

static void SphericalToCartesian(float x, float y, float z, VecFx32* dst)
{
	dst->y = FX_F32_TO_FX32(x * sinf(z));
	float a = x*cosf(z);
	dst->x = FX_F32_TO_FX32(a*cosf(y));
	dst->z = FX_F32_TO_FX32(a*sinf(y));
}

void TOTDController::initSunThetaPhi()
{
	float solarDeclination, opp, adj, solarTime, solarAzimuth, solarAltitude;
	solarTime = TIME + (0.170f* sinf(4.f*3.141592653589793f*(JULIANDATE - 80.f) / 373.f) - 0.129f*sinf(2.f*3.141592653589793f*(JULIANDATE - 8.f) / 355.f)) + (STD_MERIDIAN - LONGITUDE_RADIANS) / 15.0f;
	solarDeclination = (0.4093f*sinf(2.f*3.141592653589793f*(JULIANDATE - 81.f) / 368.f));
	solarAltitude = asinf(sinf(LATITUDE_RADIANS) * sinf(solarDeclination) -
		cosf(LATITUDE_RADIANS) * cosf(solarDeclination) * cosf(3.141592653589793f*solarTime / 12.f));

	opp = -cosf(solarDeclination) * sinf(3.141592653589793f*solarTime / 12.f);
	adj = -(cosf(LATITUDE_RADIANS) * sinf(solarDeclination) +
		sinf(LATITUDE_RADIANS) * cosf(solarDeclination) *  cosf(3.141592653589793f*solarTime / 12.f));
	solarAzimuth = atan2f(opp, adj);

	float phiS = -solarAzimuth;
	float thetaS = 3.141592653589793f / 2.0f - solarAltitude;


	//Vector3 sunDirection = new Vector3(domeRadius, phiS, solarAltitude);
	VecFx32 sunDirection;
	SphericalToCartesian(50, phiS, solarAltitude, &sunDirection);

	mGameController->mSunPosition = sunDirection;

	VecFx32 sunDirection2;
	calcDirection(thetaS, phiS, &sunDirection2);

	VecFx32 ldir;
	VEC_Subtract(&sunDirection2, &sunDirection, &ldir);
	VEC_Normalize(&ldir, &ldir);
	LightDir.x = ldir.x;
	LightDir.y = ldir.y;
	LightDir.z = ldir.z;

	if (ldir.x > GX_FX32_FX10_MAX)
		ldir.x = GX_FX32_FX10_MAX;
	if (ldir.y > GX_FX32_FX10_MAX)
		ldir.y = GX_FX32_FX10_MAX;
	if (ldir.z > GX_FX32_FX10_MAX)
		ldir.z = GX_FX32_FX10_MAX;

	mGameController->mLightDirection.x = ldir.x;
	mGameController->mLightDirection.y = ldir.y;
	mGameController->mLightDirection.z = ldir.z;

	//sunObj.transform.position = SphericalToCartesian(sunDirection);
	//sunObj.transform.LookAt(sunDirection2);
	ComputeAttenuation(thetaS);
	//sunLight.intensity = _sunIntensity;
	//sunLight.shadowStrength = _shadowIntensity;
}

void TOTDController::Update()
{
	initSunThetaPhi();
	calcAtmosphere();
	VecFx16* v = &mGameController->mHemisphere->mVtx[0];
	GXRgb* c = &mGameController->mHemisphere->mColors[0];
	VecFx16 forward = { 0, 0, -FX16_ONE };
	VEC_Fx16Normalize(&forward, &forward);
	VecFx16 sun = { 0, FX16_ONE, -FX16_ONE };
	VEC_Fx16Normalize(&sun, &sun);
	fx32 distmul = FX32_ONE - VEC_Fx16DotProduct(&forward, &sun);
	for (int r = 0; r < HEMISPHERE_NR_RINGS; r++)
	{
		for (int s = 0; s < HEMISPHERE_NR_SECTORS; s++)
		{
			fx32 cosang = VEC_Fx16DotProduct(v, &sun);
			if (cosang < 0)
				cosang = 0;
			fx32 cosang2 = -VEC_Fx16DotProduct(v, &forward);
			if (cosang2 < 0)
				cosang2 = 0;
			VecFx32 color;

			//float3 vPosWorld = mul(UNITY_MATRIX_MV, Input.vertex);

			//float3 ray = vPosWorld - g_vEyePt;
			//float3 ray = ObjSpaceViewDir(Input.vertex);
			fx32 far = 50 * FX32_ONE;
			//ray = normalize(ray);
			fx32 Theta = -VEC_Fx16DotProduct(v, &LightDir);
			//float Theta = dot(ray, LightDir);
			float rayy = FX_FX16_TO_F32(v->y);
			if (rayy < 0)
				rayy = 0;
			float SR = (1.05f - pow(rayy, 0.3f)) * 2000;//(1.05f - powf(FX_FX16_TO_F32(v->y), 0.3f)) * 2000;
			float SM = FX_FX32_TO_F32(far) * 0.05f;
			Lin(FX_FX32_TO_F32(Theta), SR, SM, &color);

			color.x = FX_Mul(FX_Mul(color.x, g_vSunColor.x), SunColorIntensity);
			color.y = FX_Mul(FX_Mul(color.y, g_vSunColor.y), SunColorIntensity);
			color.z = FX_Mul(FX_Mul(color.z, g_vSunColor.z), SunColorIntensity);

			//CalcLightIn(25 * FX32_ONE, cosang, &color);
			int r = (color.x * 31) >> 12;
			r = MATH_CLAMP(r, 0, 31);
			int g = (color.y * 31) >> 12;
			g = MATH_CLAMP(g, 0, 31);
			int b = (color.z * 31) >> 12;
			b = MATH_CLAMP(b, 0, 31);
			*c++ = GX_RGB(r, g, b);
			v++;
		}
	}
	TIME += 0.05f;
	if (TIME >= 24.f)
		TIME -= 24.f;
}