#pragma once

#include "Core/ObjectBase.hpp"

template <class T>
class Functor: public ObjectBase
{
	GENERATE_METADATA(Functor<T>)

public:
	virtual void execute() = 0;

protected:
	T mCallback; 
public:
	SET(Callback)
};

class FunctorVoid: public Functor<std::function<void()>>
{
	GENERATE_METADATA(FunctorVoid)
public:	
	void execute() override
	{
		ASSERT_MSG(mCallback != nullptr, "Callback is null.");

		if (mCallback)
		{
			mCallback();
		}
	}
};
