#pragma once

#include "Core/CoreBase.hpp"
#include "Core/HashedString/HashedString.hpp"

NS_BEGIN(Paths)

class PredefinedPaths
{
public:
	inline static Core::HashedString mResources = "resources/";
	inline static Core::HashedString mOutput = "output/";
	inline static Core::HashedString mOutputLog = "output/log/";
	inline static Core::HashedString mOutputShaders = "output/shaders/";
};

NS_END