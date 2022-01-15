#pragma once

#include "Core/Module.hpp"
class EditorController;

class EditorCommands
{
PUB
    static void registerEditorCommands(EditorController* editorController);
};