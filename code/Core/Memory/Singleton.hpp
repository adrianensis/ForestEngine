#pragma once

#include "Core/CoreBase.hpp"
#include "Core/Memory/Pointers.hpp"

NS_BEGIN(Core)
template <class T>
class Singleton
{
public:
	Singleton() = default;
	~Singleton() = default;

	static WeakPtr<T> getInstancePtr()
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

NS_END