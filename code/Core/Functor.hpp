#pragma once

#include <functional>
#include "Core/Assert/Assert.hpp"

template <class T>
class Functor
{
public:
	virtual void execute() = 0;

public:
	T mCallback;
};

class FunctorVoid: public Functor<std::function<void()>>
{
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
