#include "Core/Engine.hpp"
#include "Scene/Module.hpp"
#include "Scripting/Module.hpp"

#include "Editor.hpp"

/*#include <ft2build.h>
#include FT_FREETYPE_H */

int main()
{
    Engine::getInstance().init();

    OwnerPtr<GameObject> controller = OwnerPtr<GameObject>::newObject();
    controller->init();

    // NOTE: this script won't be added to the ScriptEngine in the usual way,
    // since ScriptEngine itself is still not initialized.
    OwnerPtr<Editor> script = OwnerPtr<Editor>::newObject();
    script->init();
    controller->addComponent<Script>(OwnerPtr<Script>::moveCast(script));

    GET_SYSTEM(ScenesManager).setMoveGameObjectController(std::move(controller));

    Engine::getInstance().run();
    Engine::getInstance().terminate();
    Engine::deleteInstance();

    return 0;
}
