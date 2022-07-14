#include "Core/Command/DefaultCommands.hpp"
#include "Core/Command/CommandLine.hpp"

#include "Core/Log/Log.hpp"

void DefaultCommands::registerDefaultCommands()
{
R(CommandLine) cmd = CommandLine::getInstance();

cmd.registerCommand("echo", [](CR(Command) command)
{
    ECHO(command.getArgumentsString());
});
}