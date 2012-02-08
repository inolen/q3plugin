#ifndef SERVERCOMMAND_H
#define SERVERCOMMAND_H

#include <boost/asio/ip/udp.hpp>
#include <string>
#include <vector>
#include <map>

class ServerCommandClient {
public:
	ServerCommandClient(const std::string& address, int port);

	void SendCommand(const std::string& command);
	void WaitForResponse(std::vector<unsigned char>& response, int timeout);

private:
	template <typename MutableBufferSequence>
	void ReadWithTimeout(const MutableBufferSequence& buffer, size_t& count, int seconds);

	boost::asio::io_service io_service_;
	boost::asio::ip::udp::endpoint receiver_endpoint_;
	boost::asio::ip::udp::socket socket_;
};

namespace ServerCommands {
	typedef boost::asio::ip::udp::endpoint     ServerAddress;
	typedef std::vector<ServerAddress>         ServerAddressList;
	typedef std::map<std::string, std::string> ServerInfo;

	void GetAllServers(const std::string& address, int port, ServerAddressList& servers);
	void GetServerInfo(const std::string& address, int port, ServerInfo& info);
}

#endif