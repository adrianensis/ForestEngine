#include "Engine/Engine.hpp"
#include "Scene/Module.hpp"
#include "Scripting/Module.hpp"
#include "Core/ECS/ComponentsManager.hpp"
#include "Core/ECS/EntityManager.hpp"

#include "Editor.hpp"

/*#include <ft2build.h>
#include FT_FREETYPE_H */

int main()
{
    Engine::getInstance().init();

    TEntityHandler<GameObject> controller = EntityManager::getInstance().requestEntity<GameObject>();
    controller->init();

    // NOTE: this script won't be added to the ScriptEngine in the usual way,
    // since ScriptEngine itself is still not initialized.
    TComponentHandler<Editor> editor = ComponentsManager::getInstance().requestComponent<Editor>();
    controller->addComponent(editor);

    GET_SYSTEM(ScenesManager).setGameObjectController(controller);

    Engine::getInstance().run();
    Engine::getInstance().terminate();
    Engine::deleteInstance();

    return 0;
}
