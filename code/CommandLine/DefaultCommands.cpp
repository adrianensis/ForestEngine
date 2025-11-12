#include "CommandLine/DefaultCommands.hpp"
#include "CommandLine/CommandLine.hpp"
#include "Core/System/SystemsManager.hpp"

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