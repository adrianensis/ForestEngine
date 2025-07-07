#pragma once

#include "Core/CoreBase.hpp"
#include "Core/Memory/Memory.hpp"

NS_BEGIN(Core)
template <class T>
class Singleton
{
public:
	Singleton() = default;
	~Singleton()
	{
		deleteInstance();
	};

	static T* getInstancePtr()
	{
		if (!mInstance)
		{
			mInstance = new T();
			Memory::registerPointer<T>(mInstance);
		}

		return mInstance;
	}

	static T& getInstance()
	{
		return *getInstancePtr();
	}

	static void deleteInstance()
	{
		if (mInstance)
		{
			Memory::unregisterPointer(mInstance);
			T* ptr = mInstance;
			mInstance = nullptr;
			delete ptr;
		}
	}
	
private:
	inline static T* mInstance = nullptr;
};

NS_END