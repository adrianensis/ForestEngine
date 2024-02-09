#pragma once

#include "Core/StdCore.hpp"

class CommandArgument
{
    GENERATE_METADATA_STRUCT(CommandArgument)

private:
    std::string mName;
    std::string mValue;

public:

    CRGET_SET(Name)
    CRGET_SET(Value)
};

class Command
{
    GENERATE_METADATA_STRUCT(Command)

private:
    std::string mName;
    std::string mArgumentsString;
    std::unordered_map<std::string, CommandArgument> mArguments;

    inline static const std::string smDefaultArgumentValue = ""; 

public:

    void clearArguments() { mArguments.clear(); };
    void addArgument(const CommandArgument& arg) { mArguments.insert_or_assign(arg.getName(), arg); };
    bool argumentExists(const std::string& argName) const { return mArguments.contains(argName); };
    const std::string& getValue(const std::string& argName) const { return argumentExists(argName) ? mArguments.at(argName).getValue() : smDefaultArgumentValue; };

    CRGET_SET(Name)
    CRGET_SET(ArgumentsString)
    CRGET(Arguments)
};
