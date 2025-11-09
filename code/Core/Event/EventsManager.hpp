#pragma once

#include "Core/Memory/Singleton.hpp"
#include "Core/Event/Event.hpp"

NS_BEGIN(Event)
/*
  Macros for (un)susbscribing and sending events.
*/

#define SUBSCRIBE_TO_EVENT(EventClassName, owner, receiver, eventCallback) Event::EventsManager::getInstance().subscribe<EventClassName>(owner, receiver, eventCallback);
#define UNSUBSCRIBE_TO_EVENT(EventClassName, owner, receiver) Event::EventsManager::getInstance().unsubscribe<EventClassName>(owner, receiver);
#define SEND_EVENT(owner, instigator, event) Event::EventsManager::getInstance().send<REMOVE_REFERENCE(decltype(event))>(owner, instigator, &event);

class EventsManager: public Core::Singleton<EventsManager>
{
public:
    void init();
    void terminate();

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
        Core::ClassId eventClassId = Core::ClassManager::getClassMetadata<E>().mClassDefinition.getId();
        send(eventClassId, eventOwner, eventInstigator, event);
    }

private:
	using ReceiversFunctorMap = std::unordered_map<IEventObject *, EventFunctor<Event>>;
	using EventReceiversMap = std::unordered_map<Core::ClassId, ReceiversFunctorMap>;
	using OwnersMap = std::unordered_map<IEventObject *, EventReceiversMap>;

	OwnersMap mOwnersMap;

    void removeMapContent();
    bool ownerExists(IEventObject *eventOwner) const;
    bool ownerHasEventType(IEventObject *eventOwner, Core::ClassId eventClassId) const;
    bool eventTypeHasReceiver(IEventObject *eventOwner, Core::ClassId eventClassId, IEventObject *eventReceiver) const;
    void insertEventCallback(Core::ClassId eventClassId, IEventObject *eventOwner, IEventObject *eventReceiver, EventCallback eventCallback);
    void removeEventCallback(Core::ClassId eventClassId, IEventObject *eventOwner, IEventObject *eventReceiver);
    EventsManager::ReceiversFunctorMap& getReceiversFunctorMap(IEventObject *eventOwner, Core::ClassId eventClassId);
    void subscribe(Core::ClassId eventClassId, IEventObject *eventOwner, IEventObject *eventReceiver, EventCallback eventCallback);
    void unsubscribe(Core::ClassId eventClassId, IEventObject *eventOwner, IEventObject *eventReceiver);
    void send(Core::ClassId eventClassId, IEventObject *eventOwner, IEventObject *eventInstigator, Event *event);
};
REGISTER_CLASS(EventsManager);

NS_END