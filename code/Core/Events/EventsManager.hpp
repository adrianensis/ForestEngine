#pragma once

#include "Core/BasicTypes.hpp"
#include "Core/ObjectBase.hpp"
#include "Core/Events/Event.hpp"
#include "Core/Assert/Assert.hpp"
#include "Core/Singleton.hpp"

#ifdef CPP_INCLUDE
#include "Core/Events/EventsManager.hpp"
#endif

/*
  Macros for (un)susbscribing and sending events.
*/

#define SUBSCRIBE_TO_EVENT(EventClassName, owner, receiver, eventCallback) EventsManager::getInstance().subscribe<EventClassName>(owner, receiver, eventCallback);
#define UNSUBSCRIBE_TO_EVENT(EventClassName, owner, receiver) EventsManager::getInstance().unsubscribe<EventClassName>(owner, receiver);
#define SEND_EVENT(owner, instigator, event) EventsManager::getInstance().send(owner, instigator, &event);

class EventsManager: public ObjectBase, public Singleton<EventsManager>
{
	GENERATE_METADATA(EventsManager)
	
private:
	using ReceiversFunctorMap = std::map<ObjectBase *, EventFunctor<Event>>;
	using EventReceiversMap = std::map<ClassId, ReceiversFunctorMap>;
	using OwnersMap = std::map<ObjectBase *, EventReceiversMap>;

	OwnersMap mOwnersMap;

	CPP void removeMapContent()
	{
		mOwnersMap.clear();
	}

	CPP bool ownerExists(ObjectBase *eventOwner) const
	{
		return MAP_CONTAINS(mOwnersMap, eventOwner);
	}

	CPP bool ownerHasEventType(ObjectBase *eventOwner, ClassId eventClassId) const
	{
		return MAP_CONTAINS(mOwnersMap.at(eventOwner), eventClassId);
	}

	CPP bool eventTypeHasReceiver(ObjectBase *eventOwner, ClassId eventClassId, ObjectBase *eventReceiver) const
	{
		return MAP_CONTAINS(mOwnersMap.at(eventOwner).at(eventClassId), eventReceiver);
	}

	CPP void insertEventCallback(ClassId eventClassId, ObjectBase *eventOwner, ObjectBase *eventReceiver, EventCallback eventCallback)
	{
		EventFunctor<Event> eventFunctor;
		eventFunctor.setCallback(eventCallback);
		eventFunctor.mEventClassId = eventClassId;
		eventFunctor.mEventReceiver = eventReceiver;

		MAP_INSERT(mOwnersMap.at(eventOwner).at(eventClassId), eventReceiver, eventFunctor);
	}

	CPP void removeEventCallback(ClassId eventClassId, ObjectBase *eventOwner, ObjectBase *eventReceiver)
	{
		mOwnersMap.at(eventOwner).at(eventClassId).erase(eventReceiver);
	}

	CPP EventsManager::ReceiversFunctorMap& getReceiversFunctorMap(ObjectBase *eventOwner, ClassId eventClassId)
	{
		return mOwnersMap.at(eventOwner).at(eventClassId);
	}

	CPP void subscribe(ClassId eventClassId, ObjectBase *eventOwner, ObjectBase *eventReceiver, EventCallback eventCallback)
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

	CPP void unsubscribe(ClassId eventClassId, ObjectBase *eventOwner, ObjectBase *eventReceiver)
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



public:
	CPP void init()
	{
	}

	CPP void terminate()
	{
		removeMapContent();
	}

	template <class E>
	void subscribe(ObjectBase * eventOwner, ObjectBase * eventReceiver, EventCallback eventCallback)
	{
		if (std::is_base_of<Event, E>::value)
		{
			subscribe(E::getClassIdStatic(), eventOwner, eventReceiver, eventCallback);
		}
		else
		{
			ASSERT_MSG(false, "The event class must inherit from Event.");
		}
	}

	template <class E>
	void unsubscribe(ObjectBase * eventOwner, ObjectBase * eventReceiver)
	{
		if (std::is_base_of<Event, E>::value)
		{
			unsubscribe(E::getClassIdStatic(), eventOwner, eventReceiver);
		}
		else
		{
			ASSERT_MSG(false, "The event class must inherit from Event.");
		}
	}

	CPP void send(ObjectBase *eventOwner, ObjectBase *eventInstigator, Event *event)
	{
		if (ownerExists(eventOwner))
		{
			ClassId eventClassId = event->getClassId();
			if (ownerHasEventType(eventOwner, eventClassId))
			{
				// Duplicate functors map. New event-receivers can subscribe during the iteration.
				// So we don't want to iterate a mutable map.
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
};