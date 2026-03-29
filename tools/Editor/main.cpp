#include "Core/HashedString/HashedString.hpp"
#include "Engine/Engine.hpp"
#include "Scene/Module.hpp"
#include "Scene/ScenesManager.hpp"
#include "Scripting/Module.hpp"
#include "Core/EntityComponent/EntityComponentManager.hpp"

#include "Editor.hpp"

/*#include <ft2build.h>
#include FT_FREETYPE_H */

int main()
{
    Engine engine;
    engine.init();

    GameObject* controller = engine.getScenesManager()->getScene(ScenesManager::smGlobalSceneName)->createGameObject<GameObject>();

    Editor* editor = ECManager.requestComponent<Editor>(controller, [&](auto* component)
    {
        component->init();
    });

    editor->setUIManager(engine.getUIManager());

    engine.run();
    engine.terminate();

    return 0;
}
