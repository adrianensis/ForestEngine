#include "Engine/Command/DefaultCommands.hpp"
#include "Engine/Command/CommandLine.hpp"
#include "Engine/System/SystemManager.hpp"

NS_BEGIN(Command)

void DefaultCommands::registerDefaultCommands()
{
    CommandLine& cmd = GET_SYSTEM(CommandLine);
    cmd.registerCommand("echo", [](const Command& command)
    {
        GET_SYSTEM(CommandLine).writeLine(command.getArgumentsString());
    });
}
NS_END