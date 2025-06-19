#include "Engine/Engine.hpp"
#include "Scene/Module.hpp"
#include "Scripting/Module.hpp"
#include "Core/EntityComponent/EntityComponentManager.hpp"

#include "Editor.hpp"

/*#include <ft2build.h>
#include FT_FREETYPE_H */

int main()
{
    Engine engine;
    engine.init();

    TEntityPtr<SceneObject> controller = EntityComponentManager::getInstance().requestEntity<SceneObject>();
    controller->init();

    TComponentPtr<Editor> editor = EntityComponentManager::getInstance().requestComponent<Editor>();
    editor->init();
    controller->addComponent(editor);

    GET_SYSTEM(ScenesManager).setSceneObjectController(controller);

    engine.run();
    engine.terminate();
    return 0;
}
