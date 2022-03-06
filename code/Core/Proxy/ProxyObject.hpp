#pragma once

// CPP_IGNORE

#include "Core/BasicTypes.hpp"
#include "Core/ObjectBase.hpp"

template <class T, typename = std::enable_if_t<std::is_base_of<ObjectBase, T>::value> >
class ProxyObject
{
protected:
	T* mObject = nullptr;

private: 
	virtual void onDestroy() { mObject = nullptr; }

public:
	void init(T* object)
	{
		mObject = object;

		mObject->setProxyCallback([this]()
		{
			onDestroy();
		});
	}

	void disconnectFromOwner()
	{
		if(mObject)
		{
			mObject->setProxyCallback([](){});
		}
	}

    bool isValid() const { return mObject != nullptr; }
	T* getObject() const { return mObject; }
};