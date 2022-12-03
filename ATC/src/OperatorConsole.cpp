/*
 * OperatorConsole.cpp
 *
 *  Created on: Nov. 23, 2022
 *      Author: Matthew Faigan
 */

#include "OperatorConsole.h"
#include <string>
#include <iostream>
#include <atomic>

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

	// Open a client to our own connection to be used for timer pulses and store the handle in coid.
	int coid;
	if ((coid = ConnectAttach(0, 0, chid, 0, 0)) == -1) {
		std::cout
				<< "Operator console: failed to attach to self. Exiting thread.";
		return;
	}

	// Initialize a sigevent to send a pulse.
	struct sigevent sigev;
	SIGEV_PULSE_INIT(&sigev, coid, SIGEV_PULSE_PRIO_INHERIT, OPCON_CODE_TIMER,
			0);

	timer_t updateTimer;
	if (timer_create(CLOCK_MONOTONIC, &sigev, &updateTimer) == -1) {
		std::cout
				<< "Operator console: failed to initialize update timer. Exiting thread.";
		return;
	}

	// Set the timer to check for new user commands every second.
	struct itimerspec timerValue;
	timerValue.it_value.tv_sec = 1;
	timerValue.it_value.tv_nsec = 0;
	timerValue.it_interval.tv_sec = 1;
	timerValue.it_interval.tv_nsec = 0;

	// Start the timer.
	timer_settime(updateTimer, 0, &timerValue, NULL);

	pthread_t cinReaderThread;
	std::atomic_bool cinReaderStopFlag;
	cinReaderStopFlag = false;
	pthread_create(&cinReaderThread, NULL, &OperatorConsole::cinRead, &cinReaderStopFlag);

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
		switch(msg.systemCommandType)
		{
		case OPCON_CONSOLE_COMMAND_GET_USER_COMMAND:
			MsgReply(rcvid, EOK, NULL, 0); // TODO check head of queue, return if present
			break;
		case OPCON_CONSOLE_COMMAND_EXIT_THREAD:
			// Required to allow all threads to gracefully terminate when the program is terminating
			MsgReply(rcvid, EOK, NULL, 0);
			return;
		}
	}
}

void* OperatorConsole::start(void *context) {
	auto c = (OperatorConsole*) context;
	c->run();
	return NULL;
}

void* OperatorConsole::cinRead(void* param) {
	// TODO parse command, store in queue
	std::atomic_bool *stop = (std::atomic_bool *)param;
	while (!(*stop)) {
		std::string msg;
		std::getline(std::cin, msg);
		std::cout << "Msg was: " << msg << std::endl;
		std::cout << "eof is: " << std::cin.eof() << std::endl;
		std::cout << "fail is: " << std::cin.fail() << std::endl;
		std::cout << "good is: " << std::cin.good() << std::endl;
	}
	return NULL;
}
