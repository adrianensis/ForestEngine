#include "Core/Engine.hpp"
#include "Scene/ScenesManager.hpp"
#include "Scene/GameObject.hpp"
#include "Scripting/Script.hpp"

#include "Server/EditorServer.hpp"

#include "Config/ConfigObject.hpp"

#include <iostream>

#include "generated-code/EditorElectron.generated.class.hpp"

/*#include <ft2build.h>
#include FT_FREETYPE_H */

ENGINE_MAIN()
{
	GameObject *controller = NEW(GameObject);
	controller->init();

	Script *script = NEW(EditorServer);
	controller->addComponent<Script>(script);

	ScenesManager::getInstance()->setGameObjectController(controller);
}
