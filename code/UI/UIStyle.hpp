#pragma once

#include "Core/Minimal.hpp"
#include "Core/Memory/Singleton.hpp"
#include "Core/Object/ObjectBase.hpp"

class UIStyle
{
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
REGISTER_CLASS(UIStyle)

class UIStyleDefault: public UIStyle
{
public:
	UIStyleDefault()
	{
		mTextColor = Vector4(0.0f, 0.0f, 0.0f, 1);
		mBackgroundColor = Vector4(0.5f, 0.5f, 0.5f, 1);
		mColorPressed = Vector4(0.3f, 0.3f, 0.3f, 1);
		mColorHovered = Vector4(0.7f, 0.7f, 0.7f, 1);
	}
};
REGISTER_CLASS(UIStyleDefault)

class UIStyleManager: public ObjectBase, public Singleton<UIStyleManager>
{
	
	
public:
    void init();

	template<class T> T_EXTENDS(T, UIStyle)
	void addStyle()
	{   
        T newStyle;
		mStyles.insert_or_assign(ClassManager::getClassMetadata<T>().mClassDefinition.getId(), newStyle);
	}

	template<class T> T_EXTENDS(T, UIStyle)
	const T& getStyle()
	{
		CHECK_MSG(mStyles.contains(ClassManager::getClassMetadata<T>().mClassDefinition.getId()), "Style not found");

		return static_cast<T&>(mStyles.at(ClassManager::getClassMetadata<T>().mClassDefinition.getId()));
	}

	template<class T> T_EXTENDS(T, UIStyle)
	const T& getOrAddStyle()
	{
		if(!mStyles.contains(ClassManager::getClassMetadata<T>().mClassDefinition.getId()))
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
REGISTER_CLASS(UIStyleManager);
