#pragma once

#include "Core/Module.hpp"
#include "EditorUIElement.hpp"

class ToolsBar: public EditorUIElement
{
    GENERATE_METADATA(ToolsBar)
PUB
	void init(EditorController* editorController) override;
	void setVisibility(bool visible) override;
};