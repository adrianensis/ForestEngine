#pragma once

#include "Core/CoreBase.hpp"
#include "Core/Memory/Pointers.hpp"
#include "Core/Metadata/ClassManager.hpp"
#include "Core/JSON/JSON.hpp"
#include <netinet/in.h>
NS_BEGIN(Core)
enum class ServerStatus
{
	UNINITIALIZED,
	LISTENING,
	CONNECTED
};

#define DEFAULT_SOCKET_READ_SIZE 1024

class Server
{
public:
    void init(Core::u16 port);
    bool checkConnectionRequest();
    void writeData(const std::string& data) const;
    std::string readData(Core::u32 size = DEFAULT_SOCKET_READ_SIZE) const;
    Core::JSON readJSON() const;
    void writeJSON(Core::JSON& json) const;

private:
    Core::JSON readSimpleJSON(Core::u32 size  = DEFAULT_SOCKET_READ_SIZE) const;

private:
	ServerStatus mStatus;
	
	int server_fd, new_socket;
	struct sockaddr_in address;
	int opt = 1;
	int addrlen = sizeof(address);

	mutable std::string buffer;
	
public:
	GET(Status)
};
NS_END