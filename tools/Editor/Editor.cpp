#include "Editor.hpp"
#include "Graphics/Module.hpp"

void Editor::init()
{
	
}

void Editor::firstUpdate()
{

}

void Editor::update()
{
	Vector3 localPosition(0,0,0);

    Matrix4 rotationMatrix;
    rotationMatrix.rotation(Vector3(rotation,rotation,rotation));

    RenderEngine::getInstance().drawLine(Line(localPosition, rotationMatrix.mulVector(Vector4(localPosition.x + 0.1f ,localPosition.y,localPosition.z, 1))), 1, false, Vector4(1,0,0,1));
	RenderEngine::getInstance().drawLine(Line(localPosition, rotationMatrix.mulVector(Vector4(localPosition.x,localPosition.y + 0.1f ,localPosition.z, 1))), 1, false, Vector4(0,1,0,1));
	RenderEngine::getInstance().drawLine(Line(localPosition, rotationMatrix.mulVector(Vector4(localPosition.x,localPosition.y,localPosition.z + 0.1f , 1))), 1, false, Vector4(0,0,1,1));

    rotation += 1.0f;
}

void Editor::terminate()
{
	
}