#include <sys/dispatch.h>
#include <pthread.h>
#include <chrono>
#include <string>
#include <thread>
#include <fstream>
#include <sstream>
#include <iostream>
#include <time.h>

#include "ComputerSystem.h"
#include "Radar.h"
#include "Plane.h"
#include "OperatorConsole.h"
#include "DataDisplay.h"
#include "CommunicationSystem.h"
#include "InputStrings.h"
#include "constants.h"

// Create files on disk for the given test inputs.
void writeFiles() {
	int fdlow = creat("/data/home/qnxuser/lowload.txt",
	S_IRUSR | S_IWUSR | S_IXUSR);
	if (fdlow != -1) {
		write(fdlow, LOW_LOAD, strlen(LOW_LOAD));
		close(fdlow);
	}

	int fdmed = creat("/data/home/qnxuser/medload.txt",
	S_IRUSR | S_IWUSR | S_IXUSR);
	if (fdmed != -1) {
		write(fdmed, MED_LOAD, strlen(MED_LOAD));
		close(fdmed);
	}

	int fdhigh = creat("/data/home/qnxuser/highload.txt",
	S_IRUSR | S_IWUSR | S_IXUSR);
	if (fdhigh != -1) {
		write(fdhigh, HIGH_LOAD, strlen(HIGH_LOAD));
		close(fdhigh);
	}
}

// Read start parameters for each plane from a given file.
std::vector<PlaneStartParams> readFile(std::string filePath) {
	std::vector<PlaneStartParams> planes;
	std::ifstream input(filePath);
	if (input) {
		std::string line;
		while (std::getline(input, line)) {
			std::stringstream ss(line);
			int time, id, px, py, pz, vx, vy, vz;
			ss >> time >> id >> px >> py >> pz >> vx >> vy >> vz;
			PlaneStartParams p;
			p.id = id;
			p.arrivalTime = time;
			p.initialPosition = {px, py, pz};
			p.initialVelocity = {vx, vy, vz};
			planes.push_back(p);
		}
	} else {
		std::cout << "Could not open input file" << std::endl;
	}
	return planes;
}

void computerSystemDemo() {

	std::string choice = "";
	while (choice != "low" && choice != "medium" && choice != "high") {
		std::cout << "Enter the congestion level: [low,medium,high]: ";
		std::cin >> choice;
	}
	std::string filePath;
	if (choice == "low") {
		filePath = "/data/home/qnxuser/lowload.txt";
	} else if (choice == "medium") {
		filePath = "/data/home/qnxuser/medload.txt";
	} else if (choice == "high") {
		filePath = "/data/home/qnxuser/highload.txt";
	}

	// Initialize the planes (without starting their threads).
	std::vector<PlaneStartParams> params = readFile(filePath);
	std::vector<Plane> planes;
	for (size_t i = 0; i < params.size(); i++) {
		planes.push_back(Plane(params[i]));
	}

	pthread_t compSystemTid, opConsoleTid, displayTid;

	int numOfPlanes = planes.size();
	pthread_t planeThreads[numOfPlanes];

	// Start each plane thread one by one.
	for (size_t i = 0; i < planes.size(); i++) {
		pthread_create(&planeThreads[i], NULL, &Plane::start, &planes[i]);
	}

	// Initialize all other components of the system.
	ComputerSystem compSystem;
	OperatorConsole opConsole;
	DataDisplay display;

	CommunicationSystem commSystem = CommunicationSystem(planes);
	Radar radar = Radar(planes);

	compSystem.setRadar(radar);
	compSystem.setCommSystem(commSystem);
	compSystem.setCongestionDegreeSeconds(15);

	// Start the operator console and display threads, and wait for them to open up their message passing channels.
	pthread_create(&opConsoleTid, NULL, &OperatorConsole::start, &opConsole);
	pthread_create(&displayTid, NULL, &DataDisplay::start, &display);
	while (opConsole.getChid() == -1 || display.getChid() == -1)
		;

	// Give the computer system the necessary channel IDs for IPC then start its thread.
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

	// Run the simulation for 180 seconds.
	std::this_thread::sleep_for(std::chrono::seconds(TIME_RANGE_SECONDS));

	// Terminate the computer system.
	ComputerSystemMessage msg;
	msg.command = COMMAND_EXIT_THREAD;
	if (MsgSend(compSystemCoid, &msg, sizeof(msg), NULL, 0) == 0) {
		cout << "Shut down compSystem" << endl;
	} else {
		cout << "Unable to shut down compSystem." << endl;
	}
	ConnectDetach(compSystemCoid);
	pthread_join(compSystemTid, NULL);

	// Terminate the data display.
	dataDisplayCommandMessage ddMsg;
	ddMsg.commandType = COMMAND_EXIT_THREAD;
	int ddCoid = ConnectAttach(0, 0, display.getChid(), _NTO_SIDE_CHANNEL, 0);
	MsgSend(ddCoid, &ddMsg, sizeof(ddMsg), NULL, 0);
	ConnectDetach(ddCoid);
	pthread_join(displayTid, NULL);

	// Terminate the operator console.
	OperatorConsoleCommandMessage ocMsg;
	ocMsg.systemCommandType = COMMAND_EXIT_THREAD;
	int ocCoid = ConnectAttach(0, 0, opConsole.getChid(), _NTO_SIDE_CHANNEL, 0);
	MsgSend(ocCoid, &ocMsg, sizeof(ocMsg), NULL, 0);
	ConnectDetach(ocCoid);
	pthread_join(opConsoleTid, NULL);

	// Terminate each plane.
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
	std::string choice = "";
	while (choice != "write" && choice != "run") {
		std::cout
				<< "Enter 'write' to create the input files in the QNX VM. Enter 'run' to run the ATC simulation." << std::endl;
		cin >> choice;
	}
	if (choice == "write") {
		writeFiles();
	} else if (choice == "run") {
		computerSystemDemo();
	}
	return 0;
}
