#pragma once

#include "Core/Module.hpp"
#include <netinet/in.h>

#ifdef CPP_INCLUDE
#include "Core/Net/Server.hpp"
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#endif

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
	CPP void init(u16 port)
	{
		TRACE()

		mStatus = ServerStatus::UNINITIALIZED;

		// Creating socket file descriptor
		if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
		{
			perror("socket failed");
			exit(EXIT_FAILURE);
		}

		int flags = fcntl(server_fd, F_GETFL);

		if (flags < 0)
		{
			perror("could not get flags on TCP listening socket");
		}

		if (fcntl(server_fd, F_SETFL, flags | O_NONBLOCK) < 0)
		{
			perror("could not set TCP listening socket to be non-blocking");
		}

		// Forcefully attaching socket to the port 8080
		if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
					&opt, sizeof(opt)))
		{
			perror("setsockopt");
			exit(EXIT_FAILURE);
		}
		address.sin_family = AF_INET;
		address.sin_addr.s_addr = INADDR_ANY;
		address.sin_port = htons(port);

		// Forcefully attaching socket to the port 8080
		if (bind(server_fd, (struct sockaddr *)&address,
				sizeof(address)) < 0)
		{
			perror("bind failed");
			exit(EXIT_FAILURE);
		}
		if (listen(server_fd, 1) < 0)
		{
			perror("listen");
			exit(EXIT_FAILURE);
		}

		mStatus = ServerStatus::LISTENING;

		/*if ((new_socket)<0)
		{
			perror("accept");
			exit(EXIT_FAILURE);
		}*/
		/*valread = read( new_socket , buffer, 1024);
		printf("%s\n",buffer );
		send(new_socket , hello , strlen(hello) , 0 );
		printf("Hello message sent\n");*/
	}

	CPP bool checkConnectionRequest()
	{
		new_socket = accept(server_fd, (struct sockaddr *)&address,
							(socklen_t *)&addrlen);

		bool result = false;
		if (new_socket < 0)
		{
			if (errno == EWOULDBLOCK)
			{
				//perror("No pending connections.\n");
			}
			else
			{
				perror("error when accepting connection");
			}

			result = false;
		}
		else
		{
			perror("NEW Connection accepted.\n");
			mStatus = ServerStatus::CONNECTED;
			result = true;
		}

		return result;
	}

	CPP void writeData(const std::string& data) const
	{
		int sent = send(new_socket, data.c_str(), data.size(), 0);

		if (sent < 0)
		{
			perror("Error sending.\n");
		}
	}

	CPP std::string readData(u32 size = DEFAULT_SOCKET_READ_SIZE) const
	{
		buffer.clear();
		buffer.resize(size);
		int readed = read(new_socket, &buffer[0], size);

		if (readed < 0)
		{
			perror("Error sending.\n");
		}

		return buffer;
	}

	CPP JSON readJSON() const
	{
		JSON json = readSimpleJSON();

		if (json.contains("__size"))
		{
			json = readSimpleJSON(json["__size"]);
		}

		return json;
	}



	CPP void writeJSON(JSON& json) const
	{
	}

private:
	ServerStatus mStatus;
	
	int server_fd, new_socket;
	struct sockaddr_in address;
	int opt = 1;
	int addrlen = sizeof(address);

	mutable std::string buffer;

	CPP JSON readSimpleJSON(u32 size  = DEFAULT_SOCKET_READ_SIZE) const
	{
		JSON json;
		std::string data = readData(size);

		if (!data.empty())
		{
			ECHO(data);

			json = JSON::parse(data);

			if (!json.empty())
			{
				ECHO(json.dump());
			}
		}

		return json;
	}
	
public:
	GET(Status)
};