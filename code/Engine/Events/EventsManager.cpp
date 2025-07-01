#include "Engine/Events/EventsManager.hpp"

NS_BEGIN(Event)

void EventsManager::init()
{
}

void EventsManager::terminate()
{
	removeMapContent();
}

void EventsManager::send(Core::ClassId eventClassId, IEventObject *eventOwner, IEventObject *eventInstigator, Event *event)
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

bool EventsManager::ownerExists(IEventObject *eventOwner) const
{
	return mOwnersMap.contains(eventOwner);
}

bool EventsManager::ownerHasEventType(IEventObject *eventOwner, Core::ClassId eventClassId) const
{
	return mOwnersMap.at(eventOwner).contains(eventClassId);
}

bool EventsManager::eventTypeHasReceiver(IEventObject *eventOwner, Core::ClassId eventClassId, IEventObject *eventReceiver) const
{
	return mOwnersMap.at(eventOwner).at(eventClassId).contains(eventReceiver);
}

void EventsManager::insertEventCallback(Core::ClassId eventClassId, IEventObject *eventOwner, IEventObject *eventReceiver, EventCallback eventCallback)
{
	EventFunctor<Event> eventFunctor;
	eventFunctor.mCallback = eventCallback;
	eventFunctor.mEventClassId = eventClassId;
	eventFunctor.mEventReceiver = eventReceiver;

	mOwnersMap.at(eventOwner).at(eventClassId).insert_or_assign(eventReceiver, eventFunctor);
}

void EventsManager::removeEventCallback(Core::ClassId eventClassId, IEventObject *eventOwner, IEventObject *eventReceiver)
{
	mOwnersMap.at(eventOwner).at(eventClassId).erase(eventReceiver);
}

EventsManager::ReceiversFunctorMap& EventsManager::getReceiversFunctorMap(IEventObject *eventOwner, Core::ClassId eventClassId)
{
	return mOwnersMap.at(eventOwner).at(eventClassId);
}

void EventsManager::subscribe(Core::ClassId eventClassId, IEventObject *eventOwner, IEventObject *eventReceiver, EventCallback eventCallback)
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

void EventsManager::unsubscribe(Core::ClassId eventClassId, IEventObject *eventOwner, IEventObject *eventReceiver)
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

NS_END