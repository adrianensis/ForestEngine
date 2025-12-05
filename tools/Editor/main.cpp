#include "Core/HashedString/HashedString.hpp"
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

    GameObject* controller = ECManager.requestEntity<GameObject>([&](GameObject* entity)
    {
        entity->init(); 
    });

    ECManager.requestComponent<Editor>(controller, [&](auto* component)
    {
        component->init();
    });

    GET_SYSTEM(ScenesManager).setGameObjectController(controller);

    engine.run();
    engine.terminate();

    return 0;
}
