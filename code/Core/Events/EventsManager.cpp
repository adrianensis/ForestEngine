#include "Core/Events/EventsManager.hpp"


void EventsManager::init()
{
}

void EventsManager::terminate()
{
	removeMapContent();
}

void EventsManager::send(ObjectBase *eventOwner, ObjectBase *eventInstigator, Event *event)
{
	if (ownerExists(eventOwner))
	{
		ClassId eventClassId = event->getClassId();
		if (ownerHasEventType(eventOwner, eventClassId))
		{
			ReceiversFunctorMap receiversFunctorMapCopy = getReceiversFunctorMap(eventOwner, eventClassId);

			FOR_MAP(it, receiversFunctorMapCopy)
			{
				EventFunctor<Event> functor = it->second;
				functor.mEvent = event;
				functor.mEvent->mInstigator = eventInstigator;
				functor.execute();
			}
		}
	}
}

void EventsManager::removeMapContent()
{
	mOwnersMap.clear();
}

bool EventsManager::ownerExists(ObjectBase *eventOwner) const
{
	return MAP_CONTAINS(mOwnersMap, eventOwner);
}

bool EventsManager::ownerHasEventType(ObjectBase *eventOwner, ClassId eventClassId) const
{
	return MAP_CONTAINS(mOwnersMap.at(eventOwner), eventClassId);
}

bool EventsManager::eventTypeHasReceiver(ObjectBase *eventOwner, ClassId eventClassId, ObjectBase *eventReceiver) const
{
	return MAP_CONTAINS(mOwnersMap.at(eventOwner).at(eventClassId), eventReceiver);
}

void EventsManager::insertEventCallback(ClassId eventClassId, ObjectBase *eventOwner, ObjectBase *eventReceiver, EventCallback eventCallback)
{
	EventFunctor<Event> eventFunctor;
	eventFunctor.setCallback(eventCallback);
	eventFunctor.mEventClassId = eventClassId;
	eventFunctor.mEventReceiver = eventReceiver;

	MAP_INSERT(mOwnersMap.at(eventOwner).at(eventClassId), eventReceiver, eventFunctor);
}

void EventsManager::removeEventCallback(ClassId eventClassId, ObjectBase *eventOwner, ObjectBase *eventReceiver)
{
	mOwnersMap.at(eventOwner).at(eventClassId).erase(eventReceiver);
}

EventsManager::ReceiversFunctorMap& EventsManager::getReceiversFunctorMap(ObjectBase *eventOwner, ClassId eventClassId)
{
	return mOwnersMap.at(eventOwner).at(eventClassId);
}

void EventsManager::subscribe(ClassId eventClassId, ObjectBase *eventOwner, ObjectBase *eventReceiver, EventCallback eventCallback)
{
	if (!ownerExists(eventOwner))
	{
		MAP_INSERT(mOwnersMap, eventOwner, EventReceiversMap())
	}

	if (!ownerHasEventType(eventOwner, eventClassId))
	{
		MAP_INSERT(mOwnersMap.at(eventOwner), eventClassId, ReceiversFunctorMap())
	}

	insertEventCallback(eventClassId, eventOwner, eventReceiver, eventCallback);
}

void EventsManager::unsubscribe(ClassId eventClassId, ObjectBase *eventOwner, ObjectBase *eventReceiver)
{
	if (ownerExists(eventOwner))
	{
		if (ownerHasEventType(eventOwner, eventClassId))
		{
			if (eventTypeHasReceiver(eventOwner, eventClassId, eventReceiver))
			{
				removeEventCallback(eventClassId, eventOwner, eventReceiver);
			}
		}
	}
}
