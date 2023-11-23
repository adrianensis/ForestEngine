#pragma once

// --------------------------------------------------------
// COMMON
// --------------------------------------------------------

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
#define IS_SMART_POINTER(Class) IS_BASE_OF(BasePtr, REMOVE_REF(Class))
#define IS_OWNER_POINTER(Class) IS_BASE_OF(BaseOwnerPtr, REMOVE_REF(Class))
#define IS_RAW_POINTER(Class) std::is_pointer<REMOVE_REF(Class)>::value
#define IS_ARITHMETIC(Class) std::is_arithmetic<REMOVE_REF(Class)>::value
#define IS_ENUM(Class) std::is_enum<Class>::value
#define ADD_CONST(Class) typename std::add_const<Class>::type
#define ADD_REFERENCE(Class) typename std::add_lvalue_reference<Class>::type
#define ADD_POINTER(Class) typename std::add_pointer<Class>::type

#define T_EXTENDS(Class, BaseClass) requires std::derived_from<Class, BaseClass>

// --------------------------------------------------------
// CLASS - METADATA MACROS
// --------------------------------------------------------

#define DECLARE_GET_PTR_THIS() \
	Ptr<ThisClass> getPtrToThis()                                               \
	{                                                                             \
		return getPtrToThisCasted<ThisClass>();  \
	}                                \
    Ptr<const ThisClass> getPtrToThis() const                                               \
	{                                                                             \
		return getPtrToThisCasted<const ThisClass>();  \
	}

// --------------------------------------------------------
// MEMBERS, GETTERS AND SETTERS
// --------------------------------------------------------

#define P(...)          \
    __VA_ARGS__;        \
    REGISTER_MEMBER(__VA_ARGS__);


#define GETTER_TYPE(Type)                         \
    COND_TYPE(                                    \
        IS_RAW_POINTER(Type),                     \
        Type,                                     \
        COND_TYPE(                                \
            IS_ARITHMETIC(Type) || IS_ENUM(Type), \
            REMOVE_REF(Type),                     \
            COND_TYPE(                                \
                IS_SMART_POINTER(Type),               \
                typename get_ptr_type<Type>::type,       \
                Type \
            )  \
        ) \
    )

#define CGETTER_TYPE(Type)                            \
    COND_TYPE(                                        \
        IS_RAW_POINTER(Type),                         \
        ADD_POINTER(ADD_CONST(REMOVE_POINTER(Type))), \
        COND_TYPE(                                    \
            IS_ARITHMETIC(Type) || IS_ENUM(Type),     \
            REMOVE_REF(Type),                         \
            COND_TYPE(                                \
                IS_SMART_POINTER(Type),               \
                typename get_const_ptr_type<Type>::type,       \
                GETTER_TYPE(Type) \
            )  \
        ) \
    )

#define SETTER_TYPE(Type)                         \
    COND_TYPE(                                    \
        IS_RAW_POINTER(Type),                     \
        ADD_CONST(Type),                          \
        COND_TYPE(                                \
            IS_ARITHMETIC(Type) || IS_ENUM(Type), \
            REMOVE_REF(Type),                     \
            COND_TYPE(                                \
                IS_OWNER_POINTER(Type),               \
                Type&&,       \
                ADD_REFERENCE(ADD_CONST(Type)) \
            )  \
        )                                         \
    )

#define GETTER_TYPE_FROM_VAR(Var) GETTER_TYPE(decltype(Var))
#define CGETTER_TYPE_FROM_VAR(Var) CGETTER_TYPE(decltype(Var))

#define SETTER_TYPE_FROM_VAR(Var) SETTER_TYPE(decltype(Var))

#define GET(BaseName)        \
	inline GETTER_TYPE_FROM_VAR(m##BaseName) get##BaseName() const { return m##BaseName; };

#define CGET(BaseName)        \
	inline CGETTER_TYPE_FROM_VAR(m##BaseName) get##BaseName() const { return m##BaseName; };

#define RGET(BaseName) \
	inline ADD_REFERENCE(GETTER_TYPE_FROM_VAR(m##BaseName)) get##BaseName() { return m##BaseName; };

#define CRGET(BaseName) \
	inline ADD_REFERENCE(ADD_CONST(GETTER_TYPE_FROM_VAR(m##BaseName))) get##BaseName() const { return m##BaseName; };

#define SET(BaseName)  \
	inline void set##BaseName(SETTER_TYPE_FROM_VAR(m##BaseName) new##BaseName) \
    { \
        m##BaseName = new##BaseName; \
    };

#define SETMOVE(BaseName)  \
	inline void setMove##BaseName(SETTER_TYPE_FROM_VAR(m##BaseName) new##BaseName) \
    { \
        m##BaseName = std::forward<decltype(m##BaseName)>(new##BaseName); \
    };

#define SET_DIRTY(BaseName)  \
	bool mDirty##BaseName = false;\
	inline void set##BaseName(SETTER_TYPE_FROM_VAR(m##BaseName) new##BaseName) { mDirty##BaseName = true; m##BaseName = new##BaseName; };

#define GET_SET(BaseName) GET(BaseName) SET(BaseName)
#define CGET_SET(BaseName) CGET(BaseName) SET(BaseName)
#define RGET_SET(BaseName) RGET(BaseName) SET(BaseName)
#define CRGET_SET(BaseName) CRGET(BaseName) SET(BaseName)

#define HASVALID(BaseName) inline bool hasValid ## BaseName() const { return m##BaseName.isValid(); }

#define GET_SET_MAP_CURRENT(BaseName) \
	decltype(m##BaseName)::key_type m##BaseName##CurrentKey = {}; \
	SET(BaseName##CurrentKey); \
	inline CGETTER_TYPE(decltype(m##BaseName)::mapped_type) get##BaseName##Current() const { return m##BaseName.at(m##BaseName##CurrentKey); }

#define ADD_TO_VECTOR(BaseName) \
    void addTo##BaseName(const decltype(m##BaseName)::value_type & newElement) { m##BaseName.push_back(newElement); } \
    void appendTo##BaseName(const decltype(m##BaseName) & otherVector) { m##BaseName.insert(m##BaseName.end(), otherVector.begin(), otherVector.end()); } \
    void clear##BaseName() { m##BaseName.clear(); }

// --------------------------------------------------------
// COPY
// --------------------------------------------------------

#define DECLARE_COPY(...)                                                             \
    __VA_ARGS__ &operator=(const __VA_ARGS__ &other)                                  \
    {                                                                                 \
        if (this != &other)                                                           \
        {                                                                             \
            __specificCopy(other);                                               \
        }                                                                             \
        return *this;                                                                 \
    }                                                                                 \
                                                                                      \
    void __specificCopy(const __VA_ARGS__ &other)

#define DO_COPY(MemberName) MemberName = other.MemberName;


#define DECLARE_MOVE(...)                                                             \
    __VA_ARGS__ &operator=(__VA_ARGS__ &&other)                                  \
    {                                                                                 \
        if (this != &other)                                                           \
        {                                                                             \
            __specificMove(other);                                               \
        }                                                                             \
        return *this;                                                                 \
    }                                                                                 \
                                                                                      \
    void __specificMove(__VA_ARGS__ &other)

// --------------------------------------------------------
// FOR LOOPS
// --------------------------------------------------------

#define FOR_MAP(it, map) \
	for (auto it = (map).begin(); it != (map).end(); ++it)

#define FOR_RANGE(it, start, size) for (i32 it = start; it < (i32)(size); ++it)
#define FOR_RANGE_COND(it, start, size, condition) for (i32 it = start; (it < (i32)(size)) and (condition); ++it)

#define FOR_ARRAY(it, array) FOR_RANGE(it, 0, (array).size())
#define FOR_ARRAY_COND(it, array, condition) FOR_RANGE_COND(it, 0, (array).size(), (condition))

#define FOR_LIST(it, list) for (auto it = (list).begin(); it != (list).end(); ++it)
#define FOR_LIST_COND(it, list, condition) for (auto it = (list).begin(); (it != (list).end()) and (condition); ++it)

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
// #define CHECK_EQUAL_HELPER(name) and BaseName == other.name
