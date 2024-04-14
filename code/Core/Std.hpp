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

#include "Core/StdCore.hpp"
#include "Core/Memory/Memory.hpp"
#include "Core/Memory/Pointers.hpp"
#include "Core/Assert/Assert.hpp"
#include "Core/Hash.hpp"
#include "Core/Serialization/Serialization.hpp"
#include "Core/ByteBuffer.hpp"
#include "Core/GenericObject.hpp"
#include "Core/Functor.hpp"
#include "Core/Metadata/ClassManager.hpp"
#include "Core/Metadata/EnumsManager.hpp"
#include "Core/Memory/Pool.hpp"
#include "Core/SlotsManager.hpp"
#include "Core/Log/Log.hpp"
