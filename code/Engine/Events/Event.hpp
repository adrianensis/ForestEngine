#pragma once

#include "Core/Object/ObjectBase.hpp"
#include "Engine/Time/TimerManager.hpp"

class Event: public ObjectBase
{
public:
	f32 mDelayAmount = 0.0f;
	TimerDurationType mDelayType;
	ObjectBase* mInstigator = nullptr;
	
};
REGISTER_CLASS(Event);

using EventCallback = std::function<void(const Event *)>;

template<class E> T_EXTENDS(E, Event)
class EventFunctor: public Functor<EventCallback>
{
public:
	E* mEvent = nullptr;
	ClassId mEventClassId;
	ObjectBase* mEventReceiver = nullptr;

	void execute() override
	{
		if (mCallback)
		{
			mCallback(mEvent);
		}
	}

	bool operator==(const EventFunctor& eventFunctor) const
	{
		return mEventClassId == eventFunctor.mEventClassId && mEventReceiver == eventFunctor.mEventReceiver;
	}
};
