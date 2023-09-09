#pragma once

#include "json.hpp"

#include "Core/BasicTypes.hpp"

using JSON = nlohmann::ordered_json;

// TODO : use ordered_json only for debug and release? but json for ship version?
// ordered_json will keep the data order, json will sort the data alphabetically

class ISerializable
{
public:
	virtual void serialize(JSON& json) const {};
	virtual void deserialize(const JSON& json) {};
};

class SerializationUtils
{
public:
	template<class T>
	static JSON serializeTemplated(const T& value)
	{
		JSON json;
		if constexpr (IS_BASE_OF(ISerializable, REMOVE_POINTER(REMOVE_REF(T))))
		{
			if constexpr (IS_RAW_POINTER(T))
			{
				if(value)
				{
					value->serialize(json);
				}
			}
			else
			{
				value.serialize(json);
			}
		}
		else
		{
			json = value;
		}

		return json;
	}

	template<class T>
	static void deserializeTemplated(T& value, const JSON& json)
	{
		if(!json.empty())
        {
            if constexpr (IS_BASE_OF(ISerializable, REMOVE_POINTER(REMOVE_REF(T))))
            {
                if constexpr (IS_RAW_POINTER(T))
                {
                    if(value)
                    {
                        value->deserialize(json);
                    }
                }
                else
                {
                    value.deserialize(json);
                }
            }
            else
            {
                value = json.get<T>();
            }
        }
	}
};

// --------------------------------------------------------
// SERIALIZATION MACROS
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
