#include "Scene/GameObject.hpp"
#include "Core/ECS/Component.hpp"
#include "Scene/Transform.hpp"
#include "Core/Events/EventsManager.hpp"

GameObject::GameObject()
{
	mIsActive = true;
	mShouldPersist = false;
}

void GameObject::init()
{
    mTransform = createComponent<Transform>();
	mTag = "";
}

void GameObject::addComponentInternal(ComponentHandler componentHandler)
{
    CHECK_MSG(componentHandler.isValid(), "Invalid Component!");
    CHECK_MSG(!componentHandler.getComponent().mGameObject.isValid(), "Component is already assigned to a GameObject!");

	mComponentHandlers.emplace_back(componentHandler);
    Component& comp = componentHandler.getComponent();

    CHECK_MSG(getPtrToThis<GameObject>().isValid(), "invalid GameObject!");
	comp.mGameObject = getPtrToThis<GameObject>();
	comp.onComponentAdded();

	SystemsManager::getInstance().addComponentToSystem(componentHandler);
}

// Ptr<Component> GameObject::addComponentInternal(OwnerPtr<Component>&& component)
// {
//     CHECK_MSG(!component->mGameObject.isValid(), "Component is already assigned to a GameObject!");

// 	Ptr<Component> comp = mComponents.emplace_back(std::move(component));

// 	comp->mGameObject = getPtrToThis<GameObject>();
// 	comp->onComponentAdded();

// 	SystemsManager::getInstance().addComponentToSystem(comp);

//     return comp;
// }

void GameObject::removeComponentInternal(ComponentHandler componentHandler)
{
    CHECK_MSG(componentHandler.isValid(), "Invalid Component!");
    CHECK_MSG(componentHandler.getComponent().mGameObject.isValid(), "Component is not assigned to a GameObject!");
    CHECK_MSG(componentHandler.getComponent().mGameObject == getPtrToThis<GameObject>(), "Component is assigned to another GameObject!");

    FOR_LIST(it, mComponentHandlers)
	{
        if((*it) == componentHandler)
        {
            SystemsManager::getInstance().removeComponentFromSystem(componentHandler);
            componentHandler->destroy();
            // mComponentHandlers.erase(it);
            GET_SYSTEM(ComponentsManager).removeComponent(*it);
            break;
        }
    }
}
// void GameObject::removeComponentInternal(Ptr<Component> component)
// {
//     CHECK_MSG(component->mGameObject.isValid(), "Component is not assigned to a GameObject!");
//     CHECK_MSG(component->mGameObject == getPtrToThis<GameObject>(), "Component is assigned to another GameObject!");

//     auto it = std::find(mComponents.begin(), mComponents.end(), component);
//     if (it != mComponents.end())
//     {
//         SystemsManager::getInstance().removeComponentFromSystem(component);
// 	    component->destroy();
//         mComponents.erase(it);
//     }
// }
void GameObject::setIsActive(bool isActive)
{
	mIsActive = mIsDestroyed || mIsPendingToBeDestroyed ? false : isActive;

	FOR_LIST(it, mComponentHandlers)
	// FOR_LIST(it, mComponents)
	{
		(*it).getComponent().setIsActive(isActive);
	}
}

void GameObject::destroy()
{
	mIsPendingToBeDestroyed = true;
	mIsActive = false;

	EventOnDestroy event;
	SEND_EVENT(this, this, event);

	onDestroy();

	FOR_LIST(it, mComponentHandlers)
	{
        if((*it).isValid())
        {
            SystemsManager::getInstance().removeComponentFromSystem(*it);
            (*it).getComponent().destroy();
            GET_SYSTEM(ComponentsManager).removeComponent(*it);
        }
	}

	mComponentHandlers.clear();
    // FOR_LIST(it, mComponents)
	// {
    //     if(*it)
    //     {
    //         SystemsManager::getInstance().removeComponentFromSystem(Ptr<Component>(*it));
    //         (*it)->destroy();
    //     }
	// }

	// mComponents.clear();
}

IMPLEMENT_SERIALIZATION(GameObject)
{

	SERIALIZE("is_static", mIsStatic)
	SERIALIZE("should_persist", mShouldPersist)

	SERIALIZE("tag", mTag)

	SERIALIZE("transform", mTransform.get())

}

IMPLEMENT_DESERIALIZATION(GameObject)
{
	DESERIALIZE("is_static", mIsStatic)
	DESERIALIZE("should_persist", mShouldPersist)

	DESERIALIZE("tag", mTag)

	DESERIALIZE("transform", mTransform.get())


}