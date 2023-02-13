#pragma once

#include "Core/ObjectBase.hpp"

class CommandArgument: public ObjectBase
{
    GENERATE_METADATA(CommandArgument)

private:
    std::string mName;
    std::string mValue;

public:

    CRGET_SET(Name)
    CRGET_SET(Value)
};

class Command: public ObjectBase
{
    GENERATE_METADATA(Command)

private:
    std::string mName;
    std::string mArgumentsString;
    std::unordered_map<std::string, CommandArgument> mArguments;

    inline static const std::string smDefaultArgumentValue = ""; 

public:

    void clearArguments() { mArguments.clear(); };
    void addArgument(const CommandArgument& arg) { MAP_INSERT(mArguments, arg.getName(), arg); };
    bool argumentExists(const std::string& argName) const { return MAP_CONTAINS(mArguments, argName); };
    const std::string& getValue(const std::string& argName) const { return argumentExists(argName) ? mArguments.at(argName).getValue() : smDefaultArgumentValue; };

    CRGET_SET(Name)
    CRGET_SET(ArgumentsString)
    CRGET(Arguments)
};
