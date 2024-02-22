#pragma once

#include "Core/Module.hpp"
#include "Core/Object/Singleton.hpp"

class UIStyle
{
    GENERATE_METADATA_STRUCT(UIStyle)
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
    GENERATE_METADATA_STRUCT(UIStyleDefault)
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
    void init();

	template<class T> T_EXTENDS(T, UIStyle)
	void addStyle()
	{   
        T newStyle;
		mStyles.insert_or_assign(T::getClassDefinitionStatic().mId, newStyle);
	}

	template<class T> T_EXTENDS(T, UIStyle)
	const T& getStyle()
	{
		CHECK_MSG(mStyles.contains(T::getClassDefinitionStatic().mId), "Style not found");

		return static_cast<T&>(mStyles.at(T::getClassDefinitionStatic().mId));
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
	std::unordered_map<ClassId, UIStyle> mStyles;

public:
	CRGET(DefaultStyle)
};
