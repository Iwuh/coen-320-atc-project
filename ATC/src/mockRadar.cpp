#include "mockRadar.h"
#include "ComputerSystem.h"
#include "Plane.h"

#define UPDATE_PLANE_COUNT 112
#define UPDATE_AIRSPACE 113
#define ADD_PLANE 114

Radar::Radar() :
		chid(-1) {

}

int Radar::getChid() const {



	return chid;
}

void Radar::run() {
	// Create a new communication channel belonging to the plane and store the handle in chid.
	if ((chid = ChannelCreate(0)) == -1) {
		std::cout << "ComputerSystem: channel creation failed. Exiting thread."
				<< std::endl;
		return;
	}
	int rcvid;


	//ComputerSystemMessage msg; // Replace this after, only for mocking

	PlaneCommandMessage msg;

	while (1) {
		rcvid = MsgReceive(chid, &msg, sizeof(msg), NULL);
		if (rcvid == 0) {
			break; //no pulse handling
		}
		switch (msg.command) {
		case UPDATE_PLANE_COUNT: {
			int numberOfPlanesInAirspace = radarResults.size();
			MsgReply(rcvid, EOK, &numberOfPlanesInAirspace,
					sizeof(numberOfPlanesInAirspace));
			break;
		}
		case UPDATE_AIRSPACE: {
			int numberOfPlanesInAirspace = radarResults.size();
			int i = 0;
			std::pair<int, PlanePositionResponse> radarResultsArray[numberOfPlanesInAirspace];
			for (auto const &x : radarResults) {
				radarResultsArray[i++] = pair<int, PlanePositionResponse>(
						x.first, x.second);

			}
			MsgReply(rcvid, EOK, &radarResultsArray, sizeof(radarResultsArray));
			break;
		}
		case COMMAND_EXIT_THREAD:
			cout << "Received exit command" << endl;
			// Required to allow all threads to gracefully terminate when the program is terminating
			MsgReply(rcvid, EOK, NULL, 0);
			return;
		default:
			break;
		}
	}
}

void Radar::addPlaneToAirspace(int i, PlanePositionResponse p) {


	std::cout << "\n -----------Flights------------\n";
	std::cout << "\n Flight ID : " << i << "\n";
	std::cout << "\n Position: <" << p.currentPosition.x << ',' << p.currentPosition.y << ',' << p.currentPosition.z << ">, \n";
	std::cout << "\n Velocity: <" << p.currentVelocity.x << ',' << p.currentVelocity.y << ',' << p.currentVelocity.z << ">\n";


	radarResults.insert(pair<int, PlanePositionResponse>(i, p));

	int coid;
	// Change the plane's velocity.
		PlaneCommandMessage changeMsg;
		changeMsg.command = COMMAND_SET_VELOCITY;
		MsgSend(coid, &changeMsg, sizeof(changeMsg), NULL, 0);

		// Ping the plane
		for (int i = 0; i < 1; i++)
		{
			PlaneCommandMessage msg;
			msg.command = COMMAND_RADAR_PING;
			PlanePositionResponse res;
			MsgSend(coid, &msg, sizeof(msg), &res, sizeof(res));
			std::cout << "\n Updated Position: <" << res.currentPosition.x << ',' << res.currentPosition.y << ',' << res.currentPosition.z << "> \n ";
			std::cout << "\n Updated Velocity: <" << res.currentVelocity.x << ',' << res.currentVelocity.y << ',' << res.currentVelocity.z << "> \n";
			std::cout << std::endl;


		}

}



void* Radar::start(void *context) {
	auto cs = (Radar*) context;
	cs->run();
	return NULL;
}
