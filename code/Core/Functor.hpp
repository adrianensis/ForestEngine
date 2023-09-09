#pragma once

#include "Core/Object/ObjectBase.hpp"

template <class T>
class Functor: public ObjectBase
{
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
		CHECK_MSG(mCallback != nullptr, "Callback is null.");

		if (mCallback)
		{
			mCallback();
		}
	}
};
