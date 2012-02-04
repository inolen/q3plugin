#ifndef SVCMD_H
#define SVCMD_H

#include <boost/asio/ip/udp.hpp>
#include <string>
#include <vector>
#include <map>

/**
 * svcmd core.
 */
namespace svcmd {

struct request {
	request(const std::string& cmd) {
		buf = std::string("\xff\xff\xff\xff").append(cmd);
	}

	std::string buf;
};

struct response {
	std::vector<unsigned char> buf;
};

void exec(const request& req, const std::string& addr, int port, response& res);

/**
 * Specialized commands.
 */
namespace cmds {

typedef boost::asio::ip::udp::endpoint     svaddr;
typedef std::map<std::string, std::string> svinfo;

void get_all_servers(const std::string& address, int port, std::vector<svaddr>& servers);
void get_server_info(const std::string& address, int port, svinfo& info);

}
}

#endif