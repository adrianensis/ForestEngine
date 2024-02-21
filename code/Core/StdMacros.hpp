#pragma once

// --------------------------------------------------------
// COMMON
// --------------------------------------------------------

#define INVALID_INDEX -1
#define EMPTY_MACRO(...)
#define ADD_TRAIL_COMMA(token) token ,
#define TO_STRING_AND_ADD_TRAIL_COMMA(token) #token ,

// --------------------------------------------------------
// TYPE TRAITS
// --------------------------------------------------------
#define IS_BASE_OF(BaseClass, DerivedClass) std::is_base_of<BaseClass, DerivedClass>::value

#define COND_TYPE(Bool, T1, T2) typename std::conditional<Bool, T1, T2>::type

#define REMOVE_REF(Class) typename std::remove_reference<Class>::type
#define REMOVE_POINTER(Class) typename std::remove_pointer<Class>::type
#define IS_RAW_POINTER(Class) std::is_pointer<REMOVE_REF(Class)>::value
#define IS_ARITHMETIC(Class) std::is_arithmetic<REMOVE_REF(Class)>::value
#define IS_ENUM(Class) std::is_enum<Class>::value
#define ADD_CONST(Class) typename std::add_const<Class>::type
#define ADD_REFERENCE(Class) typename std::add_lvalue_reference<Class>::type
#define ADD_POINTER(Class) typename std::add_pointer<Class>::type

#define T_EXTENDS(Class, BaseClass) requires std::derived_from<Class, BaseClass>

// --------------------------------------------------------
// FOR LOOPS
// --------------------------------------------------------

#define FOR_MAP(it, map) \
	for (auto it = (map).begin(); it != (map).end(); ++it)

#define FOR_RANGE(it, start, size) for (i32 it = start; it < (i32)(size); ++it)
#define FOR_RANGE_COND(it, start, size, condition) for (i32 it = start; (it < (i32)(size)) && (condition); ++it)

#define FOR_ARRAY(it, array) FOR_RANGE(it, 0, (array).size())
#define FOR_ARRAY_COND(it, array, condition) FOR_RANGE_COND(it, 0, (array).size(), (condition))

#define FOR_LIST(it, list) for (auto it = (list).begin(); it != (list).end(); ++it)
#define FOR_LIST_COND(it, list, condition) for (auto it = (list).begin(); (it != (list).end()) && (condition); ++it)

// --------------------------------------------------------
// STD CONTAINERS
// --------------------------------------------------------

#define MAP_DELETE_CONTENT(map)           \
	FOR_MAP(_internal_it, map)            \
	{                                     \
		if (_internal_it->second)         \
			Memory::deleteObject(_internal_it->second); \
	}                                     \
	(map).clear();

#define DELETE_CONTENT(container)                       \
FOR_LIST(_internal_it, container) { Memory::deleteObject(*_internal_it); } \
	(container).clear();

// --------------------------------------------------------
// FOR EACH
// --------------------------------------------------------

// C++20 required
// from: https://www.scs.stanford.edu/~dm/blog/va-opt.html

#define PARENS ()

#define EXPAND(...) EXPAND4(EXPAND4(EXPAND4(EXPAND4(__VA_ARGS__))))
#define EXPAND4(...) EXPAND3(EXPAND3(EXPAND3(EXPAND3(__VA_ARGS__))))
#define EXPAND3(...) EXPAND2(EXPAND2(EXPAND2(EXPAND2(__VA_ARGS__))))
#define EXPAND2(...) EXPAND1(EXPAND1(EXPAND1(EXPAND1(__VA_ARGS__))))
#define EXPAND1(...) __VA_ARGS__

#define FOR_EACH(macro, ...)                                    \
  __VA_OPT__(EXPAND(FOR_EACH_HELPER(macro, __VA_ARGS__)))

#define FOR_EACH_HELPER(macro, a1, ...)                         \
  macro(a1)                                                     \
  __VA_OPT__(FOR_EACH_AGAIN PARENS (macro, __VA_ARGS__))

#define FOR_EACH_AGAIN() FOR_EACH_HELPER

// ODD
#define FOR_EACH_ODD(macro, ...)                                    \
  __VA_OPT__(EXPAND(FOR_EACH_HELPER_ODD(macro, __VA_ARGS__)))

#define FOR_EACH_HELPER_ODD(macro, a1, a2, ...)                         \
  macro(a1)                                                     \
  __VA_OPT__(FOR_EACH_AGAIN_ODD PARENS (macro, __VA_ARGS__))

#define FOR_EACH_AGAIN_ODD() FOR_EACH_HELPER_ODD

// EVEN
#define FOR_EACH_EVEN(macro, ...)                                    \
  __VA_OPT__(EXPAND(FOR_EACH_HELPER_EVEN(macro, __VA_ARGS__)))

#define FOR_EACH_HELPER_EVEN(macro, a1, a2, ...)                         \
  macro(a2)                                                     \
  __VA_OPT__(FOR_EACH_AGAIN_EVEN PARENS (macro, __VA_ARGS__))

#define FOR_EACH_AGAIN_EVEN() FOR_EACH_HELPER_EVEN

// #define CHECK_EQUAL(...) true FOR_EACH(CHECK_EQUAL_HELPER, __VA_ARGS__)
// #define CHECK_EQUAL_HELPER(name) && BaseName == other.name
