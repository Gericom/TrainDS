#pragma once



class ObjectData
{
public:
	struct object_data_header_t
	{
		u32 nr_object_refs;
		u32 string_table_length;
		u32 nr_sceneries;
	};

	struct object_data_object_ref_entry_t
	{
		char* string_offset;
	};

	struct object_data_scenery_entry_t
	{
		u32 object_type;
		fx32 x;
		fx32 z;
		fx32 roty;
	};

	struct object_data_t
	{
		object_data_header_t header;
		object_data_object_ref_entry_t object_ref_entries[0];
	};

	struct loaded_object_t
	{
		NNSG3dResFileHeader* modelData;
	};

	object_data_t* mFileData;
	object_data_scenery_entry_t* mSceneryEntries;
	loaded_object_t* mLoadedObjects;

	ObjectData(const char* filePath);
};