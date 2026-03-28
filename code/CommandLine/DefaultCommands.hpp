#pragma once

#include "Core/Core.hpp"

NS_BEGIN(Command)

class CommandLine;
class DefaultCommands
{
    public:
    static void registerDefaultCommands(CommandLine& cmd);
};

NS_END