#ifndef FUNCTOR_HPP
#define FUNCTOR_HPP

#include "Core/ObjectBase.hpp"

template <class T>
class Functor: public ObjectBase
{
	GENERATE_METADATA(Functor<T>)

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

#endif