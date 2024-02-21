#pragma once

#include "Core/Std.hpp"

class CommandArgument
{
private:
    std::string mName;
    std::string mValue;

public:

    CRGET_SET(Name)
    CRGET_SET(Value)
};

class Command
{
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
