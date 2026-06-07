#pragma once

#include <cstring>
#include <cmath> /* ceil, max */
#include <limits>
#include <filesystem>

#include <vector>
#include <span>
#include <list>
#include <array>
#include <unordered_map>
#include <unordered_set>

#include <algorithm>
#include <execution>

#include "Core/CoreBase.hpp"
#include "Core/Memory/Memory.hpp"
#include "Core/Memory/Pointers.hpp"
#include "Core/TypeMacros.hpp"
#include "Core/Threads/ThreadPool.hpp"
#include "Core/HashedString/HashedString.hpp"
#include "Core/Tag/Tag.hpp"
#include "Core/Assert/Assert.hpp"
#include "Core/Serialization/Serialization.hpp"
#include "Core/Functor/Functor.hpp"
#include "Core/Metadata/ClassManager.hpp"
#include "Core/Metadata/EnumsManager.hpp"
#include "Core/Memory/Pool.hpp"
#include "Core/Memory/SlotsManager.hpp"
#include "Core/Log/Log.hpp"

#include "Maths/MathUtils.hpp"
#include "Maths/Vector2.hpp"
#include "Maths/Vector3.hpp"
#include "Maths/Vector4.hpp"
#include "Maths/Quaternion.hpp"
#include "Maths/Matrix4.hpp"
#include "Maths/Geometry.hpp"

#ifdef ENGINE_ENABLE_PROFILER
#include "Core/Profiler/Profiler.hpp"
#endif