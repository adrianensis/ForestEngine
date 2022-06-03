#ifndef COMMAND_HPP
#define COMMAND_HPP

#include "Core/ObjectBase.hpp"

class CommandArgument: public ObjectBase
{
    GENERATE_METADATA(CommandArgument)

private:
    String mName;
    String mValue;

public:

    COPY(CommandArgument)
    {
        DO_COPY(mName)
        DO_COPY(mValue)
    }

    CRGET_SET(Name)
    CRGET_SET(Value)
};

class Command: public ObjectBase
{
    GENERATE_METADATA(Command)

private:
    String mName;
    String mArgumentsString;
    std::map<String, CommandArgument> mArguments;

    inline static const String smDefaultArgumentValue = ""; 

public:

    void clearArguments() { mArguments.clear(); };
    void addArgument(const CommandArgument& arg) { MAP_INSERT(mArguments, arg.getName(), arg); };
    bool argumentExists(const String& argName) const { return MAP_CONTAINS(mArguments, argName); };
    const String& getValue(const String& argName) const { return argumentExists(argName) ? mArguments.at(argName).getValue() : smDefaultArgumentValue; };

    COPY(Command)
    {
        DO_COPY(mName)
        DO_COPY(mArguments)
        DO_COPY(mArgumentsString)
    }

    CRGET_SET(Name)
    CRGET_SET(ArgumentsString)
    CRGET(Arguments)
};

#endif