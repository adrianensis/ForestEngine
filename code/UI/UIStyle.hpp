#pragma once

#include "Engine/Core.hpp"
#include "Core/Memory/Singleton.hpp"

class UIStyle
{
public:
	UIStyle()
	{
	}

public:
    Maths::Vector4 mTextColor;
	Maths::Vector4 mBackgroundColor;
	Maths::Vector4 mColorPressed;
	Maths::Vector4 mColorHovered;
};
REGISTER_CLASS(UIStyle)

class UIStyleDefault: public UIStyle
{
public:
	UIStyleDefault()
	{
		mTextColor = Maths::Vector4(0.0f, 0.0f, 0.0f, 1);
		mBackgroundColor = Maths::Vector4(0.5f, 0.5f, 0.5f, 1);
		mColorPressed = Maths::Vector4(0.3f, 0.3f, 0.3f, 1);
		mColorHovered = Maths::Vector4(0.7f, 0.7f, 0.7f, 1);
	}
};
REGISTER_CLASS(UIStyleDefault)

class UIStyleManager: public Core::Singleton<UIStyleManager>
{
	
	
public:
    void init();

	template<class T> T_EXTENDS(T, UIStyle)
	void addStyle()
	{   
        T newStyle;
		mStyles.insert_or_assign(Core::ClassManager::getClassMetadata<T>().mClassDefinition.getId(), newStyle);
	}

	template<class T> T_EXTENDS(T, UIStyle)
	const T& getStyle()
	{
		CHECK_MSG(mStyles.contains(Core::ClassManager::getClassMetadata<T>().mClassDefinition.getId()), "Style not found");

		return static_cast<T&>(mStyles.at(Core::ClassManager::getClassMetadata<T>().mClassDefinition.getId()));
	}

	template<class T> T_EXTENDS(T, UIStyle)
	const T& getOrAddStyle()
	{
		if(!mStyles.contains(Core::ClassManager::getClassMetadata<T>().mClassDefinition.getId()))
		{
			addStyle<T>();
		}
		
		return getStyle<T>();
	}

private:
	UIStyleDefault mDefaultStyle;
	std::unordered_map<Core::ClassId, UIStyle> mStyles;

public:
	CRGET(DefaultStyle)
};
REGISTER_CLASS(UIStyleManager);
