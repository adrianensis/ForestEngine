#include "Core/Time/TimerManager.hpp"

NS_BEGIN(Core)
void Timer::init(f32 duration, TimerDurationType durationType, std::function<void()> callback)
{
	mDurationType = durationType;
	mDuration = duration;
	mFunctor.mCallback = callback;
}
NS_END