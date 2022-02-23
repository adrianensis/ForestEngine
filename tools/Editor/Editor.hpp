#pragma once

#include "Scripting/Script.hpp"

class Editor: public Script
{
    GENERATE_METADATA(Editor)

public:
	void init() override;
	void firstUpdate() override;
	void update() override;
	void terminate() override;

	void drawGrid() const;
};