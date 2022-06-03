#ifndef COMMANDLINE_HPP
#define COMMANDLINE_HPP

#include "Core/ObjectBase.hpp"
#include "Core/Singleton.hpp"
#include "Core/Functor.hpp"
#include "Core/Command/Command.hpp"

using CommandCallback = std::function<void(const Command& command)>;

class CommandFunctor: public Functor<CommandCallback>
{
	GENERATE_METADATA(CommandFunctor)

private:
	Command mCommand;

public:

	void execute() override
	{
		if (mCallback)
		{
			mCallback(mCommand);
		}
	}

	// NOTE : Override in children!
	COPY(CommandFunctor)
	{
		Functor<CommandCallback>::copy(other);
        DO_COPY(mCommand)
	}

	RGET_SET(Command)
};

class CommandLine: public ObjectBase, public Singleton<CommandLine>
{
	GENERATE_METADATA(CommandLine)

private:
	std::map<String, CommandFunctor> mCommandsMap;
	String mBuffer;

	std::list<String> mHistory;
	std::list<String>::iterator mHistoryIterator;

    bool mIsOpen = false;

    void log(const String& line, bool newLine = true) const;

public:
    void init();
	void update();
    void terminate();

    void execute(const String& commandLine);
    String autocomplete(const String& commandLine);
    void registerCommand(const String& commandName, CommandCallback callback);

	void open();
	void close();
    void toggle();

	CRGET(Buffer)
};

#endif