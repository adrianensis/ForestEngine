#pragma once

#include "Core/System/System.hpp"
#include "Core/Events/Event.hpp"

/*
  Macros for (un)susbscribing and sending events.
*/

#define SUBSCRIBE_TO_EVENT(EventClassName, owner, receiver, eventCallback) GET_SYSTEM(EventsManager).subscribe<EventClassName>(owner, receiver, eventCallback);
#define UNSUBSCRIBE_TO_EVENT(EventClassName, owner, receiver) GET_SYSTEM(EventsManager).unsubscribe<EventClassName>(owner, receiver);
#define SEND_EVENT(owner, instigator, event) GET_SYSTEM(EventsManager).send<REMOVE_REFERENCE(decltype(event))>(owner, instigator, &event);

class EventsManager: public System
{
public:
    virtual void init() override;
    virtual void terminate() override;

	template <class E> T_EXTENDS(E, Event)
	void subscribe(ObjectBase * eventOwner, ObjectBase * eventReceiver, EventCallback eventCallback)
	{
        subscribe(ClassManager::getClassMetadata<E>().mClassDefinition.getId(), eventOwner, eventReceiver, eventCallback);
	}

	template <class E> T_EXTENDS(E, Event)
	void unsubscribe(ObjectBase * eventOwner, ObjectBase * eventReceiver)
	{
        unsubscribe(ClassManager::getClassMetadata<E>().mClassDefinition.getId(), eventOwner, eventReceiver);
	}

	template <class E> T_EXTENDS(E, Event)
    void send(ObjectBase *eventOwner, ObjectBase *eventInstigator, Event *event)
    {
        ClassId eventClassId = ClassManager::getClassMetadata<E>().mClassDefinition.getId();
        send(eventClassId, eventOwner, eventInstigator, event);
    }

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
    void send(ClassId eventClassId, ObjectBase *eventOwner, ObjectBase *eventInstigator, Event *event);
};
REGISTER_CLASS(EventsManager);
