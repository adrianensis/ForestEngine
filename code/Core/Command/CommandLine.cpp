#include "Core/Command/CommandLine.hpp"

#include "Core/Assert/Assert.hpp"
#include "Core/Command/DefaultCommands.hpp"
#include "Core/Input/InputEvents.hpp"
#include "Core/Log/Log.hpp"
#include "Core/Events/EventsManager.hpp"
#include "Core/StdMacros.hpp"
#include "GLFW/glfw3.h"

#include <cstdio>
#include <regex>
#include <string>
#include <unistd.h>

NS_BEGIN(Core)
void CommandLine::init()
{
    mBuffer.reserve(smBufferSize);
    mIsOpen = false;

	SUBSCRIBE_TO_EVENT(InputEventKeyReleased, nullptr, this, [this](const Event *event)
	{
        const InputEventKeyReleased *e = (const InputEventKeyReleased *)event;
        if(e->mKey == GLFW_KEY_GRAVE_ACCENT)
        {
            toggle();
        }
	});

    // SUBSCRIBE_TO_EVENT(InputEventKeyArrow, nullptr, this, [this](const Event *event)
    // {
    //     if(mIsOpen)
    //     {
    //         // NEXT: GLFW_KEY_UP && DOWN creates a dependency with Graphics/Window module 

    //         // const InputEventKeyArrow *e = (const InputEventKeyArrow*) event;

    //         // switch (e->mArrowButton)
    //         // {
    //         //     case GLFW_KEY_UP:
    //         //     {
    //         //         if(!mHistory.empty() && mHistoryIterator != mHistory.begin())
    //         //         {
    //         //             --mHistoryIterator;
    //         //             mBuffer = *mHistoryIterator;
    //         //         }
    //         //         break;
    //         //     }
    //         //     case GLFW_KEY_DOWN:
    //         //     {
    //         //         if(!mHistory.empty() && mHistoryIterator != mHistory.end())
    //         //         {
    //         //             ++mHistoryIterator;
    //         //             if(mHistoryIterator == mHistory.end())
    //         //             {
    //         //                 mBuffer.clear();
    //         //             }
    //         //             else
    //         //             {
    //         //                 mBuffer = *mHistoryIterator;
    //         //             }
    //         //         }
    //         //         break;
    //         //     }
    //         // }
    //     }
    // });

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
    if(mIsOpen && mBufferDirty)
    {
        writeLine(mBuffer);
        
        mBufferDirty = false;
    }
}

void CommandLine::terminate()
{
    if(mIsOpen)
    {
        close();
    }
}

void CommandLine::charReceived(char c)
{
    mBuffer.push_back(c);
    mBufferDirty = true;
}

void CommandLine::clearBuffer()
{
    mBuffer.clear();
}

Command CommandLine::extractCommand(const std::string& commandLine) const
{
    Command command;

    std::regex regexCommand("^\\s*(" + mPatternValidName + ")\\s*");
    std::smatch matchCommand;
    std::regex_search(commandLine, matchCommand, regexCommand);
    std::string commandName = matchCommand[1];
    //writeLine("command: " + commandName);

    command.setName(commandName);

    if(mCommandsMap.contains(commandName))
    {
        command = mCommandsMap.at(commandName).mCommand;
        command.clearArguments();
    }

    std::string patternAssignation("\\s*=\\s*");
    std::regex regexCommandWithArgumentList("^\\s*" + mPatternValidName + "\\s+((" + mPatternValidName + "(" + patternAssignation + mPatternValidName + ")?\\s*)+)\\s*");

    std::smatch matchCommandWithArgumentList;
    std::regex_search(commandLine, matchCommandWithArgumentList, regexCommandWithArgumentList);
    bool isCommandWithArgumentList = !matchCommandWithArgumentList.empty();
    if(isCommandWithArgumentList)
    {
        std::string argumentList = matchCommandWithArgumentList[1].str();

        std::regex regexArgument("\\s*(" + mPatternValidName + "(" + patternAssignation + mPatternValidName + ")?)\\s*");

        auto argumentlistBegin = std::sregex_iterator(argumentList.begin(), argumentList.end(), regexArgument);
        auto argumentlistEnd = std::sregex_iterator();
    
        //writeLine("arguments");
        //LOG_VAL(std::distance(argumentlistBegin, argumentlistEnd))
    
        command.setArgumentsString(argumentList);

        for (std::sregex_iterator i = argumentlistBegin; i != argumentlistEnd; ++i)
        {
            std::smatch argumentMatch = *i;                                                 
            std::string argumentStr = argumentMatch.str(); 

            std::regex regexPair("(" + mPatternValidName + ")(" + patternAssignation + "(" + mPatternValidName + "))?");

            std::smatch matchPair;
            std::regex_search(argumentStr, matchPair, regexPair);
            //bool isPair = !matchPair.empty();

            //LOG_VAL(matchPair[1].str())
            //LOG_VAL(matchPair[3].str())

            CommandArgument arg;
            arg.setName(matchPair[1].str());
            arg.setValue(matchPair[3].str());
            command.addArgument(arg);
        }   
    }

    return command;
}

bool CommandLine::checkCommand(const Command& command) const
{
    return command.isValid() && mCommandsMap.contains(command.getName().get());
}

void CommandLine::execute()
{
    // white line case
    if(mBuffer.empty())
    {
        return;
    }

    Command command = extractCommand(mBuffer);
    if(checkCommand(command))
    {
        CommandFunctor functor = mCommandsMap.at(command.getName());
        functor.mCommand = command;
        functor.execute();
    }
    else
    {
        writeLine("command: '" + mBuffer + "' not recognized.");
    }

    mHistory.push_back(mBuffer);
    mHistoryIterator = mHistory.end();
}

void CommandLine::autocomplete()
{
    // white line case
    if(mBuffer.empty())
    {
        return;
    }

    Command command = extractCommand(mBuffer);
    
    // only autocomplete first token (command name), if arguments found, skip
    if(!command.getArguments().empty())
    {
        return;
    }

    if(!checkCommand(command))
    {
        FOR_MAP(it, mCommandsMap)
        {
            Command predictedCommand;

            if(it->first.get().find(command.getName().get()) != std::string::npos)
            {
                predictedCommand = it->second.mCommand;
                CHECK_MSG((predictedCommand.getName().get().size() + 1) < smBufferSize, "String size is greater than max buffer size.")
                clearBuffer();
                mBuffer = predictedCommand.getName().get();
                mBuffer += " ";
                mBufferDirty = true;
                break;
            }
        }
    }
}

void CommandLine::registerCommand(const std::string& commandName, CommandCallback callback)
{
    CHECK_MSG(!mCommandsMap.contains(commandName), "Command " + commandName + " already registered!");

    Command command;
    command.setName(commandName);

    CommandFunctor functor;
    functor.mCallback = callback;
    functor.mCommand = command;

    mCommandsMap.insert_or_assign(commandName, functor);
}

void CommandLine::open()
{
    subscribeToEvents();
    clearBuffer();
    mIsOpen = true;
    mBufferDirty = true;
    writeLine("CMD Opened");
    writeLine("Press [`] (grave accent) to toggle command line.");
}

void CommandLine::close()
{
    unsubscribeToEvents();
    clearBuffer();
    mIsOpen = false;
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

void CommandLine::subscribeToEvents()
{
    SUBSCRIBE_TO_EVENT(InputEventChar, nullptr, this, [this](const Event *event)
    {
        
        if(mIsOpen)
        {
            const InputEventChar *e = (const InputEventChar*) event;
            char c = e->mChar;
            
            if(c == '`') // ignore GLFW_KEY_GRAVE_ACCENT
            {
                return;
            }
            
            charReceived(c);

            // writeLine(mBuffer);
        }
    });

    SUBSCRIBE_TO_EVENT(InputEventKeyEnter, nullptr, this, [this](const Event *event)
    {
        if(mIsOpen)
        {
            execute();
            clearBuffer();
            mBufferDirty = true;

            // writeLine("", false);
        }
    });

    SUBSCRIBE_TO_EVENT(InputEventKeyTab, nullptr, this, [this](const Event *event)
    {
        if(mIsOpen)
        {
            autocomplete();
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
                mBufferDirty = true;
            }
            
            // writeLine(mBuffer, false);
        }
    });
}

void CommandLine::unsubscribeToEvents()
{
    UNSUBSCRIBE_TO_EVENT(InputEventChar, nullptr, this);
    UNSUBSCRIBE_TO_EVENT(InputEventKeyEnter, nullptr, this);
    UNSUBSCRIBE_TO_EVENT(InputEventKeyTab, nullptr, this);
    UNSUBSCRIBE_TO_EVENT(InputEventKeyBackspace, nullptr, this);
}
NS_END