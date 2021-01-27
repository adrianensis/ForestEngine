#pragma once

#include "Scripting/Script.hpp"
#include "Maths/Vector2.hpp"
#include "Element.hpp"

namespace DE {

class Renderer;

class TornadoScript: public Script {

private:
	Renderer* mRenderer;
	Element mElement;

	f32 mDurationTime;
	f32 mDurationTimeCounter;

public:

	DE_CLASS(TornadoScript)

	void init() override;
	void firstStep() override;
	void step() override;
	void terminate() override;

	void onEnterCollision(GameObject *otherGameObject) override;
	void onCollision(GameObject *otherGameObject) override;
	void onExitCollision(GameObject *otherGameObject) override;

};

}
