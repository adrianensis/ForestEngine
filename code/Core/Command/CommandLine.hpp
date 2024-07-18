#pragma once

#include "Core/ECS/System.hpp"
#include "Core/Command/Command.hpp"

using CommandCallback = std::function<void(const Command& command)>;

class CommandFunctor: public Functor<CommandCallback>
{
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

class CommandLine: public System
{
	

private:
	std::unordered_map<std::string, CommandFunctor> mCommandsMap;
	std::string mBuffer;

	std::list<std::string> mHistory;
	std::list<std::string>::iterator mHistoryIterator;

    bool mIsOpen = false;

    void writeLine(const std::string& line, bool newLine = true) const;

public:
    virtual void init() override;
    virtual void terminate() override;
	virtual void update() override;

    void execute(const std::string& commandLine);
    std::string autocomplete(const std::string& commandLine);
    void registerCommand(const std::string& commandName, CommandCallback callback);

	void open();
	void close();
    void toggle();

	CRGET(Buffer)
};
REGISTER_CLASS(CommandLine);
