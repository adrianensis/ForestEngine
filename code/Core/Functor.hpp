#pragma once

// CPP_IGNORE

#include "Core/BasicTypes.hpp"
#include "Core/ObjectBase.hpp"

template <class T>
class Functor: public ObjectBase
{
	GENERATE_METADATA(Functor<T>)
protected:
	T mCallback; 

public:
	virtual void execute() = 0;

	// NOTE : Override in children if needed!
	COPY(Functor<T>)
	{
		DO_COPY(mCallback)
	}

	/*bool operator== (const Functor& functor) const{
	   return this == &functor;
	}*/

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
