#include "Engine/Time/TimerManager.hpp"

NS_BEGIN(Time)
void Timer::init(Core::f32 duration, TimerDurationType durationType, std::function<void()> callback)
{
	mDurationType = durationType;
	mDuration = duration;
	mFunctor.mCallback = callback;
}
NS_END