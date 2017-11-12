#pragma once

#include <map>
#include <string>

struct NNSG2dFont;

class FontManager
{
private:
	std::map<std::string, const NNSG2dFont*> mFontMap;

public:
	bool RegisterFont(const char* name, const NNSG2dFont* font)
	{
		if (mFontMap.count(name))
			return false;
		mFontMap[name] = font;
		return true;
	}

	void UnregisterFont(const char* name)
	{
		mFontMap.erase(name);
	}

	const NNSG2dFont* GetFontByName(const char* name)
	{
		return mFontMap[name];
	}
};
