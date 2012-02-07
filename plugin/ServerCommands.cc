#include "ServerCommands.h"
#include <iostream>
#include <boost/algorithm/string.hpp>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/optional.hpp>

using boost::asio::ip::udp;

#define	MAX_MSGLEN        16384

/**
 * Helper functions for boost.
 */
void set_result(boost::optional<boost::system::error_code>* destination,
	boost::system::error_code source
) {
	destination->reset( source );
}

void set_bytes_result(boost::optional<boost::system::error_code>* error_destination,
	size_t *transferred_destination,
	boost::system::error_code error_source,
	size_t transferred_source
) {
	error_destination->reset( error_source );
	*transferred_destination = transferred_source;
}

/**
 * Simple datagram send/recv helper.
 */
ServerCommandClient::ServerCommandClient(const std::string& address, int port)
	: socket_(io_service_) {
	// Convert port to string.
	std::ostringstream portstr;
	portstr << port;

	// Resolve address.
	udp::resolver resolver(io_service_);
	udp::resolver::query query(udp::v4(), address, portstr.str());
	receiver_endpoint_ = *resolver.resolve(query);

	// Open socket.
	socket_.open(udp::v4());
}

void ServerCommandClient::SendCommand(const std::string &command) {
	socket_.send_to(boost::asio::buffer(command), receiver_endpoint_);
}

void ServerCommandClient::WaitForResponse(std::vector<unsigned char>& response, int timeout) {
	udp::endpoint sender_endpoint;
	boost::array<unsigned char, MAX_MSGLEN> recv_buf;
	size_t recv_len;

	ReadWithTimeout(boost::asio::buffer(recv_buf), recv_len, timeout);

	response.insert(response.begin(), recv_buf.begin(), recv_buf.begin() + recv_len);
}

template <typename MutableBufferSequence>
size_t ServerCommandClient::ReadWithTimeout(const MutableBufferSequence& buffer, size_t& count, int seconds) {
	boost::optional<boost::system::error_code> timer_result;
	boost::optional<boost::system::error_code> read_result;

	// Set up a timer on the io_service for this socket.
	boost::asio::deadline_timer timer(io_service_);
	timer.expires_from_now(boost::posix_time::seconds(1));
	timer.async_wait(boost::bind(set_result, &timer_result, boost::asio::placeholders::error));

	// Set up async read.
	udp::endpoint sender_endpoint;

	socket_.async_receive_from(buffer, sender_endpoint,
		boost::bind(set_bytes_result,
			&read_result,
			&count,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred
		));

	// Read until we get something.
	io_service_.reset();

	while (io_service_.run_one()) {
		if (read_result) {
			timer.cancel();
		}
		else if (timer_result) {
			socket_.cancel();
		}
	}

	// Throw any resulting errors.
	if (*read_result) {
		throw boost::system::system_error(*read_result);
	}
}

/**
 * Specialized commands.
 */
void ServerCommands::GetAllServers(const std::string &address, int port, ServerAddressList &servers) {
	// Send the command and grab the response.
	ServerCommandClient client(address, port);
	std::string cmd("\xff\xff\xff\xffgetservers Quake3Arena 68 empty full");
	std::vector<unsigned char> res;

	client.SendCommand(cmd);
	client.WaitForResponse(res, 1);

	// Parse the response. Start by skipping the response header.
	int i = 18;
	char c;

	while (i < res.size()) {
		c = res[i++];

		// If we have enough data to read.
		if (c == '\\' && res.size() - i >= 6) {
			boost::asio::ip::address_v4 addr(ntohl(*(unsigned long *)&res[i]));
			unsigned short port = ntohs(*(unsigned short *)&res[i+4]);

			servers.push_back(ServerAddress(addr, port));

			i += 6;
		}
	};
}

void ServerCommands::GetServerInfo(const std::string &address, int port, ServerInfo &info) {
	// Send the command and grab the response.
	ServerCommandClient client(address, port);
	std::string cmd("\xff\xff\xff\xffgetinfo xxx");
	std::vector<unsigned char> res;

	client.SendCommand(cmd);
	client.WaitForResponse(res, 1);

	// Copy the response into a string (skipping the header).
	std::string buf(res.begin() + 18, res.end());

	// Split by backslashes.
	std::vector<std::string> split;
	boost::split(split, buf, boost::is_any_of("\\"));

	for (int i = 0; i < split.size(); i += 2) {
		std::string key = split[i];
		std::string val = split[i+1];
		info[key] = val;
	}
}