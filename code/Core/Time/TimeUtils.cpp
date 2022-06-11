#include "Core/Time/TimeUtils.hpp"
#include "Graphics/RenderContext.hpp"

void TimeMark::init()
{
	mIsStarted = false;
	mDeltaTimeMillis = 0.0;
	mLastTime = std::chrono::high_resolution_clock::now();
}

void TimeMark::start()
{
	mIsStarted = true;
	mStartTime = std::chrono::high_resolution_clock::now();
}

void TimeMark::end()
{
	if (mIsStarted)
	{
		mIsStarted = false;
		mLastTime = std::chrono::high_resolution_clock::now();
		std::chrono::milliseconds mDeltaTimeChronoDuration = std::chrono::duration_cast<std::chrono::milliseconds>(mLastTime - mStartTime);
		mDeltaTimeMillis = mDeltaTimeChronoDuration.count();
	}
}

f32 TimeMark::getElapsedTimeMillis()
{
	auto now = std::chrono::high_resolution_clock::now();
	return mIsStarted ? std::chrono::duration_cast<std::chrono::milliseconds>(now - mStartTime).count() : 0.0f;
}

f32 TimeMark::getElapsedTimeSeconds()
{
	return getElapsedTimeMillis() / 1000.0f;
}

f32 TimeMark::getDeltaTimeMillis()
{
	return mDeltaTimeMillis;
}

f32 TimeMark::getDeltaTimeSeconds()
{
	return getDeltaTimeMillis() / 1000.0f;
}

// -------------------------------------------
// GPU
// -------------------------------------------

void TimeMarkGPU::init()
{
	mIsStarted = false;
	mDeltaTimeMillis = 0.0;
	mLastTime = 0;
	mStartTime = 0;
	
	// generate two queries
	glGenQueries(1, &mQueryIDStart);
	glGenQueries(1, &mQueryIDEnd);

}

void TimeMarkGPU::start()
{
	mIsStarted = true;

	glQueryCounter(mQueryIDStart, GL_TIMESTAMP);
}

void TimeMarkGPU::end()
{
	if (mIsStarted)
	{
		glQueryCounter(mQueryIDEnd, GL_TIMESTAMP);

		i32 stopTimerAvailable = 0;
		while (!stopTimerAvailable) {
			glGetQueryObjectiv(mQueryIDEnd, 
									GL_QUERY_RESULT_AVAILABLE, 
									&stopTimerAvailable);
		}

		// get query results
		glGetQueryObjectui64v(mQueryIDStart, GL_QUERY_RESULT, &mStartTime);
		glGetQueryObjectui64v(mQueryIDEnd, GL_QUERY_RESULT, &mLastTime);

		mIsStarted = false;
		mDeltaTimeMillis = (mLastTime - mStartTime) / 1000000.0f;
	}
}

f32 TimeMarkGPU::getDeltaTimeMillis()
{
	return mDeltaTimeMillis;
}

f32 TimeMarkGPU::getDeltaTimeSeconds()
{
	return getDeltaTimeMillis() / 1000.0f;
}
