#pragma once

#include "Core/Memory.hpp"
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
			mInstance = SharedPtr<T>::newObject();
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
	inline static SharedPtr<T> mInstance;
};
