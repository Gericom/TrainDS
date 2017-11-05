#pragma once
#include "lyt_res_struct.h"
#include "behavior/Behavior.h"

class Pane
{
protected:
	NNSFndLink mLink;

	Pane* mParent;
	NNSFndList mChildrenList;
	char* mName;
	pane_flags_t mFlags;

	s16 mTranslationX;
	s16 mTranslationY;
	fx32 mRotation;
	fx16 mScaleX;
	fx16 mScaleY;
	u16 mWidth;
	u16 mHeight;

	Behavior* mBehavior;

	NNSG2dFVec2 GetBaseTranslation() const;
	MtxFx32 ApplyTransform(const MtxFx32* mtx) const;
	bool IsPointInBounds(const MtxFx32* mtx, fx32 px, fx32 py, NNSG2dFVec2* newPoint = NULL) const;

	virtual void RenderContent() { }
public:
	Pane(Pane* parent, const char* name, int x, int y, int width, int height)
		: mParent(parent), mTranslationX(x), mTranslationY(y), mRotation(0), mScaleX(FX16_ONE), mScaleY(FX16_ONE), mWidth(width), mHeight(height), mBehavior(NULL)
	{
		NNS_FND_INIT_LIST(&mChildrenList, Pane, mLink);
		mFlags.visible = true;
		mFlags.touchable = false;
		mFlags.hOrigin = PANE_H_ORIGIN_CENTER;
		mFlags.vOrigin = PANE_V_ORIGIN_CENTER;
		mFlags.hParentCenter = PANE_H_PARENT_CENTER_CENTER;
		mFlags.vParentCenter = PANE_V_PARENT_CENTER_CENTER;
		mName = new char[STD_StrLen(name) + 1];
		STD_StrCpy(mName, name);
	}

	Pane(Pane* parent, lyt_res_pan1_t* pan1Res, const char* name)
		: mParent(parent), mFlags(pan1Res->flags), mTranslationX(pan1Res->xtranslation), mTranslationY(pan1Res->ytranslation),
		mRotation(pan1Res->rotation), mScaleX(pan1Res->scalex), mScaleY(pan1Res->scaley), mWidth(pan1Res->width), mHeight(pan1Res->height), mBehavior(NULL)
	{
		NNS_FND_INIT_LIST(&mChildrenList, Pane, mLink);
		mName = new char[STD_StrLen(name) + 1];
		STD_StrCpy(mName, name);
	}

	virtual ~Pane()
	{
		delete mName;
	}

	void Render();

	void AddChild(Pane* pane)
	{
		NNS_FndAppendListObject(&mChildrenList, pane);
	}

	Pane* FindPaneByName(const char* name)
	{
		if (!STD_StrCmp(name, mName))
			return this;
		Pane* pane = NULL;
		while ((pane = (Pane*)NNS_FndGetNextListObject(&mChildrenList, pane)) != NULL)
		{
			Pane* result = pane->FindPaneByName(name);
			if (result)
				return result;
		}
		return NULL;
	}

	Pane* FindPaneByNamePrefix(const char* name)
	{
		return FindPaneByNamePrefix(name, STD_StrLen(name));
	}

private:
	Pane* FindPaneByNamePrefix(const char* name, int count)
	{
		if (!STD_StrNCmp(name, mName, count))
			return this;
		Pane* pane = NULL;
		while ((pane = (Pane*)NNS_FndGetNextListObject(&mChildrenList, pane)) != NULL)
		{
			Pane* result = pane->FindPaneByNamePrefix(name, count);
			if (result)
				return result;
		}
		return NULL;
	}
public:

	bool OnPenDown(const MtxFx32* mtx, fx32 px, fx32 py);
	bool OnPenMove(const MtxFx32* mtx, fx32 px, fx32 py);
	bool OnPenUp(const MtxFx32* mtx, fx32 px, fx32 py);

	Pane* GetParent() const { return mParent; }
	const char* GetName() const { return mName; }

	bool GetVisible() const { return mFlags.visible; }
	void SetVisible(bool visible) { mFlags.visible = visible; }

	bool GetTouchable() const { return mFlags.touchable; }
	void SetTouchable(bool touchable) { mFlags.touchable = touchable; }

	s16 GetTranslationX() const { return mTranslationX; }
	void SetTranslationX(s16 x) { mTranslationX = x; }
	s16 GetTranslationY() const { return mTranslationY; }
	void SetTranslationY(s16 y) { mTranslationY = y; }

	int GetWidth() const { return mWidth; }
	int GetHeight() const { return mHeight; }

	void SetBehavior(Behavior* behavior)
	{
		mBehavior = behavior;
		mBehavior->SetPane(this);
	}

	NNSFndList* GetChildrenList() { return &mChildrenList; }
};
