#pragma once

#define NONE(...)

#define PUB public:
#define PRO protected:
#define PRI private:

#define REMOVE_REF(Class) typename std::remove_reference<Class>::type
#define REMOVE_POINTER(Class) typename std::remove_pointer<Class>::type
#define IS_POINTER(Class) std::is_pointer<REMOVE_REF(Class)>::value
#define IS_ARITHMETIC(Class) std::is_arithmetic<REMOVE_REF(Class)>::value
#define IS_ENUM(Class) std::is_enum<Class>::value
#define ADD_CONST(Class) typename std::add_const<Class>::type
#define ADD_REFERENCE(Class) typename std::add_lvalue_reference<Class>::type
#define ADD_POINTER(Class) typename std::add_pointer<Class>::type

#define IS_BASE_OF(BaseClass, DerivedClass) std::is_base_of<BaseClass, DerivedClass>::value

#define COND_TYPE(Bool, T1, T2) typename std::conditional<Bool, T1, T2>::type

// --------------------------------------------------------
// NEW - DELETE
// --------------------------------------------------------

#define NEW(...) Memory::newObject<__VA_ARGS__>()
#define INSTANCE_BY_NAME(ClassNameString, ...) ClassManager::getInstance().instanceByName<__VA_ARGS__>(ClassNameString)
#define DELETE(...) Memory::deleteObject<REMOVE_POINTER(REMOVE_REF(decltype(__VA_ARGS__)))>(__VA_ARGS__);

// --------------------------------------------------------
// MAIN
// --------------------------------------------------------

#define ENGINE_MAIN() \
void __customMain();\
void __initMain()\
{\
	Engine::getInstance().init();\
}\
void __runMain()\
{\
	Engine::getInstance().run();\
	Engine::getInstance().terminate();\
	Engine::deleteInstance();\
}\
int main()\
{\
	ModuleClassesRegister(); /* This comes from generateInstanceByName.py */ \
	__initMain();\
	__customMain();\
	__runMain();\
	return 0;\
}\
void __customMain()

// --------------------------------------------------------
// CLASS - METADATA MACROS
// --------------------------------------------------------

#define CLASS_MACRO_CONSTRUCTOR(ClassName) \
	ClassName() = default;        \
	~ClassName() override = default;

#define GENERATE_METADATA(...)                                                    \
	PRI \
		inline static std::string smClassName = std::string(#__VA_ARGS__);        \
		inline static ClassId smClassId = Hash::hashString(__VA_ARGS__::smClassName); \
	PUB \
		static ClassId getClassIdStatic()                                             \
		{                                                                             \
			return smClassId;                                                         \
		};                                                                            \
																					\
		ClassId getClassId() const override                                           \
		{                                                                             \
			return __VA_ARGS__::getClassIdStatic();                                   \
		};                                                                            \
																					\
		static std::string getClassNameStatic()                                       \
		{                                                                             \
			return smClassName;                                                       \
		};                                                                            \
																					\
		std::string getClassName() const override                                     \
		{                                                                             \
			return __VA_ARGS__::getClassNameStatic();                                 \
		};                                                                            \
	PRI // NOTE: notice the last blank space " "

// --------------------------------------------------------
// MEMBERS, GETTERS AND SETTERS
// --------------------------------------------------------

#define GETTER_TYPE(Type)                                            \
	COND_TYPE(                                                      \
		IS_POINTER(Type),                                  \
		ADD_CONST(Type),                                   \
		COND_TYPE(                                                  \
			IS_ARITHMETIC(Type) || IS_ENUM(Type), \
			REMOVE_REF(Type),                              \
			Type))

#define SETTER_TYPE(Type) \
	COND_TYPE(                                                      \
		IS_POINTER(Type),                                  \
		ADD_CONST(Type),                                   \
		COND_TYPE(                                                  \
			IS_ARITHMETIC(Type) || IS_ENUM(Type), \
			REMOVE_REF(Type),                              \
			ADD_REFERENCE(ADD_CONST(Type))))

#define GETTER_TYPE_FROM_VAR(Var) GETTER_TYPE(decltype(Var))

#define SETTER_TYPE_FROM_VAR(Var) SETTER_TYPE(decltype(Var))

#define GENERATE_GET(BaseName)        \
	PUB GETTER_TYPE_FROM_VAR(m##BaseName) get##BaseName() const { return m##BaseName; };

#define GENERATE_GET_R(BaseName) \
	PUB ADD_REFERENCE(GETTER_TYPE_FROM_VAR(m##BaseName)) get##BaseName() { return m##BaseName; };

#define GENERATE_GET_RC(BaseName) \
	PUB ADD_REFERENCE(ADD_CONST(GETTER_TYPE_FROM_VAR(m##BaseName))) get##BaseName() const { return m##BaseName; };

#define GENERATE_SET(BaseName) \
	PUB void set##BaseName(SETTER_TYPE_FROM_VAR(m##BaseName) new##BaseName) { m##BaseName = new##BaseName; };

#define GET(BaseName) GENERATE_GET(BaseName)
#define GET_R(BaseName) GENERATE_GET_R(BaseName)
#define GET_RC(BaseName) GENERATE_GET_RC(BaseName)

#define SET(BaseName) GENERATE_SET(BaseName)

#define GET_SET(BaseName) GENERATE_GET(BaseName) GENERATE_SET(BaseName)
#define GET_R_SET(BaseName) GENERATE_GET_R(BaseName) GENERATE_SET(BaseName)
#define GET_RC_SET(BaseName) GENERATE_GET_RC(BaseName) GENERATE_SET(BaseName)

/*
#define FUNCTION_MEMBER(ReturnClassName, FuncName, VirtualMacro, ModifiersMacro, Visibility, ...)\
Visibility:\
VirtualMacro() ReturnClassName FuncName (__VA_ARGS__) ModifiersMacro();\
Visibility:

#define VIR() virtual
#define OVR() override
#define CONST() const
#define CONST_OVR() const override

#define PUB_F(ReturnClassName, FuncName, ...) FUNCTION_MEMBER(ReturnClassName, FuncName, NONE, NONE, public, __VA_ARGS__)
#define PUB_FC(ReturnClassName, FuncName, ...) FUNCTION_MEMBER(ReturnClassName, FuncName, NONE, CONST, public, __VA_ARGS__)
#define PUB_FO(ReturnClassName, FuncName, ...) FUNCTION_MEMBER(ReturnClassName, FuncName, NONE, OVR, public, __VA_ARGS__)
#define PUB_FCO(ReturnClassName, FuncName, ...) FUNCTION_MEMBER(ReturnClassName, FuncName, NONE, CONST_OVR, public, __VA_ARGS__)
#define PUB_VF(ReturnClassName, FuncName, ...) FUNCTION_MEMBER(ReturnClassName, FuncName, VIR, NONE, public, __VA_ARGS__)
#define PUB_VFC(ReturnClassName, FuncName, ...) FUNCTION_MEMBER(ReturnClassName, FuncName, VIR, CONST, public, __VA_ARGS__)
#define PUB_VFO(ReturnClassName, FuncName, ...) FUNCTION_MEMBER(ReturnClassName, FuncName, VIR, OVR, public, __VA_ARGS__)
#define PUB_VFCO(ReturnClassName, FuncName, ...) FUNCTION_MEMBER(ReturnClassName, FuncName, VIR, CONST_OVR, public, __VA_ARGS__)
*/

// --------------------------------------------------------
// COPY
// --------------------------------------------------------

#define COPY(...)                                                                        \
	virtual void copy(const ObjectBase *other) override                                  \
	{                                                                                    \
		if (this != other)                                                               \
		{                                                                                \
			if (const __VA_ARGS__ *otherCast = dynamic_cast<const __VA_ARGS__ *>(other)) \
			{                                                                            \
				specificCopy(otherCast);                                                 \
			}                                                                            \
		}                                                                                \
	}                                                                                    \
                                                                                         \
	void specificCopy(const __VA_ARGS__ *other)

#define DO_COPY(MemberName) MemberName = other->MemberName;

// --------------------------------------------------------
// SERIALIZATION
// --------------------------------------------------------

// DECLARATION AND IMPLEMENTATION

#define DECL_SERIALIZATION()\
PUB \
void serialize(JSON& json) const override;\
void deserialize(const JSON& json) override;

#define IMPL_SERIALIZE(...)\
void __VA_ARGS__::serialize(JSON& json) const

#define IMPL_DESERIALIZE(...)\
void __VA_ARGS__::deserialize(const JSON& json)

#define IMPL_DEFAULT_SERIALIZATION(...)\
IMPL_SERIALIZE(__VA_ARGS__) { }; IMPL_DESERIALIZE(__VA_ARGS__) { };

// SERIALIZE

#define DO_SERIALIZE(Name, Var)\
json[Name] = SerializationUtils::serializeTemplated<decltype(Var)>(Var);

#define DO_SERIALIZE_IF(Condition, Name, Var)\
if((Condition))\
{\
	DO_SERIALIZE(Name, Var)\
}

#define DO_SERIALIZE_LIST_ELEMENT(Name, Var)\
json[Name].push_back(SerializationUtils::serializeTemplated<decltype(Var)>(Var));

#define DO_SERIALIZE_LIST(Name, Var)\
FOR_LIST(__it, Var)\
{\
	DO_SERIALIZE_LIST_ELEMENT(Name, (*__it))\
}

#define DO_SERIALIZE_LIST_IF(Name, Var, ConditionLambda)\
FOR_LIST(__it, Var)\
{\
	if((ConditionLambda(*__it)))\
	{\
		DO_SERIALIZE_LIST_ELEMENT(Name, (*__it))\
	}\
}

// #define DO_SERIALIZE_MAP(Name, Var)
// JSON __jsonMap = JSON::object();
// FOR_MAP(__it, Var)
// {
//     __jsonMap[__it->first]  = SerializationUtils::serializeTemplated<decltype(__it->second)>(__it->second);
// }
// json[Name] = __jsonMap;

// DESERIALIZE

#define DO_DESERIALIZE(Name, Var)\
SerializationUtils::deserializeTemplated<decltype(Var)>(Var, json[Name]);

#define DO_DESERIALIZE_LIST(Name, Var, ConstructionLambda)\
if(!json.empty() && json.contains(Name))\
{\
    FOR_LIST(__it, json[Name])\
    {\
        auto object = ConstructionLambda(*__it);\
        SerializationUtils::deserializeTemplated<decltype(object)>(object, (*__it));\
        (Var).push_back(object);\
    }\
}

// #define DO_DESERIALIZE_MAP(Name, Var, ConstructionLambda)
// if(!json.empty() && json.contains(Name))
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
#define FOR_RANGE_COND(it, start, size, condition) for (i32 it = start; (it < (i32)(size)) && (condition); ++it)

#define FOR_ARRAY(it, array) FOR_RANGE(it, 0, (array).size())
#define FOR_ARRAY_COND(it, array, condition) FOR_RANGE_COND(it, 0, (array).size(), (condition))

#define FOR_LIST(it, list) for (auto it = (list).begin(); it != (list).end(); ++it)
#define FOR_LIST_COND(it, list, condition) for (auto it = (list).begin(); (it != (list).end()) && (condition); ++it)

// --------------------------------------------------------
// STD CONTAINERS
// --------------------------------------------------------

#define MAP_CONTAINS(map, key) ((map).find(key) != (map).end())
#define MAP_INSERT(map, key, value) (map).insert_or_assign((key), (value));

#define MAP_DELETE_CONTENT(map)           \
	FOR_MAP(_internal_it, map)            \
	{                                     \
		if (_internal_it->second)         \
			DELETE(_internal_it->second); \
	}                                     \
	(map).clear();

#define LIST_DELETE_CONTENT(list)                       \
FOR_LIST(_internal_it, list) { DELETE(*_internal_it); } \
	(list).clear();
