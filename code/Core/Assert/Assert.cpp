#include "Core/Assert/Assert.hpp"
#include <csignal>

void AssertUtils::sendAssertSignal()
{
    #if defined(SIGTRAP)
        std::raise(SIGTRAP);
    #else
        std::raise(SIGABRT);
    #endif
}
