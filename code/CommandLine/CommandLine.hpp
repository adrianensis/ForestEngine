#pragma once

#include "Core/HashedString/HashedString.hpp"
#include "Core/System/System.hpp"
#include "CommandLine/Command.hpp"

NS_BEGIN(Command)

using CommandCallback = std::function<void(const Command& command)>;

class CommandFunctor: public Core::Functor<CommandCallback>
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

class CommandLine: public System::System
{
public:
	virtual void init() override;
	virtual void terminate() override;
	void update();

	void registerCommand(const std::string& commandName, CommandCallback callback);
	
	void open();
	void close();
	void toggle();
	void writeLine(const std::string& line, bool newLine = true) const;
	
private:
	void subscribeToEvents();
	void unsubscribeToEvents();
	void charReceived(char c);
	void clearBuffer();
	Command extractCommand(const std::string& commandLine) const;
	void execute();
	void autocomplete();
	bool checkCommand(const Command& command) const;
	void flush();

private:
	std::unordered_map<Core::HashedString, CommandFunctor> mCommandsMap;
	std::string mBuffer;
	static const Core::u32 smBufferSize = 1024;

	std::list<std::string> mHistory;
	std::list<std::string>::iterator mHistoryIterator;

    bool mIsOpen = false;
    bool mBufferDirty = false;

    std::string mPatternValidName = "[-+]?[a-zA-Z_\\.0-9]+";

	bool mPrintToConsole = true;


public:
	CRGET(Buffer)
	GET_SET(PrintToConsole)
};
REGISTER_CLASS(CommandLine);
NS_END