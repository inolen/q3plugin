#include "msgpipe.h"
#include <iostream>
#include <sstream>
#include <errno.h>
#include <fcntl.h>

extern int errno;

namespace msgpipe {

/**
 * Half-duplex pipe implementation.
 */
bool hdxpipe::open(const std::string& name, bool readit) {
	if (name.empty()) {
		return false;
	}

	name_ = name;
	buflen_ = 0;

	// Create the pipe
	errno = 0;
	if (mkfifo(name_.c_str(), 0666) < 0 && errno != EEXIST) {
		return false;
	}

	// Open the pipe.
	if (readit) {
		if ((fd_ = ::open(name_.c_str(), O_RDONLY)) < 0) {
			return false;
		}

		// After we've successfully opened, set to non-blocking.
		fcntl(fd_, F_SETFL, O_RDONLY | O_NONBLOCK);
	}
	else {
		if ((fd_ = ::open(name_.c_str(), O_WRONLY)) < 0) {
			return false;
		}
	}

	return true;
}

int hdxpipe::send(const message& msg) {
	int num;

	if ((num = write(fd_, &msg, sizeof(msg)) < 0)) {
		return -1;
	}

	return num;
}

void hdxpipe::pump() {
	int num;

	// We don't want to read more than our current buffer can hold.
	char* buf = buf_ + buflen_;
	int length = sizeof(char) * sizeof(buf_) - buflen_;

	if ((num = read(fd_, buf, length)) == -1 && errno != EAGAIN) {
		return;
	} else if (num > 0) {
		// Update our current buffer size on a successful read.
		buflen_ += num;
	}
}

bool hdxpipe::poll(message& msg) {
	// Pump data in from the pipe.
	pump();

	if (buflen_ < sizeof(msg)) {
		return false;
	}

	// Copy message from buffer to input param.
	memcpy(&msg, buf_, sizeof(msg));

	// Remove message from buffer.
	buflen_ -= sizeof(msg);
	memcpy(buf_, buf_ + sizeof(msg), buflen_);

	return true;
}

void hdxpipe::close() {
	::close(fd_);
}

/**
 * Full-duplex pipe implementation.
 */
bool fdxpipe::open(const std::string& name, bool reverse) {
	std::ostringstream namer;
	std::ostringstream namew;

	namer << name << reverse;
	namew << name << !reverse;

	if ((!reverse && (!piper_.open(namer.str(), true)  || !pipew_.open(namew.str(), false))) ||
		(reverse  && (!pipew_.open(namew.str(), false) || !piper_.open(namer.str(), true)))) {
		close();
		return false;
	}

	return true;
}

int fdxpipe::send(const message& msg) {
	return pipew_.send(msg);
}

bool fdxpipe::poll(message& msg) {
	return piper_.poll(msg);
}

void fdxpipe::close() {
	piper_.close();
	pipew_.close();
}

}