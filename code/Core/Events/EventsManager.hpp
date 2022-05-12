#ifndef EVENTSMANAGER_HPP
#define EVENTSMANAGER_HPP

#include "Core/BasicTypes.hpp"
#include "Core/ObjectBase.hpp"
#include "Core/Events/Event.hpp"
#include "Core/Assert/Assert.hpp"
#include "Core/Singleton.hpp"

/*
  Macros for (un)susbscribing and sending events.
*/

#define SUBSCRIBE_TO_EVENT(EventClassName, owner, receiver, eventCallback) EventsManager::getInstance().subscribe<EventClassName>(owner, receiver, eventCallback);
#define UNSUBSCRIBE_TO_EVENT(EventClassName, owner, receiver) EventsManager::getInstance().unsubscribe<EventClassName>(owner, receiver);
#define SEND_EVENT(owner, instigator, event) EventsManager::getInstance().send(owner, instigator, &event);

class EventsManager: public ObjectBase, public Singleton<EventsManager>
{
	GENERATE_METADATA(EventsManager)

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

    void send(ObjectBase *eventOwner, ObjectBase *eventInstigator, Event *event);

private:
	using ReceiversFunctorMap = std::map<ObjectBase *, EventFunctor<Event>>;
	using EventReceiversMap = std::map<ClassId, ReceiversFunctorMap>;
	using OwnersMap = std::map<ObjectBase *, EventReceiversMap>;

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

#endif