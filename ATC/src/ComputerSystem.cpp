#include "ComputerSystem.h"
#include <sys/siginfo.h>
#include <time.h>
#include <fstream>
#include "commandCodes.h"
#include "constants.h"
#include "inlineStaticHelpers.h"

ComputerSystem::ComputerSystem() :
		chid(-1), operatorChid(-1), radarChid(-1), displayChid(-1) {
}

int ComputerSystem::getChid() const {
	return chid;
}

void ComputerSystem::setOperatorChid(int id) {
	operatorChid = id;
}

void ComputerSystem::setRadarChid(int id) {
	radarChid = id;
}

void ComputerSystem::setDisplayChid(int id) {
	displayChid = id;
}

void ComputerSystem::setCongestionDegreeSeconds(int congestionDegreeSeconds) {
	this->congestionDegreeSeconds = congestionDegreeSeconds;
}

void ComputerSystem::run() {
	createPeriodicTasks();
	// Start listening for messages
	listen();
}

void ComputerSystem::createPeriodicTasks() {
	/*
	 * The following periodic tasks are to be performed by the computer system:
	 * Calculate airspace violation constraints - every 1 second
	 * Save the airspace state and operator requests to the logfile - every 30 seconds
	 * Codes corresponding to the index of the timer in the array are:
	 * AIRSPACE_VIOLATION_CONSTRAINT_TIMER 0
	 * LOG_AIRSPACE_TIMER 1
	 */

	periodicTask periodicTasks[COMPUTER_SYSTEM_NUM_PERIODIC_TASKS] =
			{ { AIRSPACE_VIOLATION_CONSTRAINT_TIMER, 2 }, { LOG_AIRSPACE_TIMER,
					5 } };

	// Create a new communication channel belonging to the plane and store the handle in chid.
	if ((chid = ChannelCreate(0)) == -1) {
		std::cout << "ComputerSystem: channel creation failed. Exiting thread."
				<< std::endl;
		return;
	}

	// Open a client to our own connection to be used for timer pulses and store the handle in coid.
	int coid;
	if ((coid = ConnectAttach(0, 0, chid, 0, 0)) == -1) {
		std::cout
				<< "ComputerSystem: failed to attach to self. Exiting thread.";
		return;
	}

	for (int i = 0; i < COMPUTER_SYSTEM_NUM_PERIODIC_TASKS; i++) {
		periodicTask pt = periodicTasks[i];
		struct sigevent sigev;
		timer_t timer;
		SIGEV_PULSE_INIT(&sigev, coid, SIGEV_PULSE_PRIO_INHERIT, pt.timerCode,
				0);
		if (timer_create(CLOCK_MONOTONIC, &sigev, &timer) == -1) {
			std::cout
					<< "ComputerSystem: failed to initialize timer. Exiting thread.";
			return;
		}
		// Set the timer to fire once at the arrival time, and then every second thereafter.
		struct itimerspec timerValue;
		timerValue.it_value.tv_sec = 1;
		timerValue.it_value.tv_nsec = 0;
		timerValue.it_interval.tv_sec = pt.taskIntervalSeconds;
		timerValue.it_interval.tv_nsec = 0;

		// Start the timer.
		timer_settime(timer, 0, &timerValue, NULL);
	}

}

void ComputerSystem::listen() {
	int rcvid;
	ComputerSystemMessage msg;
	while (1) {
		// Wait for any type of message.
		rcvid = MsgReceive(chid, &msg, sizeof(msg), NULL);
		if (rcvid == 0) {
			// Handle internal switches from the pulses of the various timers.
			switch (msg.header.code) {
			case LOG_AIRSPACE_TIMER:
//				logSystem();
				break;
			case AIRSPACE_VIOLATION_CONSTRAINT_TIMER:
				violationCheck();
				break;
			default:
				std::cout
						<< "ComputerSystem: received pulse with unknown code: "
						<< msg.header.code << " and unknown command: "
						<< msg.command << std::endl;
				break;
			}
		} else {
			// Handle messages from external processes
			switch (msg.command) {
			case COMMAND_OPERATOR_REQUEST:
				MsgSend(operatorChid, &msg, sizeof(msg), NULL, 0);
				break;
			case COMMAND_EXIT_THREAD:
				// Required to allow all threads to gracefully terminate when the program is terminating
				cout << "Received EXIT command";
				MsgReply(rcvid, EOK, NULL, 0);
				return;
			default:
				std::cout << "ComputerSystem: received unknown command "
						<< msg.command << std::endl;
				MsgError(rcvid, ENOSYS);
				break;
			}
		}
	}
}

void ComputerSystem::logSystem() {
	ofstream logfile;
	printCurrentTime();
	cout << endl;
	for (auto const &x : airspace) {
		logfile.open("logfile.txt");
		logfile << x.first << ":" << std::to_string(x.second.currentPosition.x)
				<< "," << std::to_string(x.second.currentPosition.y) << ","
				<< std::to_string(x.second.currentPosition.z) << "|";
		printf(" %d:%s,%s,%s |", x.first,
				std::to_string(x.second.currentPosition.x),
				std::to_string(x.second.currentPosition.y),
				std::to_string(x.second.currentPosition.z));
		if (x.first % 5 == 0 && x.first != 0) {
			cout << endl;
		}
	}
	cout << endl;
	logfile << endl;
	logfile.close();
}

void ComputerSystem::violationCheck() {
	printf("Violation check\n");
	ComputerSystemMessage msg;
	msg.command = COMMAND_UPDATE_PLANE_COUNT;
	//Update airspace via message-passing from radar
	//Request the number of planes first
	int numberOfPlanesInAirspace = 0;
	int coid = ConnectAttach(0, 0, radarChid, _NTO_SIDE_CHANNEL, 0);
	if (MsgSend(coid, &msg, sizeof(msg), &numberOfPlanesInAirspace,
			sizeof(numberOfPlanesInAirspace)) == -1) {
		cout << "Couldn't update airspace count";
		exit(-1); //TODO: Error scenarios
	}
	std::pair<int, PlanePositionResponse> radarResults[numberOfPlanesInAirspace];

	msg.command = COMMAND_UPDATE_AIRSPACE;
	if (MsgSend(coid, &msg, sizeof(msg), radarResults, sizeof(radarResults))
			== -1) {
		cout << "Couldn't update airspace";
		exit(-1); //TODO: Error scenarios
	}

	for (int i = 0; i < numberOfPlanesInAirspace; i++) {
		auto const result = airspace.insert(radarResults[i]);
		if (not result.second) { //This will update the value if the value already existed
			result.first->second = radarResults[i].second;
		}
	}
	//Perform sequential validation, in case of a collision send out an alert to the operator and an update to the display
	for (int i = 0; i < numberOfPlanesInAirspace; i++) {
		for (int j = i + 1; j < numberOfPlanesInAirspace; j++) {
			checkForFutureViolation(radarResults[i], radarResults[j]);
		}
	}
}

void ComputerSystem::checkForFutureViolation(
		std::pair<int, PlanePositionResponse> plane1,
		std::pair<int, PlanePositionResponse> plane2) {
	// Based on the doc:
	// for all aircrafts in space, aircrafts must have a distance no less than
	// 1000 units in height, 3000 units in width/length
	// verify at time T + congestionDegreeSeconds for potential collision range
	int VERTICAL_LIMIT = 1000;
	int HORIZONTAL_LIMIT = 3000;
	Vec3 plane1posInCongestionSeconds = plane1.second.currentPosition.sum(plane1.second.currentVelocity.scalarMultiplication(congestionDegreeSeconds));
	Vec3 plane2posInCongestionSeconds = plane2.second.currentPosition.sum(plane2.second.currentVelocity.scalarMultiplication(congestionDegreeSeconds));
	Vec3 distancesBetweenPlanes = plane1posInCongestionSeconds.absoluteDiff(plane2posInCongestionSeconds);
	if (distancesBetweenPlanes.x <= HORIZONTAL_LIMIT || distancesBetweenPlanes.y <= HORIZONTAL_LIMIT || distancesBetweenPlanes.z <= VERTICAL_LIMIT){
		// raise alert
		cout << "ALERT" << endl;

	}
}

Vec3 ComputerSystem::getDirectionVector(
		std::pair<int, PlanePositionResponse> plane) {
	Vec3 startPoint = plane.second.currentPosition;
	Vec3 endPoint = getEndCoordinate(plane);
	return endPoint.diff(startPoint);
}

Vec3 ComputerSystem::getEndCoordinate(
		std::pair<int, PlanePositionResponse> plane) {
	Vec3 initPos = plane.second.currentPosition;
	Vec3 velocity = plane.second.currentVelocity;
	return initPos.sum(velocity);
}

void* ComputerSystem::start(void *context) {
	auto cs = (ComputerSystem*) context;
	cs->run();
	return NULL;
}
