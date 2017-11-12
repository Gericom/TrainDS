#include "common.h"

#include "util.h"
#include "ObjectData.h"

#define GEN_FILE_PATH(objectName, extension, dst) OS_SPrintf((dst), "/data/map/scenery/%s.%s", (objectName), (extension))

ObjectData::ObjectData(const char* filePath)
{
	mFileData = (object_data_t*)Util_LoadLZ77FileToBuffer(filePath, NULL, false);
	mLoadedObjects = new loaded_object_t[mFileData->header.nr_object_refs];
	char* stringTable = (char*)&mFileData->object_ref_entries[mFileData->header.nr_object_refs];
	for (int i = 0; i < mFileData->header.nr_object_refs; i++)
	{
		char* name = stringTable + (u32)mFileData->object_ref_entries[i].string_offset;
		mFileData->object_ref_entries[i].string_offset = name;
		//load model
		char path[50];
		GEN_FILE_PATH(name, "nsbmd.lz", path);
		NNSG3dResFileHeader* modelData = (NNSG3dResFileHeader*)Util_LoadLZ77FileToBuffer(path, NULL, false);
		mLoadedObjects[i].modelData = modelData;
		NNS_G3dResDefaultSetup(mLoadedObjects[i].modelData);
		GEN_FILE_PATH(name, "nsbtx.lz", path);
		NNSG3dResFileHeader* mTextures = (NNSG3dResFileHeader*)Util_LoadLZ77FileToBuffer(path, NULL, true);
		NNS_G3dResDefaultSetup(mTextures);
		NNSG3dResMdl* model = NNS_G3dGetMdlByIdx(NNS_G3dGetMdlSet(modelData), 0);
		//NNS_G3dMdlSetMdlLightEnableFlagAll(model, GX_LIGHTMASK_0);
		NNS_G3dMdlSetMdlDiffAll(model, GX_RGB(31, 31, 31));
		NNS_G3dMdlSetMdlAmbAll(model, GX_RGB(20, 20, 20));
		NNS_G3dMdlSetMdlSpecAll(model, GX_RGB(0, 0, 0));
		NNS_G3dMdlSetMdlEmiAll(model, GX_RGB(0, 0, 0));
		//NNS_G3dMdlSetMdlPolygonModeAll(model, GX_POLYGONMODE_TOON);
		NNS_G3dMdlSetMdlFarClipAll(model, true);
		NNS_G3dMdlSetMdlFogEnableFlagAll(model, true);
		NNSG3dResTex* tex = NNS_G3dGetTex(mTextures);
		NNS_G3dBindMdlSet(NNS_G3dGetMdlSet(modelData), tex);
		//NNS_G3dRenderObjInit(&mRenderObj, model);
		NNS_FndFreeToExpHeap(gHeapHandle, mTextures);
	}
	mSceneryEntries = (object_data_scenery_entry_t*)((u32)&mFileData->object_ref_entries[mFileData->header.nr_object_refs] + mFileData->header.string_table_length);
}