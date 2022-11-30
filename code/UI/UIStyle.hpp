#ifndef UISTYLE_HPP
#define UISTYLE_HPP

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

private:
	UIStyleDefault mDefaultStyle;
	std::map<ClassId, UIStyle*> mStyles;

public:
	CRGET(DefaultStyle)
};

#endif