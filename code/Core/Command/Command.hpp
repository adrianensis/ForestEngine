#pragma once

#include "Core/ObjectBase.hpp"

class CommandArgument: public ObjectBase
{
    GENERATE_METADATA(CommandArgument)
    PRI std::string mName; GET_RC_SET(Name);
    PRI std::string mValue; GET_RC_SET(Value);

public:

    COPY(CommandArgument)
    {
        DO_COPY(mName)
        DO_COPY(mValue)
    }
};

class Command: public ObjectBase
{
    GENERATE_METADATA(Command)
    PRI std::string mName; GET_RC_SET(Name);
    PRI std::string mArgumentsString; GET_RC_SET(ArgumentsString);
    PRI std::map<std::string, CommandArgument> mArguments; GET_RC(Arguments);

    inline static const std::string smDefaultArgumentValue = ""; 

public:

    void clearArguments() { mArguments.clear(); };
    void addArgument(const CommandArgument& arg) { MAP_INSERT(mArguments, arg.getName(), arg); };
    bool argumentExists(const std::string& argName) const { return MAP_CONTAINS(mArguments, argName); };
    const std::string& getValue(const std::string& argName) const { return argumentExists(argName) ? mArguments.at(argName).getValue() : smDefaultArgumentValue; };

    COPY(Command)
    {
        DO_COPY(mName)
        DO_COPY(mArguments)
        DO_COPY(mArgumentsString)
    }
};