#include "Core/Engine.hpp"
#include "Scene/Module.hpp"
#include "Scripting/Module.hpp"

#include "Editor.hpp"

/*#include <ft2build.h>
#include FT_FREETYPE_H */

int main()
{
    Engine::getInstance().init();

    OwnerPtr<GameObject> controller = OwnerPtr<GameObject>(Memory::newObject<GameObject>());
    controller.get().init();

    // NOTE: this script won't be added to the ScriptEngine in the usual way,
    // since ScriptEngine itself is still not initialized.
    OwnerPtr<Editor> script = OwnerPtr<Editor>(Memory::newObject<Editor>());
    script.get().init();
    controller.get().addComponent<Script>(OwnerPtr<Script>::cast(script));

    ScenesManager::getInstance().setGameObjectController(controller);

    Engine::getInstance().run();
    Engine::getInstance().terminate();
    Engine::deleteInstance();

    return 0;
}
