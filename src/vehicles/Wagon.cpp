#include <nitro.h>
#include <nnsys/g3d.h>
#include "core.h"
#include "util.h"
#include "inih/INIReader.h"
#include "engine/PathWorker.h"
#include "terrain/Map.h"
#include "terrain/GameController.h"
#include "terrain/managers/SeqArcSfx.h"
#include "terrain/managers/SfxManager.h"
#include "Vehicle.h"
#include "Wagon.h"

#define GEN_FILE_PATH(wagonName, fileName, dst) OS_SPrintf((dst), "/data/wagons/%s/%s", (wagonName), (fileName))

Wagon::Wagon(GameController* gameController, char* name)
	: Vehicle(gameController), mOnTrack(false), mDriving(false), mSfx(NULL)
{
	STD_StrCpy(mDirName, name);
	//read the ini file
	char path[50];
	GEN_FILE_PATH(name, "wagon.ini", path);
	INIReader iniReader(path);
	//wagon name
	const char* displayName = iniReader.Get("wagon", "name", "<no name>").c_str();
	mName = new char[STD_StrLen(name) + 1];
	STD_StrCpy(mName, name);
	//front
	mFront.x = FX_F32_TO_FX32(iniReader.GetReal("front", "x", 0));
	mFront.y = FX_F32_TO_FX32(iniReader.GetReal("front", "y", 0));
	mFront.z = FX_F32_TO_FX32(iniReader.GetReal("front", "z", 0));
	//back
	mBack.x = FX_F32_TO_FX32(iniReader.GetReal("back", "x", 0));
	mBack.y = FX_F32_TO_FX32(iniReader.GetReal("back", "y", 0));
	mBack.z = FX_F32_TO_FX32(iniReader.GetReal("back", "z", 0));

	mAcceleration = FX64C_ONE / 2400;
	mDeceleration = -8192 * 32;
	mSpeed = 0;
	mMaxSpeed = FX64C_ONE / 20;

	//load model
	GEN_FILE_PATH(name, "low.nsbmd.lh", path);
	mModelData = (NNSG3dResFileHeader*)Util_LoadLHFileToBuffer(path, NULL, false);
	NNS_G3dResDefaultSetup(mModelData);
	GEN_FILE_PATH(name, "low.nsbtx.lh", path);
	NNSG3dResFileHeader* mTextures = (NNSG3dResFileHeader*)Util_LoadLHFileToBuffer(path, NULL, true);
	NNS_G3dResDefaultSetup(mTextures);
	NNSG3dResMdl* model = NNS_G3dGetMdlByIdx(NNS_G3dGetMdlSet(mModelData), 0);
	NNS_G3dMdlSetMdlLightEnableFlagAll(model, GX_LIGHTMASK_0);
	NNS_G3dMdlSetMdlDiffAll(model, GX_RGB(8, 8, 8));
	NNS_G3dMdlSetMdlAmbAll(model, GX_RGB(15, 15, 15));
	NNS_G3dMdlSetMdlSpecAll(model, GX_RGB(10, 10, 10));
	NNS_G3dMdlSetMdlEmiAll(model, GX_RGB(0, 0, 0));
	NNS_G3dMdlSetMdlPolygonModeAll(model, GX_POLYGONMODE_TOON);
	NNS_G3dMdlSetMdlFogEnableFlagAll(model, true);
	NNSG3dResTex* tex = NNS_G3dGetTex(mTextures);
	NNS_G3dBindMdlSet(NNS_G3dGetMdlSet(mModelData), tex);
	NNS_G3dRenderObjInit(&mRenderObj, model);
	NNS_FndFreeToExpHeap(gHeapHandle, mTextures);
	NNS_G3dRenderObjSetUserPtr(&mRenderObj, this);
	NNS_G3dRenderObjSetCallBack(&mRenderObj, Wagon::StaticModifyBogeyRotation, NULL, NNS_G3D_SBC_NODEDESC, NNS_G3D_SBC_CALLBACK_TIMING_B);

	//bogeys
	mNrBogeys = iniReader.GetInteger("wagon", "bogeycount", 0);
	char bogeySectionName[8];
	for (int i = 0; i < mNrBogeys; i++)
	{
		OS_SPrintf(bogeySectionName, "bogey%d", i);
		mBogeys[i].position.x = FX_F32_TO_FX32(iniReader.GetReal(bogeySectionName, "x", 0));
		mBogeys[i].position.y = FX_F32_TO_FX32(iniReader.GetReal(bogeySectionName, "y", 0));
		mBogeys[i].position.z = FX_F32_TO_FX32(iniReader.GetReal(bogeySectionName, "z", 0));
		MI_CpuClear8(&mBogeys[i].nodeName, sizeof(mBogeys[i].nodeName));
		const char* bogeyName = iniReader.Get(bogeySectionName, "name", "").c_str();
		STD_StrCpy((char*)&mBogeys[i].nodeName, bogeyName);
		mBogeys[i].pathWorker = NULL;
		mBogeys[i].jointId = NNS_G3dGetNodeIdxByName(NNS_G3dGetNodeInfo(model), &mBogeys[i].nodeName.resName);
	}
}

void Wagon::PutOnTrack(TrackPieceEx* track, int inPoint, fx32 offset)
{
	for (int i = 0; i < mNrBogeys; i++)
	{
		//calculate distance relative to the back of the train
		fx32 dist = VEC_Distance(&mBack, &mBogeys[i].position) / 13;
		if (mBogeys[i].pathWorker)
			delete mBogeys[i].pathWorker;
		mBogeys[i].pathWorker = new PathWorker(track, inPoint, offset + dist);
	}
	mOnTrack = true;
}

void Wagon::GetPosition(VecFx32* dst)
{
	VecFx32 center = { 0, 0, 0 };
	if (mOnTrack)
	{
		VecFx32 tmp;
		for (int i = 0; i < mNrBogeys; i++)
		{
			mBogeys[i].pathWorker->GetCurrent(&tmp, NULL);
			VEC_Add(&center, &tmp, &center);
		}
		center.x /= mNrBogeys;
		center.y /= mNrBogeys;
		center.z /= mNrBogeys;
	}
	*dst = center;
}

void Wagon::GetOrientation(VecFx32* dst)
{
	dst->x = FX_CosIdx(FX_RAD_TO_IDX(mCurRot));
	dst->y = 0;
	dst->z = FX_SinIdx(FX_RAD_TO_IDX(mCurRot));
}

void Wagon::ModifyBogeyRotation(NNSG3dRS* rs)
{
	for (int i = 0; i < mNrBogeys; i++)
	{
		if (NNS_G3dRSGetCurrentNodeDescID(rs) == mBogeys[i].jointId)
		{
			NNSG3dJntAnmResult* jntResult;
			jntResult = NNS_G3dRSGetJntAnmResult(rs);

#pragma cplusplus off
			jntResult->flag &= ~NNS_G3D_JNTANM_RESULTFLAG_ROT_ZERO;
#pragma cplusplus reset
			VecFx32 dir = mBogeys[i].direction;
			dir.y = 0;
			VEC_Normalize(&dir, &dir);

			fx16 bogeyrot = FX_Atan2(dir.z, dir.x);

			fx16 actrot = (bogeyrot - mCurRot) + FX_DEG_TO_RAD(90);

			MTX_RotY33(&jntResult->rot, FX_SinIdx(FX_RAD_TO_IDX(actrot)), FX_CosIdx(FX_RAD_TO_IDX(actrot)));
			return;
		}
	}
}

void Wagon::Render()
{
	if (!mOnTrack)
		return;
	NNS_G3dGePushMtx();
	{
		//calculate centerpos
		VecFx32 center;
		GetPosition(&center);
		center.y += (FX32_ONE / 32);//offset of the track from the terrain, to put the wheels on and not through the track
		NNS_G3dGeTranslateVec(&center);

		/*VecFx32 dir = { 0, 0, 0 };
		VecFx32 tmp;
		for (int i = 0; i < mNrBogeys; i++)
		{
			mBogeys[i].pathWorker->GetCurrent(NULL, &tmp);
			mBogeys[i].direction = tmp;
			VEC_Add(&dir, &tmp, &dir);
		}
		dir.x /= mNrBogeys;
		dir.y /= mNrBogeys;
		dir.z /= mNrBogeys;
		dir.y = 0;
		VEC_Normalize(&dir, &dir);

		fx16 trainrot = FX_Atan2(dir.z, dir.x);
		mCurRot = trainrot;*/

		//just why...
		MtxFx33 rotmtx;
		MTX_RotY33(&rotmtx, FX_SinIdx(FX_RAD_TO_IDX(-mCurRot)), FX_CosIdx(FX_RAD_TO_IDX(-mCurRot)));
		NNS_G3dGeMultMtx33(&rotmtx);
		MTX_RotY33(&rotmtx, FX32_SIN90, FX32_COS90);
		NNS_G3dGeMultMtx33(&rotmtx);

		//calculate rotation matrix
		/*VecFx32 up = { 0, FX32_ONE, 0 };

		dir.y = -dir.y;

		VecFx32 xaxis;
		VEC_CrossProduct(&up, &dir, &xaxis);
		VEC_Normalize(&xaxis, &xaxis);

		VecFx32 yaxis;
		VEC_CrossProduct(&dir, &xaxis, &yaxis);
		VEC_Normalize(&yaxis, &yaxis);

		MtxFx33 rot2;

		rot2._00 = xaxis.x;
		rot2._01 = yaxis.x;
		rot2._02 = dir.x;

		rot2._10 = xaxis.y;
		rot2._11 = yaxis.y;
		rot2._12 = dir.y;

		rot2._20 = xaxis.z;
		rot2._21 = yaxis.z;
		rot2._22 = dir.z;

		MTX_Inverse33(&rot2, &rot2);

		NNS_G3dGeMultMtx33(&rot2);*/

		NNS_G3dGeScale(FX32_ONE / 13, FX32_ONE / 13, FX32_ONE / 13);

		/*if (mPickingState == PICKING_STATE_RENDERING)
		{
			NNS_G3dMdlSetMdlDiffAll(mTrain.firstPart->renderObj.resMdl, GX_RGB(0, 0, 0));
			NNS_G3dMdlSetMdlAmbAll(mTrain.firstPart->renderObj.resMdl, GX_RGB(0, 0, 0));
			NNS_G3dMdlSetMdlSpecAll(mTrain.firstPart->renderObj.resMdl, GX_RGB(0, 0, 0));
			NNS_G3dMdlSetMdlEmiAll(mTrain.firstPart->renderObj.resMdl, PICKING_COLOR(PICKING_TYPE_TRAIN, 0 + 1));
		}
		else
		{
			if (mSelectedTrain == 0) NNS_G3dMdlSetMdlDiffAll(mTrain.firstPart->renderObj.resMdl, sDiffSelectionColorTable[1]);
			else NNS_G3dMdlSetMdlDiffAll(mTrain.firstPart->renderObj.resMdl, GX_RGB(21, 21, 21));
			if (mSelectedTrain == 0) NNS_G3dMdlSetMdlAmbAll(mTrain.firstPart->renderObj.resMdl, sAmbSelectionColorTable[1]);
			else NNS_G3dMdlSetMdlAmbAll(mTrain.firstPart->renderObj.resMdl, GX_RGB(15, 15, 15));
			NNS_G3dMdlSetMdlSpecAll(mTrain.firstPart->renderObj.resMdl, GX_RGB(0, 0, 0));
			NNS_G3dMdlSetMdlEmiAll(mTrain.firstPart->renderObj.resMdl, GX_RGB(0, 0, 0));
			if (mSelectedTrain == 0) NNS_G3dMdlSetMdlPolygonIDAll(mTrain.firstPart->renderObj.resMdl, 8);
			else NNS_G3dMdlSetMdlPolygonIDAll(mTrain.firstPart->renderObj.resMdl, 0);
		}*/
		//NNS_G3dMdlSetMdlDiffAll(mRenderObj.resMdl, GX_RGB(21, 21, 21));
		//NNS_G3dMdlSetMdlAmbAll(mRenderObj.resMdl, GX_RGB(15, 15, 15));
		//NNS_G3dMdlSetMdlSpecAll(mRenderObj.resMdl, GX_RGB(0, 0, 0));
		//NNS_G3dMdlSetMdlEmiAll(mRenderObj.resMdl, GX_RGB(0, 0, 0));
		NNS_G3dMdlSetMdlPolygonIDAll(mRenderObj.resMdl, 0);
		NNS_G3dDraw(&mRenderObj);
	}
	NNS_G3dGePopMtx(1);
	NNS_G3dGeFlushBuffer();
}

void Wagon::Update()
{
	if (!mOnTrack)
		return;
	if (mDriving)
		mSpeed += mAcceleration;
	else
		mSpeed += mDeceleration;
	if (mSpeed > mMaxSpeed)
		mSpeed = mMaxSpeed;
	if (mSpeed < 0)
		mSpeed = 0;
	if (mSpeed > 0)
	{
		if (!mSfx)
		{
			mSfx = new SeqArcSfx(SEQ_TRAIN, TRAIN_TRACK);
			mSfx->mParent = this;
			mGameController->mSfxManager->StartSfx(mSfx);
		}
		for (int i = 0; i < mNrBogeys; i++)
		{
			mBogeys[i].pathWorker->Proceed(mSpeed >> 20, NULL, NULL);
		}
	}
	else
	{
		if (mSfx)
		{
			mGameController->mSfxManager->StopSfx(mSfx, 8);
			mSfx = NULL;
		}
	}
	VecFx32 dir = { 0, 0, 0 };
	VecFx32 tmp;
	for (int i = 0; i < mNrBogeys; i++)
	{
		mBogeys[i].pathWorker->GetCurrent(NULL, &tmp);
		mBogeys[i].direction = tmp;
		VEC_Add(&dir, &tmp, &dir);
	}
	dir.x /= mNrBogeys;
	dir.y /= mNrBogeys;
	dir.z /= mNrBogeys;
	dir.y = 0;
	VEC_Normalize(&dir, &dir);

	fx16 trainrot = FX_Atan2(dir.z, dir.x);
	mCurRot = trainrot;
}