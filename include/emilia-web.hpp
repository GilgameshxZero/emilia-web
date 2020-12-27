#pragma once

#define EMILIA_WEB_VERSION_MAJOR 7
#define EMILIA_WEB_VERSION_MINOR 1
#define EMILIA_WEB_VERSION_REVISION 0

#include "build.hpp"

#include <rain.hpp>

class Server;

class ServerSlave
		: public Rain::Networking::Http::ServerSlave<Server, ServerSlave, void *> {
	public:
	typedef Rain::Networking::Http::ServerSlave<Server, ServerSlave, void *>
		ServerSlaveBase;
	ServerSlave(Rain::Networking::Socket &socket, Server *server)
			: Rain::Networking::Socket(std::move(socket)),
				ServerSlaveBase(socket, server) {}
};

class Server : public Rain::Networking::Http::Server<ServerSlave> {
	public:
	typedef Rain::Networking::Http::Server<ServerSlave> ServerBase;
	Server(std::size_t maxThreads = 0, std::size_t slaveBufSz = 16384)
			: ServerBase(maxThreads, slaveBufSz) {}

	protected:
	void *getSubclassPtr() { return reinterpret_cast<void *>(this); }
	void onRequest(Request *);
};

int main(int, const char *[]);
