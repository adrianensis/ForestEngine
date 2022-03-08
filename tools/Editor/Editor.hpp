#pragma once

#include "Scripting/Script.hpp"

class Editor: public Script
{
    GENERATE_METADATA(Editor)

private:
	f32 rotation = 0;

	void createSprite(const Vector3& v);
public:
	void init() override;
	void firstUpdate() override;
	void update() override;
	void terminate() override;

	void drawGrid() const;
};