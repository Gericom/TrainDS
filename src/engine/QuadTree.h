#pragma once

#include <vector>
#include "engine/objects/WorldObject.h"

class QuadTree
{
private:
	struct quadtree_node_t
	{
		box2d_t bounds;
		union
		{
			quadtree_node_t* subnodes;//4
			std::vector<WorldObject*>* objects;
		};
	};

	box2d_t mBounds;
	int mDepth;
	quadtree_node_t* mRootNode;
	std::vector<WorldObject*>* mWorldObjectLists;
	u32 mRenderFlag;

	void SetupNode(quadtree_node_t* node, int &nodeIdx, int &leafNodeIdx, int depth, box2d_t* nodeBounds);
	void Insert(quadtree_node_t* node, int depth, WorldObject* object);
	void Render(quadtree_node_t* node, int depth, box2d_t* frustum);

public:
	QuadTree(fx32 x, fx32 z, fx32 width, fx32 height, int depth);
	~QuadTree()
	{
		delete[] mWorldObjectLists;
		delete mRootNode;
	}

	void Insert(WorldObject* object)
	{
		Insert(mRootNode, 0, object);
	}

	void Render(box2d_t* frustum)
	{
		mRenderFlag++;
		Render(mRootNode, 0, frustum);
	}
};