#pragma once

#include "Core/System/System.hpp"
#include "Core/System/SystemManager.hpp"
#include "Core/Events/Event.hpp"

NS_BEGIN(Core)
/*
  Macros for (un)susbscribing and sending events.
*/

#define SUBSCRIBE_TO_EVENT(EventClassName, owner, receiver, eventCallback) GET_SYSTEM(Core::EventsManager).subscribe<EventClassName>(owner, receiver, eventCallback);
#define UNSUBSCRIBE_TO_EVENT(EventClassName, owner, receiver) GET_SYSTEM(Core::EventsManager).unsubscribe<EventClassName>(owner, receiver);
#define SEND_EVENT(owner, instigator, event) GET_SYSTEM(Core::EventsManager).send<REMOVE_REFERENCE(decltype(event))>(owner, instigator, &event);

class EventsManager: public System
{
public:
    virtual void init() override;
    virtual void terminate() override;

	template <class E> T_EXTENDS(E, Event)
	void subscribe(IEventObject * eventOwner, IEventObject * eventReceiver, EventCallback eventCallback)
	{
        subscribe(Core::ClassManager::getClassMetadata<E>().mClassDefinition.getId(), eventOwner, eventReceiver, eventCallback);
	}

	template <class E> T_EXTENDS(E, Event)
	void unsubscribe(IEventObject * eventOwner, IEventObject * eventReceiver)
	{
        unsubscribe(Core::ClassManager::getClassMetadata<E>().mClassDefinition.getId(), eventOwner, eventReceiver);
	}

	template <class E> T_EXTENDS(E, Event)
    void send(IEventObject *eventOwner, IEventObject *eventInstigator, Event *event)
    {
        ClassId eventClassId = Core::ClassManager::getClassMetadata<E>().mClassDefinition.getId();
        send(eventClassId, eventOwner, eventInstigator, event);
    }

private:
	using ReceiversFunctorMap = std::unordered_map<IEventObject *, EventFunctor<Event>>;
	using EventReceiversMap = std::unordered_map<ClassId, ReceiversFunctorMap>;
	using OwnersMap = std::unordered_map<IEventObject *, EventReceiversMap>;

	OwnersMap mOwnersMap;

    void removeMapContent();
    bool ownerExists(IEventObject *eventOwner) const;
    bool ownerHasEventType(IEventObject *eventOwner, ClassId eventClassId) const;
    bool eventTypeHasReceiver(IEventObject *eventOwner, ClassId eventClassId, IEventObject *eventReceiver) const;
    void insertEventCallback(ClassId eventClassId, IEventObject *eventOwner, IEventObject *eventReceiver, EventCallback eventCallback);
    void removeEventCallback(ClassId eventClassId, IEventObject *eventOwner, IEventObject *eventReceiver);
    EventsManager::ReceiversFunctorMap& getReceiversFunctorMap(IEventObject *eventOwner, ClassId eventClassId);
    void subscribe(ClassId eventClassId, IEventObject *eventOwner, IEventObject *eventReceiver, EventCallback eventCallback);
    void unsubscribe(ClassId eventClassId, IEventObject *eventOwner, IEventObject *eventReceiver);
    void send(ClassId eventClassId, IEventObject *eventOwner, IEventObject *eventInstigator, Event *event);
};
REGISTER_CLASS(EventsManager);

NS_END