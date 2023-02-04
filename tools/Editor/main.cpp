#include "Core/Engine.hpp"
#include "Scene/Module.hpp"
#include "Scripting/Module.hpp"

#include "Editor.hpp"

#include "tools/Editor/generated.hpp"

/*#include <ft2build.h>
#include FT_FREETYPE_H */

ENGINE_MAIN()
{
GameObject *controller = Memory::newObject<GameObject>();
controller->init();

// NOTE: this script won't be added to the ScriptEngine in the usual way,
// since ScriptEngine itself is still not initialized.
Script *script = Memory::newObject<Editor>();
script->init();
controller->addComponent<Script>(script);

ScenesManager::getInstance().setGameObjectController(controller);
}
