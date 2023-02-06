#pragma once

#include "Core/Memory.hpp"
#include "Core/Pointers.hpp"

template <class T>
class Singleton
{
public:
	Singleton() = default;
	~Singleton() = default;

	static T& getInstance()
	{
		if (!mInstance.isValid())
		{
			mInstance = OwnerPtr<T>::newObject();
		}

		return mInstance.get();
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
