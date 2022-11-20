/*
 * Plane.cpp
 *
 *  Created on: Oct. 24, 2022
 *      Author: Matthew Faigan
 */

#include "Plane.h"

#include <time.h>
#include <sys/siginfo.h>
#include <signal.h>
#include <sys/neutrino.h>
#include <iostream>

Plane::Plane(PlaneStartParams &params)
	: startParams(params), currentPosition{-1,-1,-1}, currentVelocity{1,-1,-1}, chid(-1)
{}

int Plane::getChid() const
{
	return chid;
}

void Plane::run()
{
	// Create a new communication channel belonging to the plane and store the handle in chid.
	if ((chid = ChannelCreate(0)) == -1)
	{
		std::cout << "Plane " << startParams.id << ": channel creation failed. Exiting thread." << std::endl;
		return;
	}

	// Open a client to our own connection to be used for timer pulses and store the handle in coid.
	int coid;
	if ((coid = ConnectAttach(0,0,chid,0,0)) == -1)
	{
		std::cout << "Plane " << startParams.id << ": failed to attach to self. Exiting thread.";
		return;
	}

	// Initialize a sigevent to send a pulse.
	struct sigevent sigev;
	SIGEV_PULSE_INIT(&sigev, coid, SIGEV_PULSE_PRIO_INHERIT, CODE_TIMER, 0);

	timer_t updateTimer;
	if ((updateTimer = timer_create(CLOCK_MONOTONIC, &sigev, &updateTimer)) == -1)
	{
		std::cout << "Plane " << startParams.id << ": failed to initialize update timer. Exiting thread.";
		return;
	}

	// Set the timer to fire once at the arrival time, and then every second thereafter.
	struct itimerspec timerValue;
	timerValue.it_value.tv_sec = startParams.arrivalTime;
	timerValue.it_value.tv_nsec = 0;
	timerValue.it_interval.tv_sec = 1;
	timerValue.it_interval.tv_nsec = 0;

	// Start the timer.
	timer_settime(updateTimer, 0, &timerValue, NULL);

	int rcvid;
	PlaneCommandMessage msg;
	while (1)
	{
		// Wait for any type of message.
		rcvid = MsgReceive(chid, &msg, sizeof(msg), NULL);

		if (rcvid == 0)
		{
			// We've received a pulse.
			switch (msg.header.code)
			{
			case CODE_TIMER:
				// TODO implement periodic update
				break;
			default:
				break;
			}
		}
		else
		{
			// We've received a user message.
			switch (msg.command)
			{
			case COMMAND_RADAR_PING:
				// TODO reply with radar stuff
				break;
			case COMMAND_EXIT_THREAD:
				// Required to allow all threads to gracefully terminate when the program is terminating
				return;
			}
		}
	}
}

void* Plane::start(void *context)
{
	auto p = (Plane*)context;
	p->run();
	return NULL;
}
