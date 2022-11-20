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

int main() {

	planeDemo();
	return 0;
}
