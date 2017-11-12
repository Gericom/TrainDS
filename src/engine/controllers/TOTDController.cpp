#include "common.h"

#include <math.h>
#include <float.h>
#include <algorithm>
#include "../Hemisphere.h"
#include "terrain/GameController.h"
#include "TOTDController.h"

void TOTDController::BetaR(float Theta, vec3* dst) 
{
	float t2 = 3.f + 0.5f * Theta * Theta;
	dst->x = BetaRayTheta.x * t2;
	dst->y = BetaRayTheta.y * t2;
	dst->z = BetaRayTheta.z * t2;
}

void TOTDController::BetaM(float Theta, vec3* dst)
{
	float g = FX_FX32_TO_F32(mDirectionalityFactor);
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

	float fRayleighFactor = mRayFactor * (powf(3.141592653589793f, 2.0f) * powf(n * n - 1.0f, 2.0f) * (6 + 3 * pn)) / (N * (6 - 7 * pn));

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

	float c = (0.6544f * mTurbidity - 0.6510f) * 1e-16f;	//Concentration factor

	float fMieFactor = mMieFactor * 0.434f * c * 4.0f * 3.141592653589793f * 3.141592653589793f;

	BetaMieTheta.x = fMieFactor / (2.0f * powf(650e-9f, 2.0f));
	BetaMieTheta.y = fMieFactor / (2.0f * powf(570e-9f, 2.0f));
	BetaMieTheta.z = fMieFactor / (2.0f * powf(475e-9f, 2.0f));

	vBetaMie.x = K[0] * fMieFactor / powf(650e-9f, 2.0f);
	vBetaMie.y = K[1] * fMieFactor / powf(570e-9f, 2.0f);
	vBetaMie.z = K[2] * fMieFactor / powf(475e-9f, 2.0f);
}

void TOTDController::ComputeAttenuation(float m_fTheta)
{
	float fBeta = 0.04608365822050f * mTurbidity - 0.04586025928522f;
	float fTauR, fTauA;
	float fTau[3];
	//float tmp = 93.885f - (m_fTheta / Mathf.PI * 180.0f);

	float m;

	/*if(!(mCurTime > 5 && mCurTime < 18))
	{
	m = (float)(1.0f / (Mathf.Cos (m_fTheta) + 0.15f * tmp));
	}
	else
	{*/
	// Relative Optical Mass
	if (mCurTime > 5.82 && mCurTime < 18.1)
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

	VEC_Set(&mSunColor, FX_F32_TO_FX32(fTau[0]), FX_F32_TO_FX32(fTau[1]), FX_F32_TO_FX32(fTau[2]));
	int r = (mSunColor.x * 31) >> 12;
	r = MATH_CLAMP(r, 0, 31);
	int g = (mSunColor.y * 31) >> 12;
	g = MATH_CLAMP(g, 0, 31);
	int b = (mSunColor.z * 31) >> 12;
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
	solarTime = mCurTime + (0.170f* sinf(4.f*3.141592653589793f*(mCurJulianDate - 80.f) / 373.f) - 0.129f*sinf(2.f*3.141592653589793f*(mCurJulianDate - 8.f) / 355.f)) + (mStdMeridian - mLongitudeRad) / 15.0f;
	solarDeclination = (0.4093f*sinf(2.f*3.141592653589793f*(mCurJulianDate - 81.f) / 368.f));
	solarAltitude = asinf(sinf(mLatitudeRad) * sinf(solarDeclination) -
		cosf(mLatitudeRad) * cosf(solarDeclination) * cosf(3.141592653589793f*solarTime / 12.f));

	opp = -cosf(solarDeclination) * sinf(3.141592653589793f*solarTime / 12.f);
	adj = -(cosf(mLatitudeRad) * sinf(solarDeclination) +
		sinf(mLatitudeRad) * cosf(solarDeclination) *  cosf(3.141592653589793f*solarTime / 12.f));
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
	mLightDir.x = ldir.x;
	mLightDir.y = ldir.y;
	mLightDir.z = ldir.z;

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
	OSIntrMode old = OS_DisableInterrupts();
	mNewTime += 0.01f;
	if (mNewTime >= 24.f)
	{
		mNewTime -= 24.f;
		mNewJulianDate++;
	}
	OS_RestoreInterrupts(old);
}

#ifdef ENABLE_NEW_TOTD
bool TOTDController::solveQuadratic(float a, float b, float c, float& x1, float& x2)
{
	if (b == 0) {
		// Handle special case where the the two vector ray.dir and V are perpendicular
		// with V = ray.orig - sphere.centre
		if (a == 0) return false;
		x1 = 0; x2 = std::sqrtf(-c / a);
		return true;
	}
	float discr = b * b - 4 * a * c;

	if (discr < 0) return false;

	float q = (b < 0.f) ? -0.5f * (b - std::sqrtf(discr)) : -0.5f * (b + std::sqrtf(discr));
	x1 = q / a;
	x2 = c / q;

	return true;
}

bool TOTDController::raySphereIntersect(const vec3& orig, const vec3& dir, const float& radius, float& t0, float& t1)
{
	// They ray dir is normalized so A = 1 
	float A = dir.x * dir.x + dir.y * dir.y + dir.z * dir.z;
	float B = 2 * (dir.x * orig.x + dir.y * orig.y + dir.z * orig.z);
	float C = orig.x * orig.x + orig.y * orig.y + orig.z * orig.z - radius * radius;

	if (!solveQuadratic(A, B, C, t0, t1)) return false;

	if (t0 > t1) std::swap(t0, t1);

	return true;
}

#define PI 3.141592653589793f

const TOTDController::vec3 TOTDController::betaR = { 3.8e-6f, 13.5e-6f, 33.1e-6f };
const TOTDController::vec3 TOTDController::betaM = { 21e-6f, 21e-6f, 21e-6f };

bool TOTDController::computeIncidentLight(const vec3& orig, const vec3& dir, float tmin, float tmax, vec3 &result)
{
	float t0, t1;
	if (!raySphereIntersect(orig, dir, atmosphereRadius, t0, t1) || t1 < 0) return false;
	if (t0 > tmin && t0 > 0) tmin = t0;
	if (t1 < tmax) tmax = t1;
	uint32_t numSamples = 16;
	uint32_t numSamplesLight = 8;
	float segmentLength = (tmax - tmin) / numSamples;
	float tCurrent = tmin;
	vec3 sumR = { 0, 0, 0 }, sumM = { 0, 0, 0 }; // mie and rayleigh contribution 
	float opticalDepthR = 0, opticalDepthM = 0;
	vec3 sunDirection = 
	{
		-FX_FX16_TO_F32(mLightDir.x),
		-FX_FX16_TO_F32(mLightDir.y),
		-FX_FX16_TO_F32(mLightDir.z)
	};
	float mu = dir.x * sunDirection.x + dir.y * sunDirection.y + dir.z * sunDirection.z;// dot(dir, sunDirection); // mu in the paper which is the cosine of the angle between the sun direction and the ray direction 
	float phaseR = 3.f / (16.f * PI) * (1 + mu * mu);
	float g = 0.76f;
	float phaseM = 3.f / (8.f * PI) * ((1.f - g * g) * (1.f + mu * mu)) / ((2.f + g * g) * pow(1.f + g * g - 2.f * g * mu, 1.5f));
	for (uint32_t i = 0; i < numSamples; ++i) {
		vec3 samplePosition = 
		{
			orig.x + (tCurrent + segmentLength * 0.5f) * dir.x,
			orig.y + (tCurrent + segmentLength * 0.5f) * dir.y,
			orig.z + (tCurrent + segmentLength * 0.5f) * dir.z
		};
		float height = /*samplePosition.length()*/sqrt(samplePosition.x * samplePosition.x + samplePosition.y * samplePosition.y + samplePosition.z * samplePosition.z) - earthRadius;
		// compute optical depth for light
		float hr = exp(-height / Hr) * segmentLength;
		float hm = exp(-height / Hm) * segmentLength;
		opticalDepthR += hr;
		opticalDepthM += hm;
		// light optical depth
		float t0Light, t1Light;
		raySphereIntersect(samplePosition, sunDirection, atmosphereRadius, t0Light, t1Light);
		float segmentLengthLight = t1Light / numSamplesLight, tCurrentLight = 0;
		float opticalDepthLightR = 0, opticalDepthLightM = 0;
		uint32_t j;
		for (j = 0; j < numSamplesLight; ++j) {
			vec3 samplePositionLight = 
			{ 
				samplePosition.x + (tCurrentLight + segmentLengthLight * 0.5f) * sunDirection.x,
				samplePosition.y + (tCurrentLight + segmentLengthLight * 0.5f) * sunDirection.y,
				samplePosition.z + (tCurrentLight + segmentLengthLight * 0.5f) * sunDirection.z
			};
			float heightLight = sqrt(samplePositionLight.x * samplePositionLight.x + samplePositionLight.y * samplePositionLight.y + samplePositionLight.z * samplePositionLight.z) - earthRadius;
			if (heightLight < 0) break;
			opticalDepthLightR += exp(-heightLight / Hr) * segmentLengthLight;
			opticalDepthLightM += exp(-heightLight / Hm) * segmentLengthLight;
			tCurrentLight += segmentLengthLight;
		}
		if (j == numSamplesLight) {
			vec3 tau = 
			{
				betaR.x * (opticalDepthR + opticalDepthLightR) + betaM.x * 1.1f * (opticalDepthM + opticalDepthLightM),
				betaR.y * (opticalDepthR + opticalDepthLightR) + betaM.y * 1.1f * (opticalDepthM + opticalDepthLightM),
				betaR.z * (opticalDepthR + opticalDepthLightR) + betaM.z * 1.1f * (opticalDepthM + opticalDepthLightM)
			};
			vec3 attenuation = { exp(-tau.x), exp(-tau.y), exp(-tau.z) };
			sumR.x += attenuation.x * hr;
			sumR.y += attenuation.y * hr;
			sumR.z += attenuation.z * hr;

			sumM.x += attenuation.x * hm;
			sumM.y += attenuation.y * hm;
			sumM.z += attenuation.z * hm;

			//sumR += attenuation * hr;
			//sumM += attenuation * hm;
		}
		tCurrent += segmentLength;
	}

	// We use a magic number here for the intensity of the sun (20). We will make it more
	// scientific in a future revision of this lesson/code
	result.x = (sumR.x * betaR.x * phaseR + sumM.x * betaM.x * phaseM) * 20;
	result.y = (sumR.y * betaR.y * phaseR + sumM.y * betaM.y * phaseM) * 20;
	result.z = (sumR.z * betaR.z * phaseR + sumM.z * betaM.z * phaseM) * 20;
	result.x = 1.0 - exp(-1.0 * result.x);
	result.y = 1.0 - exp(-1.0 * result.y);
	result.z = 1.0 - exp(-1.0 * result.z);
	return true;
	//return (sumR * betaR * phaseR + sumM * betaM * phaseM) * 20;
}
#endif

void TOTDController::InternalUpdate()
{
	OSIntrMode old = OS_DisableInterrupts();
	mCurJulianDate = mNewJulianDate;
	mCurTime = mNewTime;
	OS_RestoreInterrupts(old);
	initSunThetaPhi();
	calcAtmosphere();
	VecFx16* v = &mGameController->mHemisphere->mVtx[0];
	GXRgb* c = mGameController->mHemisphere->mColors[!mGameController->mHemisphere->mColorBuf];
	int meanr = 0, meang = 0, meanb = 0;
	for (int r = 0; r < HEMISPHERE_NR_RINGS; r++)
	{
		for (int s = 0; s < HEMISPHERE_NR_SECTORS; s++)
		{
#ifndef ENABLE_NEW_TOTD
			VecFx32 color;
			fx32 far = 50 * FX32_ONE;
			fx32 Theta = -VEC_Fx16DotProduct(v, &mLightDir);
			float rayy = FX_FX16_TO_F32(v->y);
			if (rayy < 0)
				rayy = 0;
			float SR = (1.05f - pow(rayy, 0.3f)) * 2000;
			float SM = FX_FX32_TO_F32(far) * 0.05f;
			Lin(FX_FX32_TO_F32(Theta), SR, SM, &color);

			color.x = FX_Mul(FX_Mul(color.x, mSunColor.x), mSunColorIntensity);
			color.y = FX_Mul(FX_Mul(color.y, mSunColor.y), mSunColorIntensity);
			color.z = FX_Mul(FX_Mul(color.z, mSunColor.z), mSunColorIntensity);
#else
			vec3 origin = { 0, earthRadius, 0 };
			vec3 dir = 
			{
				FX_FX16_TO_F32(v->x),
				FX_FX16_TO_F32(v->y),
				FX_FX16_TO_F32(v->z)
			};
			float tMax = INFINITY;
			//float t0, t1;
			//if (raySphereIntersect(origin, dir, earthRadius, t0, t1) && t1 > 0)
			//	tMax = std::max(0.f, t0);
			vec3 result;
			computeIncidentLight(origin, dir, 0, tMax, result);
			VecFx32 color =
			{
				FX_F32_TO_FX32(MATH_CLAMP(result.x, 0, 1)),
				FX_F32_TO_FX32(MATH_CLAMP(result.y, 0, 1)),
				FX_F32_TO_FX32(MATH_CLAMP(result.z, 0, 1))
			};
#endif
			int r2 = (color.x * 31) >> 12;
			r2 = MATH_CLAMP(r2, 0, 31);
			int g = (color.y * 31) >> 12;
			g = MATH_CLAMP(g, 0, 31);
			int b = (color.z * 31) >> 12;
			b = MATH_CLAMP(b, 0, 31);
			meanr += r2;
			meang += g;
			meanb += b;
			*c++ = GX_RGB(r2, g, b);
			v++;
		}
	}
	int count = HEMISPHERE_NR_RINGS * HEMISPHERE_NR_SECTORS;
	meanr /= count;
	meang /= count;
	meanb /= count;
	mGameController->mAverageSkyColor = GX_RGB(meanr, meang, meanb);
	mGameController->mHemisphere->mColorBuf = !mGameController->mHemisphere->mColorBuf;
}

void TOTDController::TOTDThreadMain()
{
	while (1)
	{
		InternalUpdate();
	}
}