#ifndef SINGLETON_HPP
#define SINGLETON_HPP

#include "Core/Memory.hpp"

template <class T>
class Singleton
{
public:
	Singleton() = default;
	~Singleton() = default;

	static bool existsInstance()
	{
		return mInstance != nullptr;
	}

	static T& getInstance()
	{
		if (!existsInstance())
		{
			mInstance = Memory::newObject<T>();
		}

		return *mInstance;
	}

	static void deleteInstance()
	{
		if (mInstance)
		{
			Memory::deleteObject<T>(mInstance);
			mInstance = nullptr;
		}
	}
	
private:
	inline static T *mInstance = nullptr;
};

#endif