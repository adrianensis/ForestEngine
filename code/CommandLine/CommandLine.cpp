#include "CommandLine/CommandLine.hpp"

#include "Core/Assert/Assert.hpp"
#include "CommandLine/DefaultCommands.hpp"
#include "Input/InputEvents.hpp"
#include "Core/Log/Log.hpp"
#include "Core/Event/EventsManager.hpp"
#include "GLFW/glfw3.h"

#include <cstdio>
#include <regex>
#include <string>
#include <unistd.h>

NS_BEGIN(Command)
void CommandLine::init()
{
    mBuffer.reserve(smBufferSize);
    mIsOpen = false;

    mSystemsDI.getSystem<Event::EventsManager>()->subscribe<Input::InputEventKeyReleased>(nullptr, this, [this](const Event::Event *event)
	{
        const Input::InputEventKeyReleased *e = (const Input::InputEventKeyReleased *)event;
        if(e->mKey == GLFW_KEY_GRAVE_ACCENT)
        {
            toggle();
        }
	});

	mSystemsDI.getSystem<Event::EventsManager>()->subscribe<Input::InputEventKeyReleased>(nullptr, this, [this](const Event::Event *event)
	{
        const Input::InputEventKeyReleased *e = (const Input::InputEventKeyReleased *)event;
        if(e->mKey == GLFW_KEY_GRAVE_ACCENT)
        {
            toggle();
        }
	});

    // SUBSCRIBE_TO_EVENT(Input::InputEventKeyArrow, nullptr, this, [this](const Event *event)
    // {
    //     if(mIsOpen)
    //     {
    //         // NEXT: GLFW_KEY_UP && DOWN creates a dependency with Graphics/Window module 

    //         // const Input::InputEventKeyArrow *e = (const Input::InputEventKeyArrow*) event;

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

    DefaultCommands::registerDefaultCommands(*this);
}

void CommandLine::writeLine(const std::string& line, bool newLine /*= true*/) const
{
    if(mPrintToConsole)
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
}

void CommandLine::update()
{
    flush();
}

void CommandLine::flush()
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
    writeLine(mBuffer);
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
    mSystemsDI.getSystem<Event::EventsManager>()->subscribe<Input::InputEventChar>(nullptr, this, [this](const Event::Event *event)
    {
        
        if(mIsOpen)
        {
            const Input::InputEventChar *e = (const Input::InputEventChar*) event;
            char c = e->mChar;
            
            if(c == '`') // ignore GLFW_KEY_GRAVE_ACCENT
            {
                return;
            }
            
            charReceived(c);
        }
    });

    mSystemsDI.getSystem<Event::EventsManager>()->subscribe<Input::InputEventKeyEnter>(nullptr, this, [this](const Event::Event *event)
    {
        if(mIsOpen)
        {
            execute();
            clearBuffer();
            mBufferDirty = true;
        }
    });

    mSystemsDI.getSystem<Event::EventsManager>()->subscribe<Input::InputEventKeyTab>(nullptr, this, [this](const Event::Event *event)
    {
        if(mIsOpen)
        {
            autocomplete();
        }
    });

    mSystemsDI.getSystem<Event::EventsManager>()->subscribe<Input::InputEventKeyBackspace>(nullptr, this, [this](const Event::Event *event)
    {
        if(mIsOpen)
        {
            if(!mBuffer.empty())
            {
                LOG_BACKSPACE()
                mBuffer.pop_back();
                mBufferDirty = true;
            }
        }
    });
}

void CommandLine::unsubscribeToEvents()
{
    mSystemsDI.getSystem<Event::EventsManager>()->unsubscribe<Input::InputEventChar>(nullptr, this);
    mSystemsDI.getSystem<Event::EventsManager>()->unsubscribe<Input::InputEventKeyEnter>(nullptr, this);
    mSystemsDI.getSystem<Event::EventsManager>()->unsubscribe<Input::InputEventKeyTab>(nullptr, this);
    mSystemsDI.getSystem<Event::EventsManager>()->unsubscribe<Input::InputEventKeyBackspace>(nullptr, this);
}
NS_END