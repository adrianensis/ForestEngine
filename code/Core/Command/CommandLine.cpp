#include "Core/Command/CommandLine.hpp"

#include "Core/Command/DefaultCommands.hpp"
#include "Core/Log/Log.hpp"
#include "Core/Events/EventsManager.hpp"
#include "Core/Input/Input.hpp"

#include <regex>

void CommandLine::init()
{
    mBuffer = "";
    mIsOpen = false;

    SUBSCRIBE_TO_EVENT(InputEventChar, nullptr, this, [this](const Event *event)
    {
        if(mIsOpen)
        {
            const InputEventChar *e = (const InputEventChar*) event;
            char c = e->mChar;
            mBuffer.push_back(c);
            writeLine(mBuffer, false);
        }
    });

    SUBSCRIBE_TO_EVENT(InputEventKeyEnter, nullptr, this, [this](const Event *event)
    {
        if(mIsOpen)
        {
            LOG_BRLINE();
            execute(mBuffer);
            mBuffer.clear();
            writeLine("", false);
        }
    });

    SUBSCRIBE_TO_EVENT(InputEventKeyBackspace, nullptr, this, [this](const Event *event)
    {
        if(mIsOpen)
        {
            if(!mBuffer.empty())
            {
                LOG_BACKSPACE()
                mBuffer.pop_back();
            }
            
            writeLine(mBuffer, false);
        }
    });

    SUBSCRIBE_TO_EVENT(InputEventKeyArrow, nullptr, this, [this](const Event *event)
    {
        if(mIsOpen)
        {
            // TODO: GLFW_KEY_UP && DOWN creates a dependency with Graphics/Window module 

            // const InputEventKeyArrow *e = (const InputEventKeyArrow*) event;

            // switch (e->mArrowButton)
            // {
            //     case GLFW_KEY_UP:
            //     {
            //         if(!mHistory.empty() && mHistoryIterator != mHistory.begin())
            //         {
            //             --mHistoryIterator;
            //             mBuffer = *mHistoryIterator;
            //         }
            //         break;
            //     }
            //     case GLFW_KEY_DOWN:
            //     {
            //         if(!mHistory.empty() && mHistoryIterator != mHistory.end())
            //         {
            //             ++mHistoryIterator;
            //             if(mHistoryIterator == mHistory.end())
            //             {
            //                 mBuffer.clear();
            //             }
            //             else
            //             {
            //                 mBuffer = *mHistoryIterator;
            //             }
            //         }
            //         break;
            //     }
            // }
        }
    });

    DefaultCommands::registerDefaultCommands();
}

void CommandLine::writeLine(const std::string& line, bool newLine /*= true*/) const
{
    if(newLine)
    {
        LOG_TAG("CMD", line)
    }
    else
    {
        LOG_TAG_APPEND("CMD", line)
    }
}

void CommandLine::update()
{

}

void CommandLine::terminate()
{

}

void CommandLine::execute(const std::string& commandLine)
{
    std::string patternValidName("[-+]?[a-zA-Z_\\.0-9]+");
    std::regex regexCommand("^\\s*(" + patternValidName + ")\\s*");

    std::smatch matchCommand;
    std::regex_search(commandLine, matchCommand, regexCommand);
    bool isCommand = !matchCommand.empty();

    std::string commandName = matchCommand[1];

    if(isCommand && mCommandsMap.contains(commandName))
    {
        //writeLine("command: " + commandName);

        CommandFunctor& functor = mCommandsMap.at(commandName);
        functor.mCommand.clearArguments();

        std::string patternAssignation("\\s*=\\s*");
        std::regex regexCommandWithArgumentList("^\\s*" + patternValidName + "\\s+((" + patternValidName + "(" + patternAssignation + patternValidName + ")?\\s*)+)\\s*");

        std::smatch matchCommandWithArgumentList;
        std::regex_search(commandLine, matchCommandWithArgumentList, regexCommandWithArgumentList);
        bool isCommandWithArgumentList = !matchCommandWithArgumentList.empty();

        if(isCommandWithArgumentList)
        {
            std::string argumentList = matchCommandWithArgumentList[1].str();

            std::regex regexArgument("\\s*(" + patternValidName + "(" + patternAssignation + patternValidName + ")?)\\s*");

            auto argumentlistBegin = std::sregex_iterator(argumentList.begin(), argumentList.end(), regexArgument);
            auto argumentlistEnd = std::sregex_iterator();
        
            //writeLine("arguments");
            //LOG_VAL(std::distance(argumentlistBegin, argumentlistEnd))
        
            functor.mCommand.setArgumentsString(argumentList);

            for (std::sregex_iterator i = argumentlistBegin; i != argumentlistEnd; ++i) {
                std::smatch argumentMatch = *i;                                                 
                std::string argumentStr = argumentMatch.str(); 

                std::regex regexPair("(" + patternValidName + ")(" + patternAssignation + "(" + patternValidName + "))?");

                std::smatch matchPair;
                std::regex_search(argumentStr, matchPair, regexPair);
                //bool isPair = !matchPair.empty();

                //LOG_VAL(matchPair[1].str())
                //LOG_VAL(matchPair[3].str())

                CommandArgument arg;
                arg.setName(matchPair[1].str());
                arg.setValue(matchPair[3].str());
                functor.mCommand.addArgument(arg);
            }   
        }
        
        functor.execute();
    }
    else
    {
        writeLine("command: " + commandLine + " not recognized.");
    }

    mHistory.push_back(commandLine);
    mHistoryIterator = mHistory.end();
}

std::string CommandLine::autocomplete(const std::string& commandLine)
{
    return std::string(); // TODO : implement command autocomplete
}

void CommandLine::registerCommand(const std::string& commandName, CommandCallback callback)
{
    CHECK_MSG(!mCommandsMap.contains(commandName), "Command " + commandName + " already registered!");

    Command command;
    command.setName(commandName);

    CommandFunctor functor;
    functor.setCallback(callback);
    functor.mCommand = command;

    mCommandsMap.insert_or_assign(commandName, functor);
}

void CommandLine::open()
{
    mIsOpen = true;
    writeLine("CMD Opened");
    writeLine(mBuffer, false);
}

void CommandLine::close()
{
    mIsOpen = false;
    writeLine(mBuffer);
    writeLine("CMD Closed");
}

void CommandLine::toggle()
{
    if(mIsOpen)
    {
        close();
    }
    else
    {
        open();
    }
}