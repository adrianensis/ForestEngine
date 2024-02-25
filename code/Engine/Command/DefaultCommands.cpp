#include "Engine/Command/DefaultCommands.hpp"
#include "Engine/Command/CommandLine.hpp"

#include "Core/Log/Log.hpp"

void DefaultCommands::registerDefaultCommands()
{
    CommandLine& cmd = GET_SYSTEM(CommandLine);

    cmd.registerCommand("echo", [](const Command& command)
    {
        LOG(command.getArgumentsString());
    });
}