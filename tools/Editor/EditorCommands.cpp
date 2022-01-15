#include "EditorCommands.hpp"
#include "EditorController.hpp"

void EditorCommands::registerEditorCommands(EditorController* editorController)
{
    CommandLine& cmd = CommandLine::getInstance();

    cmd.registerCommand("sprite", [&](const Command& command)
    {
        if(command.argumentExists("-n"))
        {
            //editorController->getSprites()
        }
    });
}