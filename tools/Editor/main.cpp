#include "Engine/Engine.hpp"
#include "Scene/Module.hpp"
#include "Scripting/Module.hpp"
#include "Core/EntityComponent/ComponentsManager.hpp"
#include "Core/EntityComponent/EntityManager.hpp"

#include "Editor.hpp"

/*#include <ft2build.h>
#include FT_FREETYPE_H */

int main()
{
    Engine engine;
    engine.init();

    TEntityPtr<SceneObject> controller = EntityManager::getInstance().requestEntity<SceneObject>();
    controller->init();

    TComponentPtr<Editor> editor = ComponentsManager::getInstance().requestComponent<Editor>();
    editor->init();
    controller->addComponent(editor);

    GET_SYSTEM(ScenesManager).setSceneObjectController(controller);

    engine.run();
    engine.terminate();
    return 0;
}
