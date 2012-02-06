#include "MessagePipe.h"
#include <boost/interprocess/ipc/message_queue.hpp>

namespace ipc = boost::interprocess;

MessagePipe::MessagePipe(const std::string& name, bool create) {
	std::string read_name(name), write_name(name);

	read_name.append(create ? "a" : "b");
	write_name.append(!create ? "a" : "b");

	if (create) {
		ipc::message_queue::remove(read_name.c_str());
		ipc::message_queue::remove(write_name.c_str());

		read_queue_ = new ipc::message_queue(ipc::create_only, read_name.c_str(), 128, sizeof(message_t));
		write_queue_ = new ipc::message_queue(ipc::create_only, write_name.c_str(), 128, sizeof(message_t));
	}
	else {
		read_queue_ = new ipc::message_queue(ipc::open_only, read_name.c_str());
		write_queue_ = new ipc::message_queue(ipc::open_only, write_name.c_str());
	}
}

MessagePipe::~MessagePipe() {
	delete read_queue_;
	delete write_queue_;
}

bool MessagePipe::Send(message_t& msg) {
	try {
		write_queue_->send(&msg, sizeof(msg), 0);
		return true;
	}
	catch (ipc::interprocess_exception& ex) {
		return false;
	}
}

bool MessagePipe::Poll(message_t& msg) {
	size_t recv_len;
	unsigned int priority;

	try {
		return read_queue_->try_receive(&msg, sizeof(msg), recv_len, priority);
	}
	catch (ipc::interprocess_exception& ex) {
		return false;
	}
}