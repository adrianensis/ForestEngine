#pragma once

#include "Core/BasicTypes.hpp"
#include "Core/ObjectBase.hpp"
#include "Core/Time/TimerManager.hpp"

class Event: public ObjectBase
{
    GENERATE_METADATA(Event)

public:
	f32 mDelayAmount = 0.0f;
	TimerDurationType mDelayType;
	ObjectBase* mInstigator = nullptr;
	
};

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
		return mEventClassId == eventFunctor.mEventClassId and mEventReceiver == eventFunctor.mEventReceiver;
	}
};
