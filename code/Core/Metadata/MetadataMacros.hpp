#pragma once

#include "Core/StdMacros.hpp"

// --------------------------------------------------------
// TYPE TRAITS
// --------------------------------------------------------
#define IS_SMART_POINTER(Class) IS_BASE_OF(BasePtr, REMOVE_REF(Class))
#define IS_OWNER_POINTER(Class) IS_BASE_OF(BaseOwnerPtr, REMOVE_REF(Class))

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

#define DECLARE_COPY()                                                             \
    ThisClass &operator=(const ThisClass &other)                                  \
    {                                                                                 \
        if (this != &other)                                                           \
        {                                                                             \
            __specificCopy(other);                                               \
        }                                                                             \
        return *this;                                                                 \
    }                                                                                 \
                                                                                      \
    void __specificCopy(const ThisClass &other)

#define DO_COPY(MemberName) MemberName = other.MemberName;


#define DECLARE_MOVE()                                                             \
    ThisClass &operator=(ThisClass &&other)                                  \
    {                                                                                 \
        if (this != &other)                                                           \
        {                                                                             \
            __specificMove(other);                                               \
        }                                                                             \
        return *this;                                                                 \
    }                                                                                 \
                                                                                      \
    void __specificMove(ThisClass &other)
