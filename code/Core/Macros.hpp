#pragma once

#define NONE(...)

// --------------------------------------------------------
// TYPE TRAITS
// --------------------------------------------------------
#define IS_BASE_OF(BaseClass, DerivedClass) std::is_base_of<BaseClass, DerivedClass>::value

#define COND_TYPE(Bool, T1, T2) typename std::conditional<Bool, T1, T2>::type

#define REMOVE_REF(Class) typename std::remove_reference<Class>::type
#define REMOVE_POINTER(Class) typename std::remove_pointer<Class>::type
#define IS_SMART_POINTER(Class) IS_BASE_OF(BasePtr, REMOVE_REF(Class))
#define IS_RAW_POINTER(Class) std::is_pointer<REMOVE_REF(Class)>::value
#define IS_ARITHMETIC(Class) std::is_arithmetic<REMOVE_REF(Class)>::value
#define IS_ENUM(Class) std::is_enum<Class>::value
#define ADD_CONST(Class) typename std::add_const<Class>::type
#define ADD_REFERENCE(Class) typename std::add_lvalue_reference<Class>::type
#define ADD_POINTER(Class) typename std::add_pointer<Class>::type

// --------------------------------------------------------
// CLASS - METADATA MACROS
// --------------------------------------------------------

#define CLASS_MACRO_CONSTRUCTOR(ClassName) \
	ClassName() = default;        \
	~ClassName() override = default;

#define DECLARE_GET_PTR_THIS() \
	Ptr<ThisClass> getPtrToThis()                                               \
	{                                                                             \
		return Ptr<ThisClass>(std::static_pointer_cast<ThisClass>(shared_from_this()));  \
	}                                \
    Ptr<const ThisClass> getPtrToThis() const                                               \
	{                                                                             \
		return Ptr<const ThisClass>(std::static_pointer_cast<const ThisClass>(shared_from_this()));  \
	}

/*
    NOTE: tagging methods as virtual here have consecuences: "virtual ClassId getClassId()"
    may cause "struct"-like classes to become virtual and require a virtual destructor 
    completely obliterating the engine. Related with virtual destructors.
*/

#define DECLARE_METADATA_METHODS(Virtual, Override) \
	constexpr static ClassId getClassIdStatic() { return smClassId; }; \
    constexpr static const std::string_view& getClassNameStatic() { return smClassName; }; \
	constexpr Virtual ClassId getClassId() const Override { return ThisClass::getClassIdStatic(); }; \
	constexpr Virtual const std::string_view& getClassName() const Override { return ThisClass::getClassNameStatic(); };

#define GENERATE_METADATA_BASE(...)                                    \
    private:                                                               \
        using ThisClass = __VA_ARGS__;                                     \
        constexpr inline static const std::string_view smClassName = #__VA_ARGS__##sv; \
        constexpr inline static const ClassId smClassId = Hash::hashString(ThisClass::smClassName);

#define GENERATE_METADATA(...)                      \
        GENERATE_METADATA_BASE(__VA_ARGS__)             \
    public:                                             \
        DECLARE_METADATA_METHODS(NONE(0), override) \
    private:                                            \
        DECLARE_GET_PTR_THIS()               \
    private: // NOTE: notice the last blank space " "

#define GENERATE_METADATA_STRUCT(...)              \
        GENERATE_METADATA_BASE(__VA_ARGS__)            \
    public:                                            \
        DECLARE_METADATA_METHODS(NONE(0), NONE(0)) \
    private: // NOTE: notice the last blank space " "

#define REGISTER_CLASS_BY_NAME(...) \
    ClassRegister classRegister_##__VA_ARGS__ {__VA_ARGS__::getClassNameStatic(), [](){ \
        if constexpr (std::is_abstract_v<__VA_ARGS__>) { return nullptr; } \
        else { return Memory::newObject<__VA_ARGS__>(); } \
    }};

// --------------------------------------------------------
// MEMBERS, GETTERS AND SETTERS
// --------------------------------------------------------

#define GETTER_TYPE(Type)                         \
    COND_TYPE(                                    \
        IS_RAW_POINTER(Type),                     \
        Type,                                     \
        COND_TYPE(                                \
            IS_ARITHMETIC(Type) || IS_ENUM(Type), \
            REMOVE_REF(Type),                     \
            Type))

#define CGETTER_TYPE(Type)                            \
    COND_TYPE(                                        \
        IS_RAW_POINTER(Type),                         \
        ADD_POINTER(ADD_CONST(REMOVE_POINTER(Type))), \
        COND_TYPE(                                    \
            IS_ARITHMETIC(Type) || IS_ENUM(Type),     \
            REMOVE_REF(Type),                         \
            COND_TYPE(                                \
                IS_SMART_POINTER(Type),               \
                get_const_ptr_type<Type>::type,       \
                GETTER_TYPE(Type))))

#define SETTER_TYPE(Type)                         \
    COND_TYPE(                                    \
        IS_RAW_POINTER(Type),                     \
        ADD_CONST(Type),                          \
        COND_TYPE(                                \
            IS_ARITHMETIC(Type) || IS_ENUM(Type), \
            REMOVE_REF(Type),                     \
            ADD_REFERENCE(ADD_CONST(Type))))

#define GETTER_TYPE_FROM_VAR(Var) GETTER_TYPE(decltype(Var))
#define CGETTER_TYPE_FROM_VAR(Var) CGETTER_TYPE(decltype(Var))

#define SETTER_TYPE_FROM_VAR(Var) SETTER_TYPE(decltype(Var))

#define GET(BaseName)        \
	inline GETTER_TYPE_FROM_VAR(m##BaseName) get##BaseName() const { return m##BaseName; };

#define CGET(BaseName)        \
	inline CGETTER_TYPE_FROM_VAR(m##BaseName) get##BaseName() const { return m##BaseName; };

#define RGET(BaseName) \
	inline ADD_REFERENCE(ADD_CONST(GETTER_TYPE_FROM_VAR(m##BaseName))) get##BaseName() const { return m##BaseName; };

#define CRGET(BaseName) \
	inline ADD_REFERENCE(ADD_CONST(GETTER_TYPE_FROM_VAR(m##BaseName))) get##BaseName() const { return m##BaseName; };

#define SET(BaseName)  \
	inline void set##BaseName(SETTER_TYPE_FROM_VAR(m##BaseName) new##BaseName) { m##BaseName = new##BaseName; };

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
    void appendTo##BaseName(const decltype(m##BaseName) & otherVector) { m##BaseName.insert(m##BaseName.end(), otherVector.begin(), otherVector.end()); }

// --------------------------------------------------------
// COPY
// --------------------------------------------------------

#define DECLARE_COPY(...)                                                             \
    __VA_ARGS__ &operator=(const __VA_ARGS__ &other)                                  \
    {                                                                                 \
        if (this != &other)                                                           \
        {                                                                             \
            const __VA_ARGS__ *otherCast = dynamic_cast<const __VA_ARGS__ *>(&other); \
            __specificCopy(*otherCast);                                               \
        }                                                                             \
        return *this;                                                                 \
    }                                                                                 \
                                                                                      \
    void __specificCopy(const __VA_ARGS__ &other)

#define DO_COPY(MemberName) MemberName = other.MemberName;

// --------------------------------------------------------
// SERIALIZATION
// --------------------------------------------------------

#define DECLARE_SERIALIZATION() \
public:\
	void serialize(JSON& json) const override; \
	void deserialize(const JSON& json) override; \
private: // NOTE: notice the last blank space " "

#define IMPLEMENT_SERIALIZATION(...) void __VA_ARGS__::serialize(JSON& json) const  
#define IMPLEMENT_DESERIALIZATION(...) void __VA_ARGS__::deserialize(const JSON& json)  

// SERIALIZE

#define SERIALIZE(Name, Var)\
json[Name] = SerializationUtils::serializeTemplated<decltype(Var)>(Var);

#define SERIALIZE_IF(Condition, Name, Var)\
if((Condition))\
{\
	SERIALIZE(Name, Var)\
}

#define SERIALIZE_LIST_ELEMENT(Name, Var)\
json[Name].push_back(SerializationUtils::serializeTemplated<decltype(Var)>(Var));

#define SERIALIZE_LIST(Name, Var)\
FOR_LIST(__it, Var)\
{\
	SERIALIZE_LIST_ELEMENT(Name, (*__it))\
}

#define SERIALIZE_LIST_IF(Name, Var, ConditionLambda)\
FOR_LIST(__it, Var)\
{\
	if((ConditionLambda(*__it)))\
	{\
		SERIALIZE_LIST_ELEMENT(Name, (*__it))\
	}\
}

// #define SERIALIZE_MAP(Name, Var)
// JSON __jsonMap = JSON::object();
// FOR_MAP(__it, Var)
// {
//     __jsonMap[__it->first]  = SerializationUtils::serializeTemplated<decltype(__it->second)>(__it->second);
// }
// json[Name] = __jsonMap;

// DESERIALIZE

#define DESERIALIZE(Name, Var)\
SerializationUtils::deserializeTemplated<decltype(Var)>(Var, json[Name]);

#define DESERIALIZE_LIST(Name, Var, ConstructionLambda)\
if(!json.empty() and json.contains(Name))\
{\
    FOR_LIST(__it, json[Name])\
    {\
        auto object = ConstructionLambda(*__it);\
        SerializationUtils::deserializeTemplated<decltype(object)>(object, (*__it));\
        (Var).push_back(object);\
    }\
}

// #define DESERIALIZE_MAP(Name, Var, ConstructionLambda)
// if(!json.empty() and json.contains(Name))
// {
//     FOR_LIST(__it, json[Name])
//     {
//         auto object = ConstructionLambda(*__it);
//         SerializationUtils::deserializeTemplated<decltype(object)>(object, (*__it));
//         MAP_INSERT(Var, json[Name], object);
//     }
// }

// NO INTERFACE - SERIALIZATION MACROS
// useful when you don't want to increase the class size with extra virtual methods from ISerializable

#define TEMPLATED_SERIALIZATION(...)\
template<>\
JSON SerializationUtils::serializeTemplated(const __VA_ARGS__& value);\
template<>\
void SerializationUtils::deserializeTemplated(__VA_ARGS__& value, const JSON& json);

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

#define MAP_INSERT(map, key, value) (map).insert_or_assign((key), (value));
#define MAP_CONTAINS(map, key) ((map).find(key) != (map).end())

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

#define CONTAINS(container, element) (std::find((container).begin(), (container).end(), (element)) != (container).end())

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

// #define CHECK_EQUAL(...) true FOR_EACH(CHECK_EQUAL_HELPER, __VA_ARGS__)
// #define CHECK_EQUAL_HELPER(name) and BaseName == other.name
