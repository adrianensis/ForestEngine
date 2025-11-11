#include "Core/HashedString/HashedString.hpp"
#include "Engine/Engine.hpp"
#include "Scene/Module.hpp"
#include "Scripting/Module.hpp"
#include "Engine/EntityComponent/EntityComponentManager.hpp"

#include "Editor.hpp"

/*#include <ft2build.h>
#include FT_FREETYPE_H */

int main()
{
    Engine engine;
    engine.init();

    EC::EntityPtr<GameObject> controller = ECManager.requestEntity<GameObject>();
    controller->init();

    EC::ComponentPtr<Editor> editor = ECManager.requestComponent<Editor>();
    editor->init();
    ECManager.addComponent(controller, editor);

    GET_SYSTEM(ScenesManager).setGameObjectController(controller);

    engine.run();
    engine.terminate();

    return 0;
}
