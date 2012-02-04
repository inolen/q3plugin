#include "svcmd.h"
#include <iostream>
#include <boost/algorithm/string.hpp>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/optional.hpp>

#define	MAX_MSGLEN        16384

using boost::asio::ip::udp;

/**
 * Helper functions for boost.
 */
void set_result(boost::optional<boost::system::error_code>* destination,
	boost::system::error_code source
) {
	destination->reset( source );
}

void set_bytes_result(boost::optional<boost::system::error_code>* error_destination,
	size_t* transferred_destination,
	boost::system::error_code error_source,
	size_t transferred_source
) {
	error_destination->reset( error_source );
	*transferred_destination = transferred_source;
}

template <typename MutableBufferSequence>
size_t read_with_timeout(udp::socket& socket, const MutableBufferSequence& buffer, size_t& count, int seconds) {
	boost::asio::io_service& io_service = socket.get_io_service();
	boost::optional<boost::system::error_code> timer_result;
	boost::optional<boost::system::error_code> read_result;

	// Set up a timer on the io_service for this socket.
	boost::asio::deadline_timer timer(io_service);
	timer.expires_from_now(boost::posix_time::seconds(1));
	timer.async_wait(boost::bind(set_result, &timer_result, boost::asio::placeholders::error));

	// Set up async read.
	udp::endpoint sender_endpoint;

	socket.async_receive_from(buffer, sender_endpoint,
		boost::bind(set_bytes_result,
			&read_result,
			&count,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred
		));

	// Read until we get something.
	io_service.reset();

	while (io_service.run_one()) {
		if (read_result) {
			timer.cancel();
		}
		else if (timer_result) {
			socket.cancel();
		}
	}

	// Throw any resulting errors.
	if (*read_result) {
		throw boost::system::system_error(*read_result);
	}
}

/**
 * Core method to send a UDP datagram request and receive a response.
 */
namespace svcmd {

void exec(const request& req, const std::string& addr, int port, response& res) {
	// Convert port to string.
	std::ostringstream portstr;
	portstr << port;

	// Resolve address.
	boost::asio::io_service io_service;
	udp::resolver resolver(io_service);
	udp::resolver::query query(udp::v4(), addr, portstr.str());
	udp::endpoint receiver_endpoint = *resolver.resolve(query);

	// Create socket.
	udp::socket socket(io_service);
	socket.open(udp::v4());

	// Send command.
	socket.send_to(boost::asio::buffer(req.buf), receiver_endpoint);

	// Get response.
	size_t recv_len;
	boost::array<unsigned char, MAX_MSGLEN> recv_buf;

	read_with_timeout(socket, boost::asio::buffer(recv_buf), recv_len, 1);

	// Since we're nice, move this into a vector as to not store a tuple of buf/len.
	res.buf.insert(res.buf.begin(), recv_buf.begin(), recv_buf.begin() + recv_len);
}

/**
 * Specialized commands.
 */
namespace cmds {

void get_all_servers(const std::string& address, int port, std::vector<svaddr>& servers) {
	// Send the command and grab the response.
	svcmd::request req("getservers Quake3Arena 68 empty full");
	svcmd::response resp;

	svcmd::exec(req, address, port, resp);

	// Parse the response. Start by skipping the response header.
	int i = 18;
	char c;

	while (i < resp.buf.size()) {
		c = resp.buf[i++];

		// If we have enough data to read.
		if (c == '\\' && resp.buf.size() - i >= 6) {
			boost::asio::ip::address_v4 addr(ntohl(*(unsigned long *)&resp.buf[i]));
			unsigned short port = ntohs(*(unsigned short *)&resp.buf[i+4]);

			servers.push_back(svaddr(addr, port));

			i += 6;
		}
	};
}

void get_server_info(const std::string& address, int port, svinfo& info) {
	// Send the command and grab the response.
	svcmd::request req("getinfo xxx");
	svcmd::response resp;

	svcmd::exec(req, address, port, resp);

	// Copy the response into a string (skipping the header).
	std::string buf(resp.buf.begin() + 18, resp.buf.end());

	// Split by backslashes.
	std::vector<std::string> split;
	boost::split(split, buf, boost::is_any_of("\\"));

	for (int i = 0; i < split.size(); i += 2) {
		std::string key = split[i];
		std::string val = split[i+1];
		info[key] = val;
	}
}

}
}