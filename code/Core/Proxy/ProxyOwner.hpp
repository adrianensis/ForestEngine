#pragma once

#include "Core/BasicTypes.hpp"

class ProxyOwner
{
public:
    using ProxyCallback = std::function<void()>;
private:
    ProxyCallback mCallback = nullptr;

public:
    void setProxyCallback(ProxyCallback callback)
    {
        mCallback = callback;
    }
    
    void notifyProxy()
    {
        if(mCallback)
        {
            mCallback();
        }
    }
};