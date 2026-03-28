#include "CommandLine/DefaultCommands.hpp"
#include "CommandLine/CommandLine.hpp"

NS_BEGIN(Command)

void DefaultCommands::registerDefaultCommands(CommandLine& cmd)
{
    cmd.registerCommand("echo", [cmd](const Command& command)
    {
        cmd.writeLine(command.getArgumentsString());
    });
}
NS_END