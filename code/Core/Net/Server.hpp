#ifndef SERVER_HPP
#define SERVER_HPP

#include "Core/Module.hpp"
#include <netinet/in.h>

enum class ServerStatus
{
	UNINITIALIZED,
	LISTENING,
	CONNECTED
};

#define DEFAULT_SOCKET_READ_SIZE 1024

class Server: public ObjectBase
{
    GENERATE_METADATA(Server)

public:
    void init(u16 port);
    bool checkConnectionRequest();
    void writeData(const std::string& data) const;
    std::string readData(u32 size = DEFAULT_SOCKET_READ_SIZE) const;
    JSON readJSON() const;
    void writeJSON(JSON& json) const;

private:
    JSON readSimpleJSON(u32 size  = DEFAULT_SOCKET_READ_SIZE) const;

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

#endif