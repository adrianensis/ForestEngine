#pragma once

#include "Core/Std.hpp"
#include "Core/StdMacros.hpp"

NS_BEGIN(Core)

class Paths
{
public:
	inline static HashedString mResources = "resources/";
	inline static HashedString mOutput = "output/";
	inline static HashedString mOutputLog = "output/log/";
	inline static HashedString mOutputShaders = "output/shaders/";
};

NS_END