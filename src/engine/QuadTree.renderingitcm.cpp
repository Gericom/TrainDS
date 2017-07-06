#include <nitro.h>
#include "core.h"
#include "util.h"
#include "QuadTree.h"

QuadTree::QuadTree(fx32 x, fx32 z, fx32 width, fx32 height, int depth)
	: mDepth(depth), mRenderFlag(0)
{
	mBounds.x1 = x;
	mBounds.y1 = z;
	mBounds.x2 = x + width;
	mBounds.y2 = z + height;
	mRootNode = new quadtree_node_t[(4 * (1 << mDepth) * (1 << mDepth) - 1) / 3];
	int nodeIdx = 1;
	SetupNode(mRootNode, nodeIdx, 0, &mBounds);
}

void QuadTree::SetupNode(quadtree_node_t* node, int &nodeIdx, int depth, box2d_t* nodeBounds)
{
	node->bounds = *nodeBounds;
	if (depth == mDepth)
		node->objects = new std::vector<WorldObject*>();
	else
	{
		node->subnodes = &mRootNode[nodeIdx];
		nodeIdx += 4;
		box2d_t bound = *nodeBounds;
		int newwidth = bound.GetWidth() / 2;
		int newheight = bound.GetHeight() / 2;
		bound.x2 -= newwidth;
		bound.y2 -= newheight;
		SetupNode(node->subnodes, nodeIdx, depth + 1, &bound);
		bound.x1 += newwidth;
		bound.x2 += newwidth;
		SetupNode(node->subnodes + 1, nodeIdx, depth + 1, &bound);
		bound.y1 += newheight;
		bound.y2 += newheight;
		SetupNode(node->subnodes + 3, nodeIdx, depth + 1, &bound);
		bound.x1 -= newwidth;
		bound.x2 -= newwidth;
		SetupNode(node->subnodes + 2, nodeIdx, depth + 1, &bound);
		//SetupNode(node->subnodes, nodeIdx, depth + 1);
		//SetupNode(node->subnodes + 1, nodeIdx, depth + 1);
		//SetupNode(node->subnodes + 2, nodeIdx, depth + 1);
		//SetupNode(node->subnodes + 3, nodeIdx, depth + 1);
	}
}

void QuadTree::Insert(quadtree_node_t* node, int depth, WorldObject* object)
{
	if (!object->Intersects(&node->bounds))
		return;
	if (depth == mDepth)
	{
		node->objects->push_back(object);
	}
	else
	{
		Insert(node->subnodes, depth + 1, object);
		Insert(node->subnodes + 1, depth + 1, object);
		Insert(node->subnodes + 2, depth + 1, object);
		Insert(node->subnodes + 3, depth + 1, object);
	}
}

void QuadTree::Render(quadtree_node_t* node, int depth, box2d_t* frustum)
{
	if (!((node->bounds.x1 <= frustum->x2) && (node->bounds.y1 <= frustum->y2) &&
		(node->bounds.x2 >= frustum->x1) && (node->bounds.y2 >= frustum->y1)))
		//!node->bounds.Intersects(frustum))
		return;
	if (depth == mDepth)
	{
		std::vector<WorldObject*>::iterator end = node->objects->end();
		for (std::vector<WorldObject*>::iterator it = node->objects->begin(); it != end; ++it)
		{
			WorldObject* obj = *it;
			if (obj->mRenderFlag != mRenderFlag && obj->Intersects(frustum))
			{
				obj->Render();
				obj->mRenderFlag = mRenderFlag;
			}
		}
	}
	else
	{
		Render(node->subnodes, depth + 1, frustum);
		Render(node->subnodes + 1, depth + 1, frustum);
		Render(node->subnodes + 2, depth + 1, frustum);
		Render(node->subnodes + 3, depth + 1, frustum);
	}
}