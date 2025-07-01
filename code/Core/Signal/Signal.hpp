#pragma once

#include "Core/StdCore.hpp"
#include <csignal>

NS_BEGIN(Core)
class SignalUtils
{
public:
    static void registerSignalHandler(i32 sig)
    {
        std::signal(sig, signalHandler);
    }

    static void signal(i32 sig)
    {
        std::raise(sig);
    }

    static void breakpointTrap()
    {
        #if defined(SIGTRAP)
            signal(SIGTRAP);
        #else
            signal(SIGABRT);
        #endif
    }
private:
    // Signal handler function
    static void signalHandler(i32 sig) {
        // Optionally exit the program after handling
        exit(sig);
    }
};
NS_END