#pragma once

#include "Core/Pointers.hpp"

template <class T>
class Singleton
{
public:
	Singleton() = default;
	~Singleton() = default;

	static Ptr<T> getInstancePtr()
	{
		if (!mInstance.isValid())
		{
			mInstance = OwnerPtr<T>::newObject();
		}

		return mInstance;
	}

	static T& getInstance()
	{
		return getInstancePtr().get();
	}

	static void deleteInstance()
	{
		if (mInstance)
		{
			mInstance.invalidate();
		}
	}
	
private:
	inline static OwnerPtr<T> mInstance;
};
