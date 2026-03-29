#pragma once

#include "Core/System/System.hpp"
#include "Core/Time/Timer.hpp"

NS_BEGIN(Time)
class TimerManager: public System::System
{
public:
    void init();
    virtual void terminate() override;
    void update(Core::f32 dt);
    TimerHandle setTimer(Core::f32 duration, TimerDurationType durationType, std::function<void()> callback);
    void cancelTimer(const TimerHandle& timerHandle);

private:
    void endTimer(Timer * timer);

private:
	std::list<Timer *> mTimers;
};
REGISTER_CLASS(TimerManager)
NS_END