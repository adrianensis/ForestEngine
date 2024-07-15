#include "Core/Events/EventsManager.hpp"


void EventsManager::init()
{
}

void EventsManager::terminate()
{
	removeMapContent();
}

void EventsManager::send(ClassId eventClassId, ObjectBase *eventOwner, ObjectBase *eventInstigator, Event *event)
{
	if (ownerExists(eventOwner))
	{
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
	return mOwnersMap.contains(eventOwner);
}

bool EventsManager::ownerHasEventType(ObjectBase *eventOwner, ClassId eventClassId) const
{
	return mOwnersMap.at(eventOwner).contains(eventClassId);
}

bool EventsManager::eventTypeHasReceiver(ObjectBase *eventOwner, ClassId eventClassId, ObjectBase *eventReceiver) const
{
	return mOwnersMap.at(eventOwner).at(eventClassId).contains(eventReceiver);
}

void EventsManager::insertEventCallback(ClassId eventClassId, ObjectBase *eventOwner, ObjectBase *eventReceiver, EventCallback eventCallback)
{
	EventFunctor<Event> eventFunctor;
	eventFunctor.mCallback = eventCallback;
	eventFunctor.mEventClassId = eventClassId;
	eventFunctor.mEventReceiver = eventReceiver;

	mOwnersMap.at(eventOwner).at(eventClassId).insert_or_assign(eventReceiver, eventFunctor);
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
		mOwnersMap.insert_or_assign(eventOwner, EventReceiversMap());
	}

	if (!ownerHasEventType(eventOwner, eventClassId))
	{
		mOwnersMap.at(eventOwner).insert_or_assign(eventClassId, ReceiversFunctorMap());
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
