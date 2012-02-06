#ifndef SERVERCOMMAND_H
#define SERVERCOMMAND_H

#include <boost/asio/ip/udp.hpp>
#include <string>
#include <vector>
#include <map>

class ServerCommand {
public:
	typedef boost::asio::ip::udp::endpoint     ServerAddress;
	typedef std::map<std::string, std::string> ServerInfo;

	static void GetAllServers(const std::string& address, int port, std::vector<ServerAddress>& servers);
	static void GetServerInfo(const std::string& address, int port, ServerInfo& info);

private:
	struct CommandRequest {
		CommandRequest(const std::string& cmd) {
			buf = std::string("\xff\xff\xff\xff").append(cmd);
		}

		std::string buf;
	};

	struct CommandResponse {
		std::vector<unsigned char> buf;
	};

	static void Execute(const CommandRequest& req, const std::string& addr, int port, CommandResponse& res);
};

#endif