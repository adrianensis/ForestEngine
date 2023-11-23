#pragma once

#include "Core/Module.hpp"


class UIStyle: public ObjectBase
{
    GENERATE_METADATA(UIStyle)

public:
	UIStyle()
	{
	}

public:
    Vector4 mTextColor;
	Vector4 mBackgroundColor;
	Vector4 mColorPressed;
	Vector4 mColorHovered;
};

class UIStyleDefault: public UIStyle
{
    GENERATE_METADATA(UIStyleDefault)

public:
	UIStyleDefault()
	{
		mTextColor = Vector4(0.0f, 0.0f, 0.0f, 1);
		mBackgroundColor = Vector4(0.5f, 0.5f, 0.5f, 1);
		mColorPressed = Vector4(0.3f, 0.3f, 0.3f, 1);
		mColorHovered = Vector4(0.7f, 0.7f, 0.7f, 1);
	}
};

class UIStyleManager: public ObjectBase, public Singleton<UIStyleManager>
{
	GENERATE_METADATA(UIStyleManager)
	
public:
    ~UIStyleManager() override;

    void init();

	template<class T> T_EXTENDS(T, UIStyle)
	void addStyle()
	{
		mStyles.insert_or_assign(T::getClassDefinitionStatic().mId, Memory::newObject<T>());
	}

	template<class T> T_EXTENDS(T, UIStyle)
	const T& getStyle()
	{
		CHECK_MSG(mStyles.contains(T::getClassDefinitionStatic().mId), "Style not found");

		return *(static_cast<T*>(mStyles.at(T::getClassDefinitionStatic().mId)));
	}

	template<class T> T_EXTENDS(T, UIStyle)
	const T& getOrAddStyle()
	{
		if(!mStyles.contains(T::getClassDefinitionStatic().mId))
		{
			addStyle<T>();
		}
		
		return getStyle<T>();
	}

private:
	UIStyleDefault mDefaultStyle;
	std::unordered_map<ClassId, UIStyle*> mStyles;

public:
	CRGET(DefaultStyle)
};
