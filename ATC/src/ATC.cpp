#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/dispatch.h>
#include <pthread.h>
#include <chrono>
#include <thread>

#include "ComputerSystem.h"
#include "Radar.h"

#include <iostream>
#include <time.h>
#include "Plane.h"
#include "OperatorConsole.h"
#include "DataDisplay.h"
#include "CommunicationSystem.h"

int64_t now() {
	struct timespec now;
	clock_gettime(CLOCK_MONOTONIC, &now);
	return now.tv_sec * 1000 * 1000 * 1000 + now.tv_nsec;
}

void planeDemo() {
	// Create an example plane and start the thread.
	PlaneStartParams params { 1, // id
			5, // arrival time
			{ 1, 1, 1 }, // initial position
			{ 1, 1, 1 } // initial velocity
	};
	Plane myPlane(params);

	pthread_t tid;
	pthread_create(&tid, NULL, &Plane::start, &myPlane);

	// Connect to plane's message passing channel.
	int coid = ConnectAttach(0, 0, myPlane.getChid(), _NTO_SIDE_CHANNEL, 0);

	// Ping the plane once per second for 15 seconds.
	int64_t sleepUntil;
	for (int i = 0; i < 15; i++) {
		PlaneCommandMessage msg;
		msg.command = COMMAND_RADAR_PING;
		PlanePositionResponse res;
		MsgSend(coid, &msg, sizeof(msg), &res, sizeof(res));
		std::cout << "Position: <" << res.currentPosition.x << ','
				<< res.currentPosition.y << ',' << res.currentPosition.z
				<< ">, ";
		std::cout << "Velocity: <" << res.currentVelocity.x << ','
				<< res.currentVelocity.y << ',' << res.currentVelocity.z << ">";
		std::cout << std::endl;

		sleepUntil = now() + 1000 * 1000 * 1000;
		while (now() < sleepUntil)
			;
	}

	// Change the plane's velocity.
	PlaneCommandMessage changeMsg;
	changeMsg.command = COMMAND_SET_VELOCITY;
	changeMsg.newVelocity = { 2, 3, -1 };
	MsgSend(coid, &changeMsg, sizeof(changeMsg), NULL, 0);

	// Ping the plane for 15 seconds again.
	for (int i = 0; i < 15; i++) {
		PlaneCommandMessage msg;
		msg.command = COMMAND_RADAR_PING;
		PlanePositionResponse res;
		MsgSend(coid, &msg, sizeof(msg), &res, sizeof(res));
		std::cout << "Position: <" << res.currentPosition.x << ','
				<< res.currentPosition.y << ',' << res.currentPosition.z
				<< ">, ";
		std::cout << "Velocity: <" << res.currentVelocity.x << ','
				<< res.currentVelocity.y << ',' << res.currentVelocity.z << ">";
		std::cout << std::endl;

		sleepUntil = now() + 1000 * 1000 * 1000;
		while (now() < sleepUntil)
			;
	}

	// Tell the plane to exit.
	PlaneCommandMessage msg;
	msg.command = COMMAND_EXIT_THREAD;
	MsgSend(coid, &msg, sizeof(msg), NULL, 0);
	pthread_join(tid, NULL);
}

void OperatorConsoleDemo() {
	OperatorConsole oc;
	pthread_t tid;
	pthread_create(&tid, NULL, &OperatorConsole::start, &oc);

	while (oc.getChid() == -1)
		;
	int coid = ConnectAttach(0, 0, oc.getChid(), _NTO_SIDE_CHANNEL, 0);

	int64_t sleepUntil;
	for (int i = 0; i < 3; i++) {
		sleepUntil = now() + 10L * 1000L * 1000L * 1000L;
		while (now() < sleepUntil)
			;

		OperatorConsoleCommandMessage sndMsg;
		OperatorConsoleResponseMessage rcvMsg;
		sndMsg.systemCommandType = OPCON_CONSOLE_COMMAND_GET_USER_COMMAND;
		MsgSend(coid, &sndMsg, sizeof(sndMsg), &rcvMsg, sizeof(rcvMsg));
		std::cout << rcvMsg.userCommandType << std::endl;
	}

	OperatorConsoleCommandMessage msg;
	msg.systemCommandType = COMMAND_EXIT_THREAD;
	MsgSend(coid, &msg, sizeof(msg), NULL, 0);
	// N.B.: The program will likely hang here until you press enter in the console one more time.
	// Can't do much about it, it's because std::getline is a blocking operation.
	pthread_join(tid, NULL);
}

void computerSystemDemo() {
	pthread_t compSystemTid, opConsoleTid, displayTid;
	PlaneStartParams params1 = { 1, 1, { 0, 50000, 20000 }, { 1000, 0, 0 } };
	PlaneStartParams params2 = { 2, 1, { 50000, 0, 20000 }, { 0, 1000, 0 } };
	PlaneStartParams params3 = { 3, 3, { 3, 3, 3 }, { 3, 3, 3 } };
	Plane plane1 = Plane(params1);
	Plane plane2 = Plane(params2);
	Plane plane3 = Plane(params3);
	vector<Plane> planes { plane1, plane2, plane3 };

	int numOfPlanes = planes.size();
	pthread_t planeThreads[numOfPlanes];

	for (size_t i = 0; i < planes.size(); i++) {
		pthread_create(&planeThreads[i], NULL, &Plane::start, &planes[i]);
	}

	ComputerSystem compSystem;
	OperatorConsole opConsole;
	DataDisplay display;

	CommunicationSystem commSystem = CommunicationSystem(planes);
	Radar radar = Radar(planes);

	compSystem.setRadar(radar);
	compSystem.setCommSystem(commSystem);
	compSystem.setCongestionDegreeSeconds(15);

	pthread_create(&opConsoleTid, NULL, &OperatorConsole::start, &opConsole);
	pthread_create(&displayTid, NULL, &DataDisplay::start, &display);
	while (opConsole.getChid() == -1 || display.getChid() == -1)
		;

	compSystem.setDisplayChid(display.getChid());
	compSystem.setOperatorChid(opConsole.getChid());
	pthread_create(&compSystemTid, NULL, &ComputerSystem::start, &compSystem);

	// If I don't sleep here the results are intermittent for the attach
	std::this_thread::sleep_for(std::chrono::milliseconds(1 * 1000));

	int compSystemCoid = 0;
	if ((compSystemCoid = ConnectAttach(0, 0, compSystem.getChid(), 0, 0))
			== -1) {
		std::cout << "ComputerSystem: failed to attach to. Exiting thread.";
		return;
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(60 * 1000));

	ComputerSystemMessage msg;
	msg.command = COMMAND_EXIT_THREAD;
	if (MsgSend(compSystemCoid, &msg, sizeof(msg), NULL, 0) == 0) {
		cout << "Shut down compSystem" << endl;
	} else {
		cout << "Unable to shut down compSystem." << endl;
	}
	ConnectDetach(compSystemCoid);
	pthread_join(compSystemTid, NULL);

	dataDisplayCommandMessage ddMsg;
	ddMsg.commandType = COMMAND_EXIT_THREAD;
	int ddCoid = ConnectAttach(0, 0, display.getChid(), _NTO_SIDE_CHANNEL, 0);
	MsgSend(ddCoid, &ddMsg, sizeof(ddMsg), NULL, 0);
	ConnectDetach(ddCoid);
	pthread_join(displayTid, NULL);

	OperatorConsoleCommandMessage ocMsg;
	ocMsg.systemCommandType = COMMAND_EXIT_THREAD;
	int ocCoid = ConnectAttach(0, 0, opConsole.getChid(), _NTO_SIDE_CHANNEL, 0);
	MsgSend(ocCoid, &ocMsg, sizeof(ocMsg), NULL, 0);
	ConnectDetach(ocCoid);
	pthread_join(opConsoleTid, NULL);

	for (size_t i = 0; i < planes.size(); i++) {
		PlaneCommandMessage exitMsg;
		exitMsg.command = COMMAND_EXIT_THREAD;
		int planeCoid = ConnectAttach(0, 0, planes[i].getChid(),
		_NTO_SIDE_CHANNEL, 0);
		MsgSend(planeCoid, &exitMsg, sizeof(exitMsg), NULL, 0);
		ConnectDetach(planeCoid);
		pthread_join(planeThreads[i], NULL);
	}

}

int main() {
	//planeDemo();
	computerSystemDemo();
//	OperatorConsoleDemo();
	return 0;
}
