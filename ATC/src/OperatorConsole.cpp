/*
 * OperatorConsole.cpp
 *
 *  Created on: Nov. 23, 2022
 *      Author: Matthew Faigan
 */

#include "OperatorConsole.h"

OperatorConsole::OperatorConsole()
	: chid(-1)
{}

void OperatorConsole::run()
{
	// Create a communication channel
	if ((chid = ChannelCreate(0)) == -1)
	{
		std::cout << "Operator console: channel creation failed. Exiting thread." << std::endl;
		return;
	}

	// Open a client to our own connection to be used for timer pulses and store the handle in coid.
	int coid;
	if ((coid = ConnectAttach(0,0,chid,0,0)) == -1)
	{
		std::cout << "Operator console: failed to attach to self. Exiting thread.";
		return;
	}

	// Initialize a sigevent to send a pulse.
	struct sigevent sigev;
	SIGEV_PULSE_INIT(&sigev, coid, SIGEV_PULSE_PRIO_INHERIT, OPCON_CODE_TIMER, 0);

	timer_t updateTimer;
	if (timer_create(CLOCK_MONOTONIC, &sigev, &updateTimer) == -1)
	{
		std::cout << "Operator console: failed to initialize update timer. Exiting thread.";
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

	// Start listening for messages
	listen();
}

void OperatorConsole::listen()
{
	int rcvid;
	OperatorConsoleCommandMessage msg;
	while (1)
	{
		rcvid = MsgReceive(chid, &msg, sizeof(msg), NULL);
		if (rcvid == 0)
		{
			// Received a pulse
			switch (msg.header.code)
			{
			case OPCON_CODE_TIMER:

			}
		}
	}
}

void* OperatorConsole::start(void* context)
{
	auto c = (OperatorConsole*)context;
	c->run();
	return NULL;
}
