/*
 * OperatorConsole.cpp
 *
 *  Created on: Nov. 23, 2022
 *      Author: Matthew Faigan
 */

#include "OperatorConsole.h"
#include <iostream>
#include <atomic>
#include <sstream>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <errno.h>

#include "commandCodes.h"

pthread_mutex_t OperatorConsole::mutex = PTHREAD_MUTEX_INITIALIZER;
std::queue<OperatorConsoleResponseMessage> OperatorConsole::responseQueue;

OperatorConsole::OperatorConsole() :
		chid(-1) {
}

int OperatorConsole::getChid() const {
	return chid;
}

void OperatorConsole::run() {
	// Create a communication channel
	if ((chid = ChannelCreate(0)) == -1) {
		std::cout
				<< "Operator console: channel creation failed. Exiting thread."
				<< std::endl;
		return;
	}

	pthread_t cinReaderThread;
	std::atomic_bool cinReaderStopFlag;
	cinReaderStopFlag = false;
	pthread_create(&cinReaderThread, NULL, &OperatorConsole::cinRead,
			&cinReaderStopFlag);

	// Start listening for messages
	listen();

	cinReaderStopFlag = true;
	pthread_join(cinReaderThread, NULL);
}

void OperatorConsole::listen() {
	int rcvid;
	OperatorConsoleCommandMessage msg;
	while (1) {
		rcvid = MsgReceive(chid, &msg, sizeof(msg), NULL);
		switch (msg.systemCommandType) {
		case OPCON_CONSOLE_COMMAND_GET_USER_COMMAND: {
			pthread_mutex_lock(&mutex);
			if (responseQueue.empty()) {
				OperatorConsoleResponseMessage msg;
				msg.userCommandType = OPCON_USER_COMMAND_NO_COMMAND_AVAILABLE;
				MsgReply(rcvid, EOK, &msg, sizeof(msg));
			} else {
				OperatorConsoleResponseMessage msg = responseQueue.front();
				responseQueue.pop();
				MsgReply(rcvid, EOK, &msg, sizeof(msg));
			}
			pthread_mutex_unlock(&mutex);
			break;
		}
		case COMMAND_EXIT_THREAD:
			// Required to allow all threads to gracefully terminate when the program is terminating
			MsgReply(rcvid, EOK, NULL, 0);
			return;
		default:
			std::cout << "OperatorConsole received unknown command "
					<< msg.systemCommandType << std::endl;
			MsgError(rcvid, ENOSYS);
			break;
		}
	}
}

void* OperatorConsole::start(void *context) {
	auto c = (OperatorConsole*) context;
	c->run();
	return NULL;
}

void* OperatorConsole::cinRead(void *param) {
	// Get the flag we monitor to know when to stop reading
	std::atomic_bool *stop = (std::atomic_bool*) param;

	int fd = creat("/data/home/qnxuser/commandlog.txt", S_IRUSR | S_IWUSR | S_IXUSR);
	if (fd == -1) {
		std::cout << "Failed to open logfile. Errno is " << errno << std::endl;
	}

	std::string msg;
	while (!(*stop)) {
		// Get a command from cin and break it up by spaces
		std::getline(std::cin, msg);
		std::vector<std::string> tokens;
		tokenize(tokens, msg);

		if (tokens.size() == 0)
			continue;

		if (fd != -1) {
			// Create a char buffer with length equal to the message + 1 for the null terminator.
			char* buffer = new char[msg.length() + 1];
			// Copy the C++ string into the char buffer.
			strncpy(buffer, msg.c_str(), msg.length() + 1);
			// Write the command to the file with a newline.
			write(fd, buffer, msg.length() + 1);
			write(fd, "\n", 1);
			// Delete the buffer.
			delete[] buffer;
		}

		if (tokens[0] == OPCON_COMMAND_STRING_SHOW_PLANE) {
			if (tokens.size() < 2) {
				std::cout << "Error: must provide plane number" << std::endl;
				continue;
			}
			try {
				int planeNum = std::stoi(tokens[1]);
				OperatorConsoleResponseMessage res;
				res.userCommandType = OPCON_USER_COMMAND_DISPLAY_PLANE_INFO;
				res.planeNumber = planeNum;
				pthread_mutex_lock(&mutex);
				responseQueue.push(res);
				pthread_mutex_unlock(&mutex);
			} catch (std::invalid_argument &e) {
				std::cout << "Error: not a valid integer" << std::endl;
				continue;
			}
		} else if (tokens[0] == OPCON_COMMAND_STRING_SET_VELOCITY) {
			if (tokens.size() < 5) {
				std::cout
						<< "Error: must provide plane number and 3 velocity components (x,y,z)"
						<< std::endl;
				continue;
			}
			try {
				int planeNum = std::stoi(tokens[1]);
				int components[3];
				for (size_t i = 0; i < 3; i++) {
					components[i] = std::stoi(tokens[2 + i]);
				}
				Vec3 velocity { components[0], components[1], components[2] };
				OperatorConsoleResponseMessage res;
				res.userCommandType = OPCON_USER_COMMAND_SET_PLANE_VELOCITY;
				res.planeNumber = planeNum;
				res.newVelocity = velocity;
				pthread_mutex_lock(&mutex);
				responseQueue.push(res);
				pthread_mutex_unlock(&mutex);
			} catch (std::invalid_argument &e) {
				std::cout << "Error: not a valid integer" << std::endl;
				continue;
			}
		} else {
			std::cout << "Unknown command" << std::endl;
			continue;
		}
	}

	if (fd != -1) {
		close(fd);
	}
	return NULL;
}

void OperatorConsole::tokenize(std::vector<std::string> &dest,
		std::string &str) {
	std::stringstream ss(str);
	std::string token;
	while (std::getline(ss, token, ' ')) {
		dest.push_back(token);
	}
}
