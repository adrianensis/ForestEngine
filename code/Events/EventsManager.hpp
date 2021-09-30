#pragma once

#include "Core/Core.hpp"
#include "Events/Event.hpp"
#include "Core/Assert/Assert.hpp"
#include "Core/Singleton.hpp"


/*
  Macros for (un)susbscribing and sending events.
*/

#define SUBSCRIBE_TO_EVENT(EventClassName, owner, receiver, eventCallback) EventsManager::getInstance()->subscribe<EventClassName>(owner, receiver, eventCallback);
#define UNSUBSCRIBE_TO_EVENT(EventClassName, owner, receiver) EventsManager::getInstance()->unsubscribe<EventClassName>(owner, receiver);
#define SEND_EVENT(owner, instigator, event) EventsManager::getInstance()->send(owner, instigator, &event);

CLASS(EventsManager, ObjectBase), SINGLETON(EventsManager)
{
private:
	using ReceiversFunctorMap = SMap(ObjectBase *, EventFunctor<Event>);
	using EventReceiversMap = SMap(ClassId, ReceiversFunctorMap);
	using OwnersMap = SMap(ObjectBase *, EventReceiversMap);

	PRI(OwnersMap, OwnersMap, NONE)

	bool ownerExists(ObjectBase * eventOwner) CNS;
	bool ownerHasEventType(ObjectBase * eventOwner, ClassId eventClassId) CNS;
	bool eventTypeHasReceiver(ObjectBase * eventOwner, ClassId eventClassId, ObjectBase * eventReceiver) CNS;
	void insertEventCallback(ClassId eventClassId, ObjectBase * eventOwner, ObjectBase * eventReceiver, EventCallback eventCallback);
	void removeEventCallback(ClassId eventClassId, ObjectBase * eventOwner, ObjectBase * eventReceiver);

	ReceiversFunctorMap &getReceiversFunctorMap(ObjectBase * eventOwner, ClassId eventClassId);

	void removeMapContent();

	void subscribe(ClassId eventClassId, ObjectBase * eventOwner, ObjectBase * eventReceiver, EventCallback eventCallback);
	void unsubscribe(ClassId eventClassId, ObjectBase * eventOwner, ObjectBase * eventReceiver);

public:
	void init();
	void terminate();

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

	void send(ObjectBase * eventOwner, ObjectBase * eventInstigator, Event * event);
};