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
    Engine::getInstance().init();

    TEntityHandler<GameObject> controller = EntityManager::getInstance().requestEntity<GameObject>();
    controller->init();

    TComponentHandler<Editor> editor = ComponentsManager::getInstance().requestComponent<Editor>();
    editor->init();
    controller->addComponent(editor);

    GET_SYSTEM(ScenesManager).setGameObjectController(controller);

    Engine::getInstance().run();
    Engine::getInstance().terminate();
    Engine::deleteInstance();

    return 0;
}
