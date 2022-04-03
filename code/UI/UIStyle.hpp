#pragma once

#include "Core/Module.hpp"

#ifdef CPP_INCLUDE
#include "UI/UIStyle.hpp"
#endif

class UIStyle: public ObjectBase
{
    GENERATE_METADATA(UIStyle)

public:
    Vector4 mTextColor;
	Vector4 mBackgroundColor;
	Vector4 mColorPressed;
	Vector4 mColorHovered;

	UIStyle()
	{
        mTextColor = Vector4(1, 1, 1, 1);
		mBackgroundColor = Vector4(0, 0, 0, 1);
		mColorPressed = Vector4(0.3f, 0.3f, 0.3f, 1);
		mColorHovered = Vector4(0.7f, 0.7f, 0.7f, 1);
	}

	COPY(UIStyle)
	{
        DO_COPY(mTextColor)
		DO_COPY(mBackgroundColor)
		DO_COPY(mColorPressed)
		DO_COPY(mColorHovered)
	}
};

class UIStyleManager: public ObjectBase, public Singleton<UIStyleManager>
{
	GENERATE_METADATA(UIStyleManager)
	
private:
	UIStyle mDefaultStyle;
	std::map<ClassId, UIStyle*> mStyles;

public:
	CPP ~UIStyleManager() override
	{
		MAP_DELETE_CONTENT(mStyles)
	}

	CPP void init()
	{
		mDefaultStyle.mTextColor = Vector4(0.0f, 0.0f, 0.0f, 1);
		mDefaultStyle.mBackgroundColor = Vector4(0.5f, 0.5f, 0.5f, 1);
		mDefaultStyle.mColorPressed = Vector4(0.3f, 0.3f, 0.3f, 1);
		mDefaultStyle.mColorHovered = Vector4(0.7f, 0.7f, 0.7f, 1);
	}

	template<class T, typename = std::enable_if_t<std::is_base_of<UIStyle, T>::value> >
	void addStyle()
	{
		MAP_INSERT(mStyles, T::getClassIdStatic(), NEW(T));
	}

	template<class T, typename = std::enable_if_t<std::is_base_of<UIStyle, T>::value> >
	const T& getStyle()
	{
		ASSERT_MSG(MAP_CONTAINS(mStyles, T::getClassIdStatic()), "Style not found");

		return *(static_cast<T*>(mStyles.at(T::getClassIdStatic())));
	}

	template<class T, typename = std::enable_if_t<std::is_base_of<UIStyle, T>::value> >
	const T& getOrAddStyle()
	{
		if(!MAP_CONTAINS(mStyles, T::getClassIdStatic()))
		{
			addStyle<T>();
		}
		
		return getStyle<T>();
	}

	CRGET(DefaultStyle)
};