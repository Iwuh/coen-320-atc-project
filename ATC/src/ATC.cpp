#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/dispatch.h>
#include <pthread.h>

#include "ComputerSystem.h"
#include "mockRadar.h"

#include <iostream>
#include <time.h>
#include "Plane.h"

int64_t now()
{
	struct timespec now;
	clock_gettime(CLOCK_MONOTONIC, &now);
	return now.tv_sec*1000*1000*1000 + now.tv_nsec;
}

void planeDemo()
{
	PlaneStartParams params
	{
		1, // id
		5, // arrival time
		{1,1,1}, // initial position
		{1,1,1} // initial velocity
	};
	Plane myPlane(params);

	pthread_t tid;
	pthread_create(&tid, NULL, &Plane::start, &myPlane);

	int coid = ConnectAttach(0, 0, myPlane.getChid(), _NTO_SIDE_CHANNEL, 0);
	int64_t sleepUntil;
	for (int i = 0; i < 15; i++)
	{
		PlaneCommandMessage msg;
		msg.command = COMMAND_RADAR_PING;
		PlanePositionResponse res;
		MsgSend(coid, &msg, sizeof(msg), &res, sizeof(res));
		std::cout << "Position: <" << res.currentPosition.x << ',' << res.currentPosition.y << ',' << res.currentPosition.z << ">, ";
		std::cout << "Velocity: <" << res.currentVelocity.x << ',' << res.currentVelocity.y << ',' << res.currentVelocity.z << ">";
		std::cout << std::endl;

		sleepUntil = now() + 1000*1000*1000;
		while (now() < sleepUntil);
	}
	PlaneCommandMessage msg;
	msg.command = COMMAND_EXIT_THREAD;
	MsgSend(coid, &msg, sizeof(msg), NULL, 0);
	pthread_join(tid, NULL);
}

void computerSystemDemo()
{
	pthread_t tid, mockRadarTid;
	ComputerSystem compSystem;
	Radar mockRadar;
	pthread_create(&mockRadarTid, NULL, &Radar::start, &mockRadar);
	compSystem.setRadarChid(mockRadar.getChid());
	pthread_create(&tid, NULL, &ComputerSystem::start,&compSystem);

	int compSystemCoid = ConnectAttach(0, 0, compSystem.getChid(), _NTO_SIDE_CHANNEL, 0);
	int mockRadarCoid = ConnectAttach(0, 0, mockRadar.getChid(), _NTO_SIDE_CHANNEL, 0);

		int64_t sleepUntil;
		for (int i = 0; i < 15; i++)
		{
			PlanePositionResponse p = {
					{1,1,1}, // initial position
					{1,1,1}
			};
			mockRadar.addPlaneToAirspace(i,p);
			sleepUntil = now() + 1000*1000*1000;
			while (now() < sleepUntil);
		}
		ComputerSystemMessage msg;
		msg.command = COMMAND_EXIT_THREAD;
		MsgSend(compSystemCoid, &msg, sizeof(msg), NULL, 0);
		MsgSend(mockRadarCoid, &msg, sizeof(msg), NULL, 0);

	pthread_join(tid, NULL);
	pthread_join(mockRadarTid, NULL);
}

int main() {

	planeDemo();
	computerSystemDemo();
	return 0;
}
