#pragma once

#include "Core/Events/Event.hpp"
#include "Engine/EngineSystem.hpp"

/*
  Macros for (un)susbscribing and sending events.
*/

#define SUBSCRIBE_TO_EVENT(EventClassName, owner, receiver, eventCallback) GET_SYSTEM(EventsManager).subscribe<EventClassName>(owner, receiver, eventCallback);
#define UNSUBSCRIBE_TO_EVENT(EventClassName, owner, receiver) GET_SYSTEM(EventsManager).unsubscribe<EventClassName>(owner, receiver);
#define SEND_EVENT(owner, instigator, event) GET_SYSTEM(EventsManager).send(owner, instigator, &event);

class EventsManager: public EngineSystem
{
	GENERATE_METADATA(EventsManager)

public:
    void init();

    void terminate();

	template <class E> T_EXTENDS(E, Event)
	void subscribe(ObjectBase * eventOwner, ObjectBase * eventReceiver, EventCallback eventCallback)
	{
        subscribe(E::getClassIdStatic(), eventOwner, eventReceiver, eventCallback);
	}

	template <class E> T_EXTENDS(E, Event)
	void unsubscribe(ObjectBase * eventOwner, ObjectBase * eventReceiver)
	{
        unsubscribe(E::getClassIdStatic(), eventOwner, eventReceiver);
	}

    void send(ObjectBase *eventOwner, ObjectBase *eventInstigator, Event *event);

private:
	using ReceiversFunctorMap = std::unordered_map<ObjectBase *, EventFunctor<Event>>;
	using EventReceiversMap = std::unordered_map<ClassId, ReceiversFunctorMap>;
	using OwnersMap = std::unordered_map<ObjectBase *, EventReceiversMap>;

	OwnersMap mOwnersMap;

    void removeMapContent();
    bool ownerExists(ObjectBase *eventOwner) const;
    bool ownerHasEventType(ObjectBase *eventOwner, ClassId eventClassId) const;
    bool eventTypeHasReceiver(ObjectBase *eventOwner, ClassId eventClassId, ObjectBase *eventReceiver) const;
    void insertEventCallback(ClassId eventClassId, ObjectBase *eventOwner, ObjectBase *eventReceiver, EventCallback eventCallback);
    void removeEventCallback(ClassId eventClassId, ObjectBase *eventOwner, ObjectBase *eventReceiver);
    EventsManager::ReceiversFunctorMap& getReceiversFunctorMap(ObjectBase *eventOwner, ClassId eventClassId);
    void subscribe(ClassId eventClassId, ObjectBase *eventOwner, ObjectBase *eventReceiver, EventCallback eventCallback);
    void unsubscribe(ClassId eventClassId, ObjectBase *eventOwner, ObjectBase *eventReceiver);
};
