#pragma once

#define EMILIA_WEB_VERSION_MAJOR 7
#define EMILIA_WEB_VERSION_MINOR 1
#define EMILIA_WEB_VERSION_REVISION 3

#include "build.hpp"

#include <rain.hpp>

class Server
		: public Rain::Networking::Http::Server<Rain::Networking::Http::Slave> {
	protected:
	bool onRequest(Slave &slave, Request &req) noexcept override;
};

int main(int, const char *[]);
