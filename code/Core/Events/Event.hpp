#pragma once

#include "Core/Time/Timer.hpp"

NS_BEGIN(Core)
class IEventObject
{
};

class Event
{
public:
	Core::f32 mDelayAmount = 0.0f;
	TimerDurationType mDelayType;
	IEventObject* mInstigator = nullptr;
	
};
REGISTER_CLASS(Event);

using EventCallback = std::function<void(const Event *)>;

template<class E> T_EXTENDS(E, Event)
class EventFunctor: public Core::Functor<EventCallback>
{
public:
	E* mEvent = nullptr;
	ClassId mEventClassId;
	IEventObject* mEventReceiver = nullptr;

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

NS_END