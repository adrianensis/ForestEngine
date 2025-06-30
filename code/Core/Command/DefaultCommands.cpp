#include "Core/Command/DefaultCommands.hpp"
#include "Core/Command/CommandLine.hpp"
#include "Core/System/SystemManager.hpp"

#include "Core/Log/Log.hpp"

NS_BEGIN(Core)

void DefaultCommands::registerDefaultCommands()
{
    CommandLine& cmd = GET_SYSTEM(CommandLine);

    cmd.registerCommand("echo", [](const Command& command)
    {
        LOG(command.getArgumentsString());
    });
}
NS_END