#pragma once

#include "Core/ObjectBase.hpp"
#include "Engine/EngineSystem.hpp"
#include "Core/Functor.hpp"
#include "Core/Command/Command.hpp"

using CommandCallback = std::function<void(const Command& command)>;

class CommandFunctor: public Functor<CommandCallback>
{
	GENERATE_METADATA(CommandFunctor)
    
public:

	void execute() override
	{
		if (mCallback)
		{
			mCallback(mCommand);
		}
	}

    Command mCommand;
};

class CommandLine: public EngineSystem
{
	GENERATE_METADATA(CommandLine)

private:
	std::unordered_map<std::string, CommandFunctor> mCommandsMap;
	std::string mBuffer;

	std::list<std::string> mHistory;
	std::list<std::string>::iterator mHistoryIterator;

    bool mIsOpen = false;

    void writeLine(const std::string& line, bool newLine = true) const;

public:
    void init();
	void update();
    void terminate();

    void execute(const std::string& commandLine);
    std::string autocomplete(const std::string& commandLine);
    void registerCommand(const std::string& commandName, CommandCallback callback);

	void open();
	void close();
    void toggle();

	CRGET(Buffer)
};
